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
size_t getEOLOffset(const TextDocument *doc, TextBlockData d)
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
void TextCursor::updateBlockData(uchar mode)
{
	if( !m_document ) {
		m_blockData = TextBlockData(0, 0);
	} else {
		m_blockData = m_document->findBlockData(m_position);
		if( mode == MoveAnchor )
			m_anchorBlockData = m_blockData;
	}
}

QString TextCursor::selectedText() const
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
int TextCursor::prevCharsCount() const
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
TextBlock TextCursor::block() const
{
	return TextBlock(m_document, m_blockData);
}

void TextCursor::copyPositionToAnchor()
{
	m_anchor = m_position;
	m_anchorBlockData = m_blockData;
	//m_ancBlockIndex = m_blockIndex;
	//m_ancBlockPosition = m_blockPosition;
}
void TextCursor::copyAnchorToPosition()
{
	m_position = m_anchor;
	m_blockData = m_anchorBlockData;
	//m_blockIndex = m_ancBlockIndex;
	//m_blockPosition = m_ancBlockPosition;
}

void TextCursor::swapPositionAnchor()
{
	index_t t;
	t = m_position; m_position = m_anchor; m_anchor = t;
	TextBlockData b;
	b = m_blockData; m_blockData = m_anchorBlockData; m_anchorBlockData = b;
	//t = m_blockIndex; m_blockIndex = m_ancBlockIndex; m_ancBlockIndex = t;
	//t = m_blockPosition; m_blockPosition = m_ancBlockPosition; m_ancBlockPosition = t;
}

void TextCursor::setPosition(index_t position, uchar mode)
{
	if( isNull() ) return;
	m_position = position;
	updateBlockData(KeepAnchor);
	m_offset = m_position - m_blockData.position();
	if( mode == MoveAnchor ) {
		m_anchor = m_position;
		m_anchorBlockData = m_blockData;
	}
}
void TextCursor::setPosition(index_t position, TextBlockData d, uchar mode)
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
bool gotoNextWord(TextCursor &cur, int n = 1, bool cdy = false);
bool gotoPrevWord(TextCursor &cur, int n = 1);
bool gotoStartOfWord(TextCursor &cur);
bool gotoEndOfWord(TextCursor &cur);
inline bool isTabOrSpace(const QChar ch)
{
	return ch == '\t' || ch == ' ';
}

