//----------------------------------------------------------------------
//
//			File:			"textCursor.cpp"
//			Created:		12-Apr-2011
//			Author:			津田伸秀
//			Description:
//
//----------------------------------------------------------------------

#include	"textCursor.h"
#include	"TextDocument.h"
#include	"TextView.h"
#include	"viCursor.h"

int firstNonBlankCharPos(const QString &text);
void moveToFirstNonBlankChar(DocCursor &cur, uchar mode)
{
	DocBlock block = cur.block();
	const int blockPos = block.position();
	const QString blockText = block.text();
	if( !blockText.isEmpty() )
		cur.setPosition(blockPos + firstNonBlankCharPos(blockText), mode);
}
inline bool isUTF8FirstChar(uchar ch)
{
	return !(ch & 0x80) || (ch & 0x40) != 0;
}
//	UTF-8データの最初のバイトにより文字バイト数を計算
size_t UTF8CharSize(uchar ch)
{
	if( !(ch & 0x80) ) return 1;
	if( !(ch & 0x20) ) return 2;
	if( !(ch & 0x10) ) return 3;
	if( !(ch & 0x08) ) return 4;
	if( !(ch & 0x04) ) return 5;
	if( !(ch & 0x02) ) return 6;
	Q_ASSERT( 0 );
	return 1;
}
int getEOLOffset(const QString text)
{
	int ix = text.length();
	if( text.isEmpty() ) return ix;
	if( text[ix-1] == '\r' )
		return ix - 1;
	if( text[ix-1] == '\n' ) {
		if( ix > 1 && text[ix-2] == '\r' )
			return ix - 2;
		else
			return ix - 1;
	}
	return ix;
}
size_t getEOLOffset(const TextDocument *doc, BlockData d)
{
	if( doc == 0 ) return 0;
	const size_t sz = doc->blockSize(d.m_index);
	index_t np = d.position() + sz;
	if( np > d.position() ) {
		uchar uch = (*doc)[np-1];
		if( uch == '\r' )
			return sz - 1;
		if( uch == '\n' ) {
			if( np - 2 >= d.position() && (*doc)[np-2] == '\r' )
				return sz - 2;
			else
				return sz - 1;
		}
	}
	return sz;
}
//----------------------------------------------------------------------
#if TEXT_CURSOR_BLOCK
void DocCursor::updateBlockData(uchar mode)
{
	if( !m_document ) {
		m_blockData = BlockData(0, 0);
	} else {
		m_blockData = m_document->findBlockData(m_position);
		if( mode == MoveAnchor )
			m_anchorBlockData = m_blockData;
	}
}
#endif

QString DocCursor::selectedText() const
{
	if( isNull() || !hasSelection() )
		return QString();
	index_t first, last;
	if( m_anchor < m_position ) {
		first = m_anchor;
		last = m_position;
	} else {
		first = m_position;
		last = m_anchor;
	}
	QByteArray ba;
	ba.reserve(last - first);
	while( first != last )
		ba += (*m_document)[first++];
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	return codec->toUnicode(ba);
}
int DocCursor::prevCharsCount() const
{
	if( isNull() ) return 0;
	int cnt = 0;
	index_t pos = m_blockData.position();
	while( pos < m_position ) {
		++cnt;
		pos += UTF8CharSize((*m_document)[pos]);
	}
	return cnt;
}
DocBlock DocCursor::block() const
{
	return DocBlock(m_document, m_blockData);
}

void DocCursor::copyPositionToAnchor()
{
	m_anchor = m_position;
	m_anchorBlockData = m_blockData;
	//m_ancBlockIndex = m_blockIndex;
	//m_ancBlockPosition = m_blockPosition;
}
void DocCursor::copyAnchorToPosition()
{
	m_position = m_anchor;
	m_blockData = m_anchorBlockData;
	//m_blockIndex = m_ancBlockIndex;
	//m_blockPosition = m_ancBlockPosition;
}

void DocCursor::swapPositionAnchor()
{
	index_t t;
	t = m_position; m_position = m_anchor; m_anchor = t;
	BlockData b;
	b = m_blockData; m_blockData = m_anchorBlockData; m_anchorBlockData = b;
	//t = m_blockIndex; m_blockIndex = m_ancBlockIndex; m_ancBlockIndex = t;
	//t = m_blockPosition; m_blockPosition = m_ancBlockPosition; m_ancBlockPosition = t;
}