bool gotoNextWord(TextCursor &cur, int n, bool cdy)
{
	const TextDocument *doc = cur.document();
	TextBlock block = cur.block();
	//int blockPos = block.position();
	QString text = block.text();
	int pos = cur.position();
	int ix = cur.prevCharsCount();
	while( --n >= 0 ) {
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
		//	空白類を読み飛ばす
		while( ix == text.length() || isTabOrSpace(text[ix]) ) {
			if( ix == text.length() ) {
				if( cdy && !n )	//	cdy が前置されている場合は、最後の改行はスキップしない
					break;
				TextBlock nb = block.next();
				if( !nb.isValid() ) {
					cur.setPosition(pos, block.data(), TextCursor::KeepAnchor);
					return true;
				}
				block = nb;
				//blockPos = block.position();
				text = block.text();
				ix = 0;
			} else {
				++ix;
				pos += UTF8CharSize((*doc)[pos]);
			}
		}
	}
	cur.setPosition(pos, block.data(), TextCursor::KeepAnchor);
	return true;
}
bool gotoPrevWord(TextCursor &cur, int n)
{
	const TextDocument *doc = cur.document();
	TextBlock block = cur.block();
	//int blockPos = block.position();
	QString text = block.text();
	int pos = cur.position();
	int ix = cur.prevCharsCount();
	while( --n >= 0 ) {
		//	ひとつ前の文字が空白類 or 行頭なら文書先頭方向に移動
		while( !ix || isTabOrSpace(text[ix-1]) ) {
			if( !ix ) {
				TextBlock pb = block.prev();
				if( !pb.isValid() ) {
					cur.setPosition(pos, block.data(), TextCursor::KeepAnchor);
					return true;
				}
				block = pb;
				//blockPos = block.position();
				text = block.text();
				ix = text.length();
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
	cur.setPosition(pos, block.data(), TextCursor::KeepAnchor);
	return true;
}
bool gotoStartOfWord(TextCursor &cur)
{
	const TextDocument *doc = cur.document();
	TextBlock block = cur.block();
	QString text = block.text();
	int pos = cur.position();
	int ix = cur.prevCharsCount();
	//	ひとつ前の文字が同じタイプ and 空白類になるまで or 行頭まで読み飛ばす
	if( ix > 0 && !isTabOrSpace(text[ix-1]) ) {
		uchar cat = getCharType(text[--ix]);
		do { } while( !isUTF8FirstChar((*doc)[--pos]) );
		while( ix > 0 && !isTabOrSpace(text[ix-1]) && getCharType(text[ix-1]) == cat ) {
			--ix;
			do { } while( !isUTF8FirstChar((*doc)[--pos]) );
		}
	}
	cur.setPosition(pos, block.data(), TextCursor::KeepAnchor);
	return true;
}
bool gotoEndOfWord(TextCursor &cur)
{
	const TextDocument *doc = cur.document();
	TextBlock block = cur.block();
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
	cur.setPosition(pos, block.data(), TextCursor::KeepAnchor);
	return true;
}
bool TextCursor::movePosition(uchar move, uchar mode, uint n)
{
	if( isNull() ) return false;
	switch( move ) {
	case Right:
		if( m_document->size() - m_position <= n ) {
			//	1文字==1byte とは限らないのでこの処理だけでは不十分
			m_position = m_document->size();
			m_blockData = m_document->prevBlockData(TextBlockData(m_document->blockCount(), m_position));
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
		m_offset = m_position - m_blockData.position();
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
	case Up:
		while( n != 0 ) {
			if( !m_blockData.m_index ) break;
			//	undone B 暫定コード
			m_position = m_blockData.m_position -= m_document->blockSize(--m_blockData.m_index);
			--n;
		}
		m_position = m_blockData.m_position + qMin(m_offset, getEOLOffset(m_document, m_blockData));
		break;
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
	case NextWord:
		gotoNextWord(*this);
		m_offset = m_position - m_blockData.position();
		break;
	case PrevWord:
		gotoPrevWord(*this);
		m_offset = m_position - m_blockData.position();
		break;
	case StartOfBlock:
		m_position = m_blockData.position();
		m_offset = 0;
		break;
	case EndOfBlock:		//	改行位置に移動
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
		m_offset = 0xffffffff;
		break;
	case StartOfDocument:
		m_position = m_offset = 0;
		m_blockData = TextBlockData(0, 0);
		m_offset = 0;
		break;
	case EndOfDocument:
		m_position = m_document->size();
		m_blockData = m_document->prevBlockData(TextBlockData(m_document->blockCount(), m_document->size()));
		m_offset = m_position - m_blockData.position();
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
bool TextCursor::atEnd() const
{
	return isNull() || m_position >= m_document->size();
}
void TextCursor::insertText(const QString &text)
{
	if( isNull() ) return;
	m_document->insertText(*this, text);
#if 0
	{
		TextBlock bk = block();
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
		TextBlock bk = block();
		size_t len = bk.length();
	}
	movePosition(Right, MoveAnchor, text.length());
#endif
}
void TextCursor::deleteChar()
{
	if( isNull() ) return;
	m_document->deleteChar(*this);
}
void TextCursor::deletePreviousChar()
{
	if( isNull() ) return;
	m_document->deletePreviousChar(*this);
}

//----------------------------------------------------------------------
ViewTextCursor::ViewTextCursor(TextView *view, index_t position)
	: m_view(view), TextCursor(view ? view->document() : 0, position)
{
	TextCursor::updateBlockData();
	updateBlockData();
}
ViewTextCursor::ViewTextCursor(TextView *view, index_t position, index_t anchor)
	: m_view(view), TextCursor(view ? view->document() : 0, position, anchor)
{
	TextCursor::updateBlockData();
	updateBlockData();
}
ViewTextCursor::ViewTextCursor(TextView *view, index_t position, index_t anchor,
			TextBlockData blockData)
	: m_view(view), TextCursor(view ? view->document() : 0, position, anchor, blockData)
{
	TextCursor::updateBlockData();
	updateBlockData();
}
ViewTextCursor::ViewTextCursor(const ViewTextCursor &x)
	: m_view(x.m_view), TextCursor(x)
{
}
void ViewTextCursor::setPosition(index_t position, uchar mode)
{
	if( isNull() ) return;
	m_position = position;
	TextCursor::updateBlockData(KeepAnchor);
	updateBlockData(KeepAnchor);
	if( mode == MoveAnchor ) {
		m_anchor = m_position;
		m_anchorBlockData = m_blockData;
		m_viewAnchorBlockData = m_viewBlockData;
	}
}
void ViewTextCursor::updateBlockData(uchar mode)
{
	if( !m_view ) {
		m_viewBlockData = m_viewAnchorBlockData = TextBlockData(0, 0);
	} else {
		m_viewBlockData = m_view->findBlockData(m_position);
		if( mode == MoveAnchor ) {
			m_anchorBlockData = m_blockData;
			m_viewAnchorBlockData = m_viewBlockData;
		}
	}
}
void ViewTextCursor::insertText(const QString &text)
{
	if( isNull() ) return;
	m_view->insertText(*this, text);
}
void ViewTextCursor::deleteChar()
{
	if( isNull() ) return;
	m_view->deleteChar(*this);
}
void ViewTextCursor::deletePreviousChar()
{
	if( isNull() ) return;
	m_view->deletePreviousChar(*this);
}
bool ViewTextCursor::movePosition(uchar move, uchar mode, uint n)
{
	if( !TextCursor::movePosition(move, mode, n) ) return false;
	return true;
}
ViewTextCursor &ViewTextCursor::operator=(const TextCursor &x)
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