void DocCursor::setAnchor(index_t anchor)
{
	m_anchor = anchor;
	m_anchorBlockData = document()->findBlockData(anchor);
}
void DocCursor::setPosition(index_t position, uchar mode)
{
	if( isNull() ) return;
	m_position = position;
#if TEXT_CURSOR_BLOCK
	updateBlockData(KeepAnchor);
	m_offset = m_position - m_blockData.position();
	if( mode == MoveAnchor ) {
		m_anchor = m_position;
		m_anchorBlockData = m_blockData;
	}
#else
	if( mode == MoveAnchor )
		m_anchor = m_position;
#endif
}
#if TEXT_CURSOR_BLOCK
void DocCursor::setPosition(index_t position, BlockData d, uchar mode)
{
	if( isNull() ) return;
	m_position = position;
	m_blockData = d;
	m_offset = m_position - m_blockData.position();
	if( mode == MoveAnchor ) {
		m_anchor = m_position;
		m_anchorBlockData = m_blockData;
		//m_ancBlockIndex = m_blockIndex;
		//m_ancBlockPosition = m_blockPosition;
	}
}
#endif
void DocCursor::move(int d)
{
	if( isNull() ) return;
	m_position += d;
	m_anchor += d;
	m_blockData = m_document->findBlockData(m_position);
	if( m_position == m_anchor )
		m_anchorBlockData = m_blockData;
	else
		m_anchorBlockData = m_document->findBlockData(m_anchor);
}
//----------------------------------------------------------------------
static uchar sbCharTypeTbl[] = {
/* 0 */	CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER,
		CT_OTHER, CT_SPACE, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER,
/* 1 */	CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER,
		CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER,
/* 2 */	CT_SPACE, CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,		//	  ! " # $ % & '
		CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,		//	( ) * + , - . /
/* 3 */	CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM,		//	0 CT_ALNUM 2 3 4 5 6 7
		CT_ALNUM, CT_ALNUM, CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,		//	8 9 : ; < = > ?
/* 4 */	CT_SYM,   CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM,		//	@ A B C D E F G
		CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM,		//	H I J K L M N O
/* 5 */	CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM,		//	P Q R S T U V W
		CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_ALNUM,		//	X Y Z [ \ ] ^ _
/* 6 */	CT_SYM,   CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM,		//	` a b c d e f g
		CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM,		//	h i j k l m n o
/* 7 */	CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM,		//	p q r s t u v w
		CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,		//	x y z { | } ~
};

uchar getCharType(QChar ch)
{
	const ushort code = ch.unicode();
	if( code < 0x80 )
		return sbCharTypeTbl[code];
	if( code <= 0xa0 ) return CT_OTHER;
	if( code <= 0xbf ) return CT_SYM;
	if( code <= 0x2af ) return CT_ALNUM;
	if( code <= 0x36f ) return CT_GREEK;
	if( code >= 0x3040 && code < 0x30a0 ) return CT_HIRA;
	if( code >= 0x30a0 && code < 0x3100 ) return CT_KANA;
	if( code >= 0x4e00 && code < 0xa000 ) return CT_KANJI;
	return CT_SYM;
}

//----------------------------------------------------------------------
bool gotoNextWord(DocCursor &cur, int n = 1 , bool vi = false, bool cdy = false);
bool gotoPrevWord(DocCursor &cur, int n = 1, bool vi = false);
bool gotoNextWordEnd(DocCursor &cur, int n, bool cdy);
bool gotoStartOfWord(DocCursor &cur);
bool gotoEndOfWord(DocCursor &cur);
inline bool isTabOrSpace(const QChar ch)
{
	return ch == '\t' || ch == ' ';
}

bool gotoNextWord(DocCursor &cur, int n , bool vi, bool cdy)
{
	const TextDocument *doc = cur.document();
	DocBlock block = cur.block();
	//int blockPos = block.position();
	QString text = block.text();
	int limitIndex = vi ? getEOLOffset(text) : text.length();
	int pos = cur.position();
	int ix = cur.prevCharsCount();
	while( --n >= 0 ) {
		//	同タイプ文字を読み飛ばす
		if( ix < limitIndex && !isTabOrSpace(text[ix]) ) {
			uchar cat = getCharType(text[ix++]);
			pos += UTF8CharSize((*doc)[pos]);
			//QChar::Category cat = text[ix++].category();
			while( ix < limitIndex && !isTabOrSpace(text[ix]) && getCharType(text[ix]) == cat ) {
				++ix;
				pos += UTF8CharSize((*doc)[pos]);
			}
		}
		//	空白類を読み飛ばす
		while( ix == limitIndex || isTabOrSpace(text[ix]) ) {
			if( ix == limitIndex ) {
				if( cdy && !n )	//	cdy が前置されている場合は、最後の改行はスキップしない
					break;
				do {
					DocBlock nb = block.next();
					if( !nb.isValid() ) {
						cur.setPosition(pos, block.data(), DocCursor::KeepAnchor);
						return true;
					}
					block = nb;
					pos = block.position();
					text = block.text();
					limitIndex = vi ? getEOLOffset(text) : text.length();
				} while( vi && !limitIndex );
				ix = 0;
			} else {
				++ix;
				pos += UTF8CharSize((*doc)[pos]);
			}
		}
	}
	cur.setPosition(pos, block.data(), DocCursor::KeepAnchor);
	return true;
}
bool gotoPrevWord(DocCursor &cur, int n, bool vi)
{
	const TextDocument *doc = cur.document();
	DocBlock block = cur.block();
	//int blockPos = block.position();
	QString text = block.text();
	int pos = cur.position();
	int ix = cur.prevCharsCount();
	while( --n >= 0 ) {
		//	ひとつ前の文字が空白類 or 行頭なら文書先頭方向に移動
		while( !ix || isTabOrSpace(text[ix-1]) ) {
			if( !ix ) {
				DocBlock pb = block.prev();
				if( !pb.isValid() ) {
					cur.setPosition(pos, block.data(), DocCursor::KeepAnchor);
					return true;
				}
				block = pb;
				//blockPos = block.position();
				pos = block.position() + block.EOLOffset();
				if( !vi ) {
					//	非viモードでは n は無視とする
					cur.setPosition(pos, block.data(), DocCursor::KeepAnchor);
					return true;
				}
				text = block.text();
				ix = getEOLOffset(text);
			} else {
				--ix;
				do { } while( !isUTF8FirstChar((*doc)[--pos]) );
			}
		}
		//	ひとつ前の文字が同じタイプ and 空白類になるまで or 行頭まで読み飛ばす
		if( ix > 0 && !isTabOrSpace(text[ix-1]) ) {
			uchar cat = getCharType(text[--ix]);
			do { } while( !isUTF8FirstChar((*doc)[--pos]) );
			while( ix > 0 && !isTabOrSpace(text[ix-1]) && getCharType(text[ix-1]) == cat ) {
				--ix;
				do { } while( !isUTF8FirstChar((*doc)[--pos]) );
			}
		}
	}
	cur.setPosition(pos, block.data(), DocCursor::KeepAnchor);
	return true;
}
bool gotoNextWordEnd(DocCursor &cur, int n, bool cdy)
{
	const TextDocument *doc = cur.document();
	DocBlock block = cur.block();
	index_t pos = cur.position();
	index_t blockPos = block.position();
	QString text = block.text();
	int ix = cur.prevCharsCount();
	int EOLIndex = getEOLOffset(text);
	while( --n >= 0 ) {
		//	次の文字が空白類 or 行末なら文書末尾方向に移動
		while( ix + 1 >= EOLIndex || isTabOrSpace(text[ix+1]) ) {
			if( ix + 1 >= EOLIndex ) {
				block = block.next();
				if( !block.isValid() ) {
					cur.setPosition(blockPos);
					return true;
				}
				pos = blockPos = block.position();
				text = block.text();
				EOLIndex = getEOLOffset(text);
				ix = 0;
				if( ix < EOLIndex && !isTabOrSpace(text[ix]) )
					break;
			} else {
				++ix;
				pos += UTF8CharSize((*doc)[pos]);
			}
		}
		//	次の文字が空白類になるまで or 行末まで読み飛ばす
		if( ix + 1 < EOLIndex && !isTabOrSpace(text[ix+1]) ) {
			uchar cat = getCharType(text[++ix]);
			pos += UTF8CharSize((*doc)[pos]);
			while( ix + 1 < EOLIndex && !isTabOrSpace(text[ix+1]) && getCharType(text[ix+1]) == cat ) {
				++ix;
				pos += UTF8CharSize((*doc)[pos]);
			}
		}
	}
	if( cdy ) {
		++ix;
		pos += UTF8CharSize((*doc)[pos]);
	}
	cur.setPosition(pos, block.data(), DocCursor::KeepAnchor);
	return true;
}
bool gotoNextSSWord(DocCursor &cur, int n, bool cdy)
{
	const TextDocument *doc = cur.document();
	DocBlock block = cur.block();
	index_t pos = cur.position();
	index_t blockPos = block.position();
	QString text = block.text();
	int EOLIndex = getEOLOffset(text);
	int ix = cur.prevCharsCount();
	while( --n >= 0 ) {
		//	空白類まで読み飛ばす
		while( ix < EOLIndex && !isTabOrSpace(text[ix]) ) {
			++ix;
			pos += UTF8CharSize((*doc)[pos]);
		}
		//	空白類を読み飛ばす
		while( ix == EOLIndex || isTabOrSpace(text[ix]) ) {
			if( ix == EOLIndex ) {
				if( cdy && !n )	//	cdy が前置されている場合は、最後の改行はスキップしない
					break;
				do {
					block = block.next();
					if( !block.isValid() ) {
						cur.setPosition(pos);
						return true;
					}
					pos = block.position();
					blockPos = block.position();
					text = block.text();
					EOLIndex = getEOLOffset(text);
				} while( !EOLIndex );
				ix = 0;
			} else {
				++ix;
				pos += UTF8CharSize((*doc)[pos]);
			}
		}
	}
	cur.setPosition(pos, block.data(), DocCursor::KeepAnchor);
	return true;
}
bool gotoNextSSWordEnd(DocCursor &cur, int n, bool cdy)
{
	const TextDocument *doc = cur.document();
	index_t pos = cur.position();
	DocBlock block = cur.block();
	index_t blockPos = block.position();
	QString text = block.text();
	int EOLIndex = getEOLOffset(text);
	int ix = cur.prevCharsCount();
	while( --n >= 0 ) {
		//	次の文字が空白類 or 行末なら文書末尾方向に移動
		while( ix + 1 >= EOLIndex || isTabOrSpace(text[ix+1]) ) {
			if( ix + 1 >= EOLIndex ) {
				do {
					block = block.next();
					if( !block.isValid() ) {
						cur.setPosition(blockPos);
						return true;
					}
					pos = blockPos = block.position();
					text = block.text();
					EOLIndex = getEOLOffset(text);
				} while( !EOLIndex );
				ix = 0;
				if( ix < EOLIndex && !isTabOrSpace(text[ix]) )
					break;
			} else {
				++ix;
				pos += UTF8CharSize((*doc)[pos]);
			}
		}
		//	次の文字が空白類になるまで or 行末まで読み飛ばす
		while( ix + 1 < EOLIndex && !isTabOrSpace(text[ix+1]) ) {
			++ix;
			pos += UTF8CharSize((*doc)[pos]);
		}
	}
	if( cdy ) {
		++ix;
		pos += UTF8CharSize((*doc)[pos]);
	}
	cur.setPosition(pos, block.data(), DocCursor::KeepAnchor);
	return true;
}
bool gotoPrevSSWord(DocCursor &cur, int n)
{
	const TextDocument *doc = cur.document();
	index_t pos = cur.position();
	DocBlock block = cur.block();
	int blockPos = block.position();
	QString text = block.text();
	int ix = cur.prevCharsCount();
	while( --n >= 0 ) {
		//	ひとつ前の文字が空白類 or 行頭なら文書先頭方向に移動
		while( !ix || isTabOrSpace(text[ix-1]) ) {
			if( !ix ) {
				while( !ix ) {
					block = block.prev();
					if( !block.isValid() ) {
						cur.setPosition(blockPos);
						return true;
					}
					blockPos = block.position();
					pos = block.position() + block.EOLOffset();
					text = block.text();
					ix = getEOLOffset(text);
				}
			} else {
				--ix;
				do { } while( !isUTF8FirstChar((*doc)[--pos]) );
			}
		}
		//	ひとつ前の文字が空白類になるまで or 行頭まで読み飛ばす
		while( ix > 0 && !isTabOrSpace(text[ix-1]) ) {
			--ix;
			do { } while( !isUTF8FirstChar((*doc)[--pos]) );
		}
	}
	cur.setPosition(pos, block.data(), DocCursor::KeepAnchor);
	return true;
}
bool gotoStartOfWord(DocCursor &cur)
{
	const TextDocument *doc = cur.document();
	DocBlock block = cur.block();
	QString text = block.text();
	int pos = cur.position();
	int ix = cur.prevCharsCount();
	//	ひとつ前の文字が同じタイプ and 空白類になるまで or 行頭まで読み飛ばす
	if( !ix || isTabOrSpace(text[ix-1]) ) return false;
	const uchar cat = getCharType(text[ix]);
	if( cat != getCharType(text[--ix]) ) return false;	//	単語先頭にいる場合
	do { } while( !isUTF8FirstChar((*doc)[--pos]) );
	while( ix > 0 && !isTabOrSpace(text[ix-1]) && getCharType(text[ix-1]) == cat ) {
		--ix;
		do { } while( !isUTF8FirstChar((*doc)[--pos]) );
	}
	cur.setPosition(pos, block.data(), DocCursor::KeepAnchor);
	return true;
}
bool gotoEndOfWord(DocCursor &cur)
{
	const TextDocument *doc = cur.document();
	DocBlock block = cur.block();
	//int blockPos = block.position();
	QString text = block.text();
	int pos = cur.position();
	int ix = cur.prevCharsCount();
	//	同タイプ文字を読み飛ばす
	if( ix < text.length() && !isTabOrSpace(text[ix]) ) {
		uchar cat = getCharType(text[ix++]);
		pos += UTF8CharSize((*doc)[pos]);
		//QChar::Category cat = text[ix++].category();
		while( ix < text.length() && !isTabOrSpace(text[ix]) && getCharType(text[ix]) == cat ) {
			++ix;
			pos += UTF8CharSize((*doc)[pos]);
		}
	}
	cur.setPosition(pos, block.data(), DocCursor::KeepAnchor);
	return true;
}
void DocCursor::moveLeftIfEndOfLine()
{
	DocBlock b = block();
	uchar uch;
	if( m_position > b.position() &&
		(uch = (*m_document)[m_position]) == '\r' ||
		uch == '\n' )
	{
		do { } while( !isUTF8FirstChar((*m_document)[--m_position]) );
		m_anchor = m_position;
	}
}
//	vi 移動コマンドでは、空行でない場合は改行位置にカーソル移動しない
//	が、{c|d|y}<move> の場合は、改行まで移動しないといけない
//	{c|d|y}<move> なのか単なる <move> なのかを引数で与えることも可能だが、
//	それだと判定箇所が多数になってしまう
//	そこで、movePosition() は常に改行まで移動することにし、
//	movePosition() を呼び出す部分で、単なる <move> の場合は、改行からひとつ戻すことにする

bool DocCursor::movePosition(uchar move, uchar mode, uint n, bool cdy)
{
	if( isNull() ) return false;
	bool vi = false;
	DocBlock b = block();
	index_t blockPos = b.position();
	QString blockText = b.text();
	switch( move ) {
	case Right:
		if( m_document->size() - m_position <= n ) {
			//	1文字==1byte とは限らないのでこの処理だけでは不十分
			m_position = m_document->size();
			m_blockData = m_document->prevBlockData(BlockData(m_document->blockCount(), m_position));
			//updateBlockData(KeepAnchor);
		} else {
			//	for UTF-8
			const size_t sz = m_document->size();
			while( n != 0 && m_position < sz) {
				if( (*m_document)[m_position] == '\r' &&
					m_position + 1 < sz && 
					(*m_document)[m_position+1] == '\n' )
				{
					m_position += 2;
				} else
					m_position += UTF8CharSize((*m_document)[m_position]);
				--n;
			}
			//	for UTF-16
			//m_position += n;
			index_t next;
			index_t bixLimit = m_document->blockCount() - 1;
			while( m_blockData.m_index < bixLimit && 
				m_position >= (next = m_blockData.m_position + m_document->blockSize(m_blockData.m_index)) )
			{
				++m_blockData.m_index;
				m_blockData.m_position = next;
			}
		}
#if TEXT_CURSOR_BLOCK
		m_offset = m_position - m_blockData.position();
#endif
		break;
	case Left:
		if( n >= m_position ) {
			m_position = 0;
			m_blockData.m_index = 0;
			m_blockData.m_position = 0;
		} else {
			while( n != 0 && m_position != 0 ) {
				if( (*m_document)[m_position - 1] == '\n' &&
					m_position > 1 &&
					(*m_document)[m_position - 2] == '\r' )
				{
					m_position -= 2;
				} else
					do { } while( !isUTF8FirstChar((*m_document)[--m_position]) );
				--n;
			}
			while( m_blockData.m_index > 0 && m_blockData.m_position > m_position ) {
				m_blockData.m_position -= m_document->blockSize(--m_blockData.m_index);
			}
		}
		m_offset = m_position - m_blockData.position();
		break;
	case ViMoveOperation::Up:	//	暫定コード
	case Up:
	case PrevBlock:
		while( n != 0 ) {
			if( !m_blockData.m_index ) break;
			//	undone B 暫定コード
			m_position = m_blockData.m_position -= m_document->blockSize(--m_blockData.m_index);
			--n;
		}
		m_position = m_blockData.m_position + qMin(m_offset, getEOLOffset(m_document, m_blockData));
		break;
	case ViMoveOperation::Down:	//	暫定コード
	case NextBlock:
	case Down:
		while( n != 0 ) {
			if( m_blockData.m_index >= m_document->blockCount() - 1 ) break;
			//	undone B 暫定コード
			m_position = m_blockData.m_position += m_document->blockSize(m_blockData.m_index++);
			--n;
		}
		m_position = m_blockData.m_position + qMin(m_offset, getEOLOffset(m_document, m_blockData));
		break;
	case StartOfWord:
		gotoStartOfWord(*this);
		m_offset = m_position - m_blockData.position();
		break;
	case EndOfWord:
		gotoEndOfWord(*this);
		m_offset = m_position - m_blockData.position();
		break;
	case ViMoveOperation::NextWord:
		vi = true;
		//	下にスルー
	case NextWord:
		gotoNextWord(*this, n, vi, cdy);
		m_offset = m_position - m_blockData.position();
		break;
	case ViMoveOperation::PrevWord:
		vi = true;
	case PrevWord:
		gotoPrevWord(*this, n, vi);
		m_offset = m_position - m_blockData.position();
		break;
	case ViMoveOperation::NextWordEnd:
		gotoNextWordEnd(*this, n, cdy);
		m_offset = m_position - m_blockData.position();
		break;
	case ViMoveOperation::NextSSWord:
		gotoNextSSWord(*this, n, cdy);
		m_offset = m_position - m_blockData.position();
		break;
	case ViMoveOperation::PrevSSWord:
		gotoPrevSSWord(*this, n);
		m_offset = m_position - m_blockData.position();
		break;
	case ViMoveOperation::NextSSWordEnd:
		gotoNextSSWordEnd(*this, n, cdy);
		m_offset = m_position - m_blockData.position();
		break;
	case StartOfBlock:
		m_position = m_blockData.position();
		m_offset = 0;
		break;
	case ViMoveOperation::LastChar: {		//	ブロックの最後の文字に移動
		index_t ix = b.EOLOffset();
		if( ix != 0 ) {
			do { } while( !isUTF8FirstChar((*m_document)[blockPos + --ix]) );
		}
		m_position = blockPos + ix;
		m_offset = 0xffffffff;
		break;
	}
	case ViMoveOperation::RightForA: {			//	右移動 for a コマンド
		if( blockText.isEmpty() ) return false;		//	改行 or EOF オンリー行の場合
		const int endpos = blockPos + b.EOLOffset();
		if( position() >= endpos ) return false;
		n = qMin(n, endpos - position());
		return movePosition(Right, mode, n);
	}
	case ViMoveOperation::JumpLine: {
		DocBlock d = document()->findBlockByNumber(n - 1);
		blockPos = d.position();
		blockText = d.text();
		//m_position = d.position();
		//m_offset = 0;
		//m_x = 0;
		//m_blockData = d.data();
		//m_viewBlockData = m_view->findBlockData(m_position);
		//break;
	}
	//	下にスルー
	case ViMoveOperation::FirstNonBlankChar:
		setPosition(blockPos + firstNonBlankCharPos(blockText));
		break;
	case EndOfBlock:		//	改行位置に移動
		m_position = b.position() + b.EOLOffset();
#if 0
		m_position = m_blockData.position() + m_document->blockSize(m_blockData.index());
		if( m_position > m_blockData.position() ) {
			uchar uch = (*m_document)[m_position - 1];
			if( uch == '\r' )
				--m_position;
			else if( uch == '\n' ) {
				--m_position;
				if( m_position > m_blockData.position() && (*m_document)[m_position - 1] == '\r' )
					--m_position;
			}
		}
#endif
		m_offset = 0xffffffff;
		break;
	case ViMoveOperation::NextLine:
		movePosition(DocCursor::NextBlock, mode, n);
		moveToFirstNonBlankChar(*this, mode);
		break;
	case ViMoveOperation::PrevLine:
		movePosition(DocCursor::PrevBlock, mode, n);
		moveToFirstNonBlankChar(*this, mode);
		break;
	case StartOfDocument:
		m_position = 0;
#if TEXT_CURSOR_BLOCK
		m_blockData = BlockData(0, 0);
		m_offset = 0;
#endif
		break;
	case EndOfDocument:
		m_position = m_document->size();
#if TEXT_CURSOR_BLOCK
		m_blockData = m_document->prevBlockData(BlockData(m_document->blockCount(), m_document->size()));
		m_offset = m_position - m_blockData.position();
#endif
		break;
	default:
		return false;
	}
	if( mode == MoveAnchor ) {
		m_anchor = m_position;
		m_anchorBlockData = m_blockData;
		//m_ancBlockIndex = m_blockData.m_index;
		//m_ancBlockPosition = m_blockData.m_position;
	}
	return true;
}
bool DocCursor::atBlockEnd() const
{
	if( position() >= document()->size() )		//	EOF 位置
		return true;
	uchar uch = document()->at(position());
	return uch == '\r' || uch == '\n';
}
bool DocCursor::atEnd() const
{
	return isNull() || m_position >= m_document->size();
}
bool DocCursor::isOverlapped(const DocCursor &x) const
{
	if( !hasSelection() ) {
		if( !x.hasSelection() )
			return position() == x.position();
		if( x.anchor() < x.position() )
			return x.anchor() <= position() && position() < x.position();
		else
			return x.position() <= position() && position() < x.anchor();
	} else {
		const index_t first = qMin(anchor(), position());
		const index_t last = qMax(anchor(), position());
		if( !x.hasSelection() )
			return first <= x.position() && x.position() < last;
		const index_t xfirst = qMin(x.anchor(), x.position());
		const index_t xlast = qMax(x.anchor(), x.position());
		return (last > xfirst && xlast > first);
	}
}
size_t DocCursor::insertText(const QString &text)
{
	if( isNull() ) return 0;
	return m_document->insertText(*this, text);
#if 0
	{
		DocBlock bk = block();
		size_t len = bk.length();
	}
	if( hasSelection() ) { 		//	done B 選択状態の場合
		if( m_anchor < m_position ) {
			m_document->do_replace(m_anchor, m_position, text);
			m_position = m_anchor;
			m_blockIndex = m_ancBlockIndex;
			m_blockPosition = m_ancBlockPosition;
			//updateBlockData();		//	undone P 速度最適化
		} else
			m_document->do_replace(m_position, m_anchor, text);
	} else
		m_document->do_insert(m_position, text);
	{
		DocBlock bk = block();
		size_t len = bk.length();
	}
	movePosition(Right, MoveAnchor, text.length());
#endif
}
void DocCursor::deleteChar()
{
	if( isNull() ) return;
	m_document->deleteChar(*this);
}
void DocCursor::deletePreviousChar()
{
	if( isNull() ) return;
	m_document->deletePreviousChar(*this);
}

//----------------------------------------------------------------------
ViewCursor::ViewCursor(TextView *view, index_t position)
	: m_view(view), DocCursor(view ? view->document() : 0, position)
	, m_x(0)
{
	DocCursor::updateBlockData();
	updateBlockData();
}
ViewCursor::ViewCursor(TextView *view, index_t position, index_t anchor)
	: m_view(view), DocCursor(view ? view->document() : 0, position, anchor)
	, m_x(0)
{
	DocCursor::updateBlockData();
	updateBlockData();
}
ViewCursor::ViewCursor(TextView *view, index_t position, index_t anchor,
			BlockData blockData)
	: m_view(view), DocCursor(view ? view->document() : 0, position, anchor, blockData)
	, m_x(0)
{
	DocCursor::updateBlockData();
	updateBlockData();
}
ViewCursor::ViewCursor(const ViewCursor &x)
	: m_view(x.m_view), DocCursor(x)
	, m_x(x.m_x)
	, m_viewBlockData(x.m_viewBlockData)
	, m_viewAnchorBlockData(x.m_viewAnchorBlockData)
{
}
ViewCursor::ViewCursor(TextView *view, const DocCursor &x)
	: m_view(view), DocCursor(x)
	, m_x(0)
{
	updateBlockData();
}
void ViewCursor::setPosition(index_t position, uchar mode)
{
	if( isNull() ) return;
	m_position = position;
	DocCursor::updateBlockData(KeepAnchor);
	updateBlockData(KeepAnchor);
	if( mode == MoveAnchor ) {
		m_anchor = m_position;
		m_anchorBlockData = m_blockData;
		m_viewAnchorBlockData = m_viewBlockData;
	}
}
void ViewCursor::updateBlockData(uchar mode)
{
	if( !m_view ) {
		m_viewBlockData = m_viewAnchorBlockData = BlockData(0, 0);
	} else {
		m_viewBlockData = m_view->findBlockData(m_position);
		if( mode == MoveAnchor || position() == anchor() ) {
			m_anchorBlockData = m_blockData;
			m_viewAnchorBlockData = m_viewBlockData;
		} else {
			m_viewAnchorBlockData = m_view->findBlockData(m_anchor);
		}
	}
}
void ViewCursor::insertText(const QString &text)
{
	if( isNull() ) return;
	m_view->insertText(*this, text);
}
void ViewCursor::deleteChar()
{
	if( isNull() ) return;
	m_view->deleteChar(*this);
}
void ViewCursor::deletePreviousChar()
{
	if( isNull() ) return;
	m_view->deletePreviousChar(*this);
}
int ViewCursor::prevCharsCount() const
{
	if( isNull() ) return 0;
	int cnt = 0;
	index_t pos = m_viewBlockData.position();
	Q_ASSERT( pos <= m_position );
	while( pos < m_position ) {
		++cnt;
		pos += UTF8CharSize((*m_document)[pos]);
	}
	return cnt;
}
void ViewCursor::copyPositionToAnchor()
{
	m_anchor = m_position;
	m_anchorBlockData = m_blockData;
	m_viewAnchorBlockData = m_viewBlockData;
}
void ViewCursor::copyAnchorToPosition()
{
	m_position = m_anchor;
	m_blockData = m_anchorBlockData;
	m_viewBlockData = m_viewAnchorBlockData;
}
void ViewCursor::swapPositionAnchor()
{
	DocCursor::swapPositionAnchor();
	BlockData b;
	b = m_viewBlockData; m_viewBlockData = m_viewAnchorBlockData; m_viewAnchorBlockData = b;
}
int firstNonBlankCharPos(const QString &text);
bool ViewCursor::movePosition(uchar move, uchar mode, uint n, bool cdy)
{
	if( isNull() || !n ) return false;
	ViewBlock vb = block();
	QString blockText = vb.text();
	index_t blockPos = vb.position();
	index_t pos = position();
	switch( move ) {
	//case ViMoveOperation::Up:	//	暫定コード
	case Up:
		while( n != 0 ) {
			if( !m_viewBlockData.m_index ) break;
			m_viewBlockData.m_position -= view()->blockSize(--m_viewBlockData.m_index);
			--n;
		}
		m_position = m_viewBlockData.m_position;
		m_position = view()->movePositionByCharCount(m_position,
									view()->xToCharCount(block().text(), m_x));
		//movePosition(Right, KeepAnchor, view()->xToCharCount(block().text(), m_x));
		m_blockData = view()->docBlockData(m_viewBlockData);
		break;
	//case ViMoveOperation::Down:	//	暫定コード
	case Down:
		while( n != 0 ) {
			if( m_viewBlockData.m_index >= view()->blockCount() - 1 ) break;
			m_viewBlockData.m_position += view()->blockSize(m_viewBlockData.m_index++);
			--n;
		}
		m_position = m_viewBlockData.m_position;
		m_position = view()->movePositionByCharCount(m_position,
									view()->xToCharCount(block().text(), m_x));
		//movePosition(Right, KeepAnchor, view()->xToCharCount(block().text(), m_x));
		m_blockData = view()->docBlockData(m_viewBlockData);
		break;
	case ViMoveOperation::Left:
		n = qMin(n, pos - vb.position());
		move = DocCursor::Left;
		goto moveDocCursor;
	case ViMoveOperation::Right: {
		if( !vb.EOLOffset() ) return false;		//	改行 or EOF オンリー行の場合
		const int endpos = vb.position() + vb.EOLOffset() - 1;
		if( pos >= endpos ) return false;
		n = qMin(n, endpos - pos);
		move = DocCursor::Right;
		goto moveDocCursor;
	}
#if 0
	case ViMoveOperation::PrevWord:
		gotoPrevWord(*this);
		break;
#endif
#if 0
	case StartOfBlock:
		m_position = m_viewBlockData.position();
		m_offset = 0;
		m_x = 0;
		break;
	case EndOfBlock:
		m_position = m_viewBlockData.position() + block().EOLOffset();
		m_offset = 0xffffffff;
		m_x = -1;
		break;
#endif
moveDocCursor:
	default:
		if( !DocCursor::movePosition(move, mode, n, cdy) )
			return false;
		//if( !m_view->isLayoutedDocBlock(blockIndex()) )
		//	m_viewBlockData = blockData();
		//else
			m_viewBlockData = view()->findBlockData(position());
		if( move == EndOfBlock )
			m_x = -1;
		else
			m_x = view()->charCountToX(block().text(), prevCharsCount());
		break;
	}
	if( mode == MoveAnchor )
		copyPositionToAnchor();
	return true;
}
ViewCursor &ViewCursor::operator=(const DocCursor &x)
{
	if( isNull() ) return *this;
	m_position = x.position();
	m_blockData = x.blockData();
	m_anchor = x.anchor();
	m_anchorBlockData = x.anchorBlockData();
	m_viewBlockData = m_view->findBlockData(m_position);
	if( m_position != m_anchor )
		m_viewAnchorBlockData = m_view->findBlockData(m_anchor);
	else
		m_viewAnchorBlockData = m_viewBlockData;
	return *this;
}
ViewBlock ViewCursor::block() const
{
	return ViewBlock(m_view, DocCursor::block(),
						m_viewBlockData);		//	
}
DocBlock ViewCursor::docAnchorBlock() const
{
	return DocBlock(m_view->document(), anchorBlockData());
}
void ViewCursor::updateViewBlock()
{
	if( view()->lineBreakMode() ) {
		m_viewBlockData = m_view->findBlockData(position());
		if( hasSelection() )
			m_viewAnchorBlockData = m_view->findBlockData(anchor());
		else
			m_viewAnchorBlockData = m_viewBlockData;
	} else {
		setViewBlockData(blockData());
		setViewAnchorBlockData(anchorBlockData());
	}
}
void ViewCursor::move(int d)
{
	DocCursor::move(d);
	updateBlockData(KeepAnchor);
}
