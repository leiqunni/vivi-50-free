//----------------------------------------------------------------------
//
//			File:			"TextDocument.cpp"
//			Created:		01-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	TextDocument クラス実装
//
//----------------------------------------------------------------------

/*

	Copyright (C) 2011 by Nobuhide Tsuda

	本ソースコードは基本的に MIT ライセンスに従う。
	http://www.opensource.org/licenses/mit-license.php
	http://sourceforge.jp/projects/opensource/wiki/licenses%2FMIT_license

	ただし、おいらは不自由で使い勝手の悪い GPL が大嫌いなので、
	GPL ライセンスプロジェクトが本ソースを流用することを禁じる

*/

#include	<QtCore>
#include "TextDocument.h"

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
//----------------------------------------------------------------------
#if	BLOCK_HAS_SIZE
void TextCursor::updateBlockData(uchar mode)
{
	if( !m_document ) {
		m_blockData = TextBlockData(0, 0);
		//m_blockIndex = m_blockPosition = m_ancBlockIndex = m_ancBlockPosition = 0;
	} else {
		//m_blockData.m_index = m_document->findBlockIndex(m_position, &m_blockData.m_position);
		m_blockData = m_document->findBlockData(m_position);
		if( mode == MoveAnchor ) {
			m_anchorBlockData = m_blockData;
			//m_ancBlockIndex = m_blockIndex;
			//m_ancBlockPosition = m_blockPosition;
		}
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
TextBlock TextCursor::block()
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
	if( mode == MoveAnchor ) {
		m_anchor = m_position;
		m_anchorBlockData = m_blockData;
		//m_ancBlockIndex = m_blockIndex;
		//m_ancBlockPosition = m_blockPosition;
	}
}
void TextCursor::setPosition(index_t position, TextBlockData d, uchar mode)
{
	if( isNull() ) return;
	m_position = position;
	m_blockData = d;
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
		break;
	case Up:
		while( n != 0 ) {
			if( !m_blockData.m_index ) break;
			//	undone B 暫定コード
			m_position = m_blockData.m_position -= m_document->blockSize(--m_blockData.m_index);
			--n;
		}
		break;
	case Down:
		while( n != 0 ) {
			if( m_blockData.m_index >= m_document->blockCount() - 1 ) break;
			//	undone B 暫定コード
			m_position = m_blockData.m_position += m_document->blockSize(m_blockData.m_index++);
			--n;
		}
		break;
	case StartOfWord:
		gotoStartOfWord(*this);
		break;
	case EndOfWord:
		gotoEndOfWord(*this);
		break;
	case NextWord:
		gotoNextWord(*this);
		break;
	case PrevWord:
		gotoPrevWord(*this);
		break;
	case StartOfBlock:
		m_position = m_blockData.position();
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
		break;
	case StartOfDocument:
		m_position = 0;
		m_blockData = TextBlockData(0, 0);
		break;
	case EndOfDocument:
		m_position = m_document->size();
		m_blockData = m_document->prevBlockData(TextBlockData(m_document->blockCount(), m_document->size()));
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
#endif
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
#if 1
void GVUndoMgr::push_back(const GVUndoItem &item, bool modified)
{
	if( m_current < m_items.size() ) {
		//	undone M 不要なヒープの解放
		m_items.erase(m_items.begin() + m_current, m_items.end());
	}
	m_items.push_back(item);
	if( !modified )
		m_items[m_items.size() - 1].m_flags |= GVUNDOITEM_UNDO_MF_OFF;
	m_current = m_items.size();
}
#else
void GVUndoMgr::push_back(GVUndoItem *ptr, bool modified)
{
	if( m_current < m_items.size() ) {
		//	undone M 不要なヒープの解放
		m_items.erase(m_items.begin() + m_current, m_items.end());
	}
	if( !modified )
		ptr->m_flags |= GVUNDOITEM_UNDO_MF_OFF;
//#if		!UNDOMGR_USE_OBJECT_POOL
//	m_items.push_back(boost::shared_ptr<GVUndoItem>(ptr));
//#else
	m_items.push_back(ptr);
//#endif
	m_current = m_items.size();
}
#endif

//	ドキュメントが保存された時にコールされる
void GVUndoMgr::resetModifiedFlags()
{
	bool before = true;
	for(int ix = 0; ix < m_items.size(); ++ix) {
		if( ix == m_current ) before = false;
		GVUndoItem *undoItem = &m_items[ix];
		undoItem->m_beforeSave = before;
		undoItem->m_flags &= ~(GVUNDOITEM_UNDO_MF_OFF|GVUNDOITEM_REDO_MF_OFF);
		if( ix == m_current - 1 )
			undoItem->m_flags |= GVUNDOITEM_REDO_MF_OFF | GVUNDOITEM_SAVED;
		else if( ix == m_current)
			undoItem->m_flags |= GVUNDOITEM_UNDO_MF_OFF;
	}
}
//	@return		Undo の結果、モディファイフラグがＯＮの場合は TRUE を返す
bool GVUndoMgr::doUndo(TextDocument *bb, uint& pos)
{
	if( !m_current ) return false;
	//boost::shared_ptr<GVUndoItem> undoItem = m_items[--m_current];
#if 1
	GVUndoItem *ptr = &m_items[--m_current];
	switch( ptr->m_type ) {
	case GVUNDOITEM_TYPE_ERASE: {
		cuchar *heap = &m_heap[ptr->m_hp_ix];
		bb->insert(ptr->m_first, heap, heap + ptr->data_size());
		pos = ptr->m_first + ptr->data_size();
		break;
	}
	case GVUNDOITEM_TYPE_INSERT:
		if( ptr->m_rhp_ix == 0 )
			ptr->m_rhp_ix = addToRedoHeap(bb->begin() + ptr->m_first, bb->begin() + ptr->m_last);
		bb->erase(ptr->m_first, ptr->m_last);
		pos = ptr->m_first;
		break;
	case GVUNDOITEM_TYPE_REPLACE: {
		if( ptr->m_rhp_ix == 0 )
			ptr->m_rhp_ix = addToRedoHeap(bb->begin() + ptr->m_first, bb->begin() + ptr->m_last2);
		bb->erase(ptr->m_first, ptr->m_last2);
		cuchar *heap = &m_heap[ptr->m_hp_ix];
		bb->insert(ptr->m_first, heap, heap + ptr->data_size());
		pos = ptr->m_first + ptr->data_size();
		break;
	}
	}
	return !(ptr->m_flags & GVUNDOITEM_UNDO_MF_OFF) ? true : false;
#else
	m_items[--m_current]->doUndo(bb, pos);
	return !(m_items[m_current]->m_flags & GVUNDOITEM_UNDO_MF_OFF) ? true : false;
#endif
	//return true;
}
bool GVUndoMgr::doRedo(TextDocument *bb, uint& pos)
{
	if( m_current >= m_items.size() ) return false;
	//boost::shared_ptr<GVUndoItem> undoItem = m_items[m_current];
	//++m_current;
#if 1
	const GVUndoItem *ptr = &m_items[m_current++];
	switch( ptr->m_type ) {
	case GVUNDOITEM_TYPE_ERASE:
		bb->erase(ptr->m_first, ptr->m_last);
		pos = ptr->m_first;
		break;
	case GVUNDOITEM_TYPE_INSERT: {
		cuchar *heap = &m_redoHeap[ptr->m_rhp_ix];
		bb->insert(ptr->m_first, heap, heap + ptr->data_size());
		pos = ptr->m_first + ptr->data_size();
		break;
	}
	case GVUNDOITEM_TYPE_REPLACE: {
		bb->erase(ptr->m_first, ptr->m_last);
		cuchar *heap = &m_redoHeap[ptr->m_rhp_ix];
		bb->insert(ptr->m_first, heap, heap + ptr->data_size2());
		pos = ptr->m_first + ptr->data_size();
		break;
	}
	}
	return !(ptr->m_flags & GVUNDOITEM_UNDO_MF_OFF) ? true : false;
#else
	m_items[m_current]->doRedo(bb, pos);
	return !(m_items[m_current++]->m_flags & GVUNDOITEM_REDO_MF_OFF) ? true : false;;
#endif
}
//----------------------------------------------------------------------
uint TextBlock::size() const
{
#if 1
	return m_document->blockSize(m_data.m_index);
#else
	if( !isValid() ) return 0;
	if( m_index == m_document->blockCount() - 1 )		//	最後のブロック
		return m_document->size() - m_document->blockPosition(m_index);
	else
		return m_document->blockPosition(m_index + 1) - m_document->blockPosition(m_index);
#endif
}
index_t TextBlock::position() const
{
#if BLOCK_HAS_SIZE
	return isValid() ? m_data.m_position : 0;
#else
	return isValid() ? m_document->blockPosition(m_blockNumber) : 0;
#endif
}

int TextBlock::charsCount(index_t position) const
{
	if( !isValid() || position <= m_data.position() ||
		position > m_data.position() + m_document->blockSize(m_data.index()) )
	{
		return 0;
	}
	int cnt = 0;
	int ix = m_data.position();
	while( ix < position ) {
		++cnt;
		ix += UTF8CharSize((*m_document)[ix]);
	}
	return cnt;
}
	
QString TextBlock::text() const
{
	if( !isValid() ) return QString();
	/*const*/ size_t sz = size();
	if( !sz ) return QString();
	sz = qMin(sz, m_document->size() - position());
	QByteArray ba;
	ba.reserve(sz);
	Q_ASSERT( position() + sz <= m_document->size() );
	for(index_t ix = position(), iend = position() + sz; ix != iend; ++ix)
		ba += (*m_document)[ix];
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	return codec->toUnicode(ba);
}
TextBlock TextBlock::next() const
{
	if( !isValid() ) return *this;
#if 1
	TextBlockData d = m_document->nextBlockData(m_data);
	if( d.index() >= m_document->blockCount() )
		d.m_index = INVALID_INDEX;
	return TextBlock(m_document, d);
#else
	index_t blockPosition = m_data.m_position + m_document->blockSize(m_data.m_index);
	int ix = m_data.m_index + 1;
	if( ix >= m_document->blockCount() )
		ix = INVALID_INDEX;
	return TextBlock(m_document, ix, blockPosition);
#endif
}
TextBlock TextBlock::prev() const
{
	if( !isValid() ) return *this;
	TextBlockData d = m_document->prevBlockData(m_data);
	return TextBlock(m_document, d);
}
//----------------------------------------------------------------------
TextDocument::TextDocument(QObject *parent)
	: QObject(parent)
{
	init();
}

TextDocument::~TextDocument()
{

}

void TextDocument::init()
{
	m_modified = false;
	m_buffer.clear();
	m_blocks.clear();
	m_blocks.push_back(TextBlockItem(0));
	m_blockData = TextBlockData(0, 0);
	//m_blockIndex = m_blockPosition = 0;
	emit blockCountChanged();
}
size_t TextDocument::blockSize(index_t ix) const
{
	if( ix >= m_blocks.size() ) return 0;
#if BLOCK_HAS_SIZE
	return m_blocks[ix].m_size;
#else
	if( ix == blockCount() - 1 )		//	最後のブロック
		return size() - blockPosition(ix);
	else
		return blockPosition(ix + 1) - blockPosition(ix);
#endif
}
QString TextDocument::toPlainText() const
{
	QByteArray ba;
	ba.reserve(size());
	for(index_t ix = 0; ix != size(); ++ix)
		ba += m_buffer[ix];
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	return codec->toUnicode(ba);
}
TextBlockData TextDocument::findBlockData(index_t position) const
{
	if( m_blocks.size() == 1 )
		return TextBlockData(0, 0);
	TextBlockData data(0, 0), next;
	if( m_blockData.m_index == 0 ) {		//	キャッシュが無い場合
		if( position <= size() / 2 ) {
			while( data.m_index < m_blocks.size() - 1 &&
					position >= (next = nextBlockData(data)).position() )
				data = next;
		} else {
			data = TextBlockData(m_blocks.size(), size());
			do {
				data = prevBlockData(data);
			} while( data.position() > position );
		}
	} else {
		if( position < m_blockData.position() ) {
			if( position <= m_blockData.position() / 2 ) {
				while( data.m_index < m_blockData.index() - 1 &&
						position >= (next = nextBlockData(data)).position() )
					data = next;
			} else {
				data = m_blockData;
				do {
					data = prevBlockData(data);
				} while( data.position() > position );
			}
		} else {
			next = nextBlockData(m_blockData);
			if( m_blockData.position() <= position && position < next.position() )
				return m_blockData;
			if( position <= m_blockData.position() + (size() - m_blockData.position()) / 2 ) {
				while( data.m_index < m_blocks.size() - 1 &&
						position >= (next = nextBlockData(data)).position() )
					data = next;
			} else {
				data = TextBlockData(m_blocks.size(), size());
				do {
					data = prevBlockData(data);
				} while( data.position() > position );
			}
		}
	}
	return data;
}
index_t	TextDocument::findBlockIndex(index_t position, index_t *pBlockPos) const
{
	TextBlockData d = findBlockData(position);
	if( pBlockPos != 0 )
		*pBlockPos = d.position();
	return d.index();
}

#if		BLOCK_HAS_SIZE
size_t TextDocument::blockPosition(index_t ix) const
{
	size_t pos = 0;
	for(int i = 0; i < ix; ++i)
		pos += m_blocks[i].m_size;
	return pos;
}
#endif

TextBlock TextDocument::findBlock(index_t position) const
{
	if( position > size() ) return TextBlock(const_cast<TextDocument*>(this), INVALID_INDEX, 0);
	index_t blockPosition;
	//const index_t ix = findBlockIndex(position, &blockPosition);
	TextBlockData d = findBlockData(position);
	return TextBlock(const_cast<TextDocument*>(this), d);
}
TextBlock TextDocument::findBlockByNumberRaw(index_t blockIndex) const
{
	index_t blockPosition = 0;
	index_t ix = 0;
	if( blockIndex <= blockCount() / 2 ) {
		while( ix < blockIndex )
			blockPosition += m_blocks[ix++].m_size;
	} else {	//	中央より後ろの場合
		blockPosition = size();
		index_t ix = blockCount();
		while( ix > blockIndex )
			blockPosition -= m_blocks[--ix].m_size;
	}
	return TextBlock(const_cast<TextDocument*>(this), ix, blockPosition);
}
TextBlock TextDocument::findBlockByNumber(index_t blockIndex) const
{
	if( blockIndex >= blockCount() - 1 ) {
		m_blockData.m_index = blockCount() - 1;
		m_blockData.m_position = size() - blockSize(m_blockData.m_index);
		return TextBlock(const_cast<TextDocument*>(this), m_blockData);
	}
	index_t blockPosition = 0;
	index_t ix = 0;
	if( m_blockData.m_index == 0 ) {		//	キャッシュが無い場合
		if( blockIndex <= blockCount() / 2 ) {
			while( ix < blockIndex )
				blockPosition += m_blocks[ix++].m_size;
		} else {	//	中央より後ろの場合
			blockPosition = size();
			ix = blockCount();
			while( ix > blockIndex )
				blockPosition -= m_blocks[--ix].m_size;
		}
	} else {
		if( blockIndex == m_blockData.m_index )
			return TextBlock(const_cast<TextDocument*>(this), m_blockData.m_index, m_blockData.m_position);
		if( blockIndex < m_blockData.m_index ) {
#if 0	//	逆方向シーケンシャルアクセス頻度は低いのでコメントアウトしておく
			if( blockIndex == m_blockIndex - 1 ) {
				m_blockPosition -= m_blocks[--m_blockIndex].m_size;
				return TextBlock(this, m_blockIndex, m_blockPosition);
			}
#endif
			if( blockIndex <= m_blockData.m_index / 2 ) {
				while( ix < blockIndex )
					blockPosition += m_blocks[ix++].m_size;
			} else {	//	中央より後ろの場合
				blockPosition = m_blockData.m_position;
				ix = m_blockData.m_index;
				while( ix > blockIndex )
					blockPosition -= m_blocks[--ix].m_size;
			}
		} else {	//	m_blockData.m_index < blockIndex < m_blocks.size() の場合
			if( blockIndex == m_blockData.m_index + 1 ) {
				m_blockData.m_position += m_blocks[m_blockData.m_index++].m_size;
				return TextBlock(const_cast<TextDocument*>(this), m_blockData);
			}
			if( blockIndex <= m_blockData.m_index + (m_blocks.size() - m_blockData.m_index) / 2 ) {
				blockPosition = m_blockData.m_position;
				ix = m_blockData.m_index;
				while( ix < blockIndex )
					blockPosition += m_blocks[ix++].m_size;
			} else {	//	中央より後ろの場合
				blockPosition = m_blockData.m_position;
				ix = m_blockData.m_index;
				while( ix > blockIndex )
					blockPosition -= m_blocks[--ix].m_size;
			}
		}
	}
	m_blockData.m_index = ix;
	m_blockData.m_position = blockPosition;
	return TextBlock(const_cast<TextDocument*>(this), ix, blockPosition);
}

void TextDocument::buildBlocks()
{
	m_blocks.clear();
	index_t offset = 0;
	index_t ix = 0;
	while( ix < m_buffer.size() ) {
		uchar ch = m_buffer[ix++];
		if( ch == '\n' ) {
			m_blocks.push_back(TextBlockItem(ix - offset));
			offset = ix;
		} else if( ch == '\r' ) {
			if( ix < m_buffer.size() && m_buffer[ix] == '\n' )
				++ix;
			m_blocks.push_back(TextBlockItem(ix - offset));
			offset = ix;
		}
	}
	//if( ix != offset )
		m_blocks.push_back(TextBlockItem(ix - offset));		//	EOF 行
	emit blockCountChanged();
}
void TextDocument::updateBlocksAtInsert(index_t first,
						TextBlockData d, /*index_t blockIndex, index_t blockPosition,*/
						size_t sz)
{
	bool bcChanged = false;
	index_t offset = first - d.position();		//	挿入位置から行頭までの文字バイト数
	const size_t rest = m_blocks[d.index()].m_size - offset;	//	挿入位置から次の行頭までの文字バイト数
	index_t last = first + sz;
	uchar ch = 0;
	while( first < last ) {
		ch = m_buffer[first++];
		if( ch == '\n' ) {
		} else if( ch == '\r' ) {
			const bool isNextLF = first < size() && m_buffer[first] == '\n';
			if( first == last ) {	//	最後の挿入文字が \r の場合
				if( isNextLF ) break;	//	次が \n ならば行を作成しない
			} else {
				if( isNextLF )
					++first;
			}
		} else
			continue;
		m_blocks.insert(d.index(), TextBlockItem(first - d.position()));
		bcChanged = true;
		++d.m_index;
		d.m_position = first;
	}
	if( ch != '\n' && ch != '\n' )	//	最後の挿入文字が改行で無い場合
		m_blocks[d.index()].m_size += (last - d.position()) - offset;
	else
		m_blocks[d.index()].m_size = rest;
	if( bcChanged )
		emit blockCountChanged();
}
#if 0
void TextDocument::updateBlocksAtInsert(index_t first, size_t sz)
{
	//index_t blockPos = 0;
	//index_t bix = findBlockIndex(first, &blockPos);
	TextBlockData d = findBlockData(first);
	updateBlocksAtInsert(first, bix, blockPos, sz);
}
#endif
//	erase 処理が行われる直前にコールされ、
//	m_blocks を更新
void TextDocument::updateBlocksAtErase(index_t first,
					TextBlockData d, /*index_t blockIndex, index_t blockPosition,*/
					index_t last)
{
	size_t sz = last - first;
	index_t bixLast = d.index();
	index_t offset = first - d.position();		//	削除位置から行頭までの文字バイト数
	uchar ch = 0;
	while( first < last ) {
		ch = m_buffer[first++];
		if( ch == '\n' ) {
		} else if( ch == '\r' ) {
			const bool isNextLF = first < size() && m_buffer[first] == '\n';
			if( first == last ) {	//	最後の挿入文字が \r の場合
				if( isNextLF ) break;	//	次が \n ならば行を削除しない
			} else {
				if( isNextLF )
					++first;
			}
		} else
			continue;
		d.m_position = first;
		++bixLast;
	}
	if( bixLast != d.index() ) {
		m_blocks.erase(d.index(), bixLast);
		emit blockCountChanged();
	}
	m_blocks[d.index()].m_size += offset - (last - d.position());
	//size_t t = m_blocks[blockIndex].m_size;	//	for debug
}
#if 0
void TextDocument::updateBlocksAtErase(index_t first, index_t last)
{
	index_t blockPosition = 0;
	index_t blockIndex = findBlockIndex(first, &blockPosition);
	updateBlocksAtErase(first, blockIndex, blockPosition, last);
}
#endif

void TextDocument::erase(index_t first, index_t last)
{
	if( last > size() ) last = size();
	updateBlocksAtErase(first, findBlockData(first), last);
	m_buffer.erase(first, last);
}
void TextDocument::erase(index_t first, TextBlockData d, index_t last)
{
	if( last > size() ) last = size();
	updateBlocksAtErase(first, d, last);
	m_buffer.erase(first, last);
}
void TextDocument::insert(index_t position, TextBlockData d, const QString &text)
{
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QByteArray ba = codec->fromUnicode(text);
	const int sz = ba.length();
	const uchar *ptr = (const uchar *)(ba.data());
	if( position > m_buffer.size() ) position = m_buffer.size();
	m_buffer.insert(position, ptr, ptr + sz);
	updateBlocksAtInsert(position, d, sz);
}
void TextDocument::insert(index_t position, const QString &text)
{
	//index_t blockPosition = 0;
	//index_t blockIndex = findBlockIndex(position, &blockPosition);
	insert(position, findBlockData(position), text);
}
void TextDocument::insert(index_t ix, TextBlockData d,
							cuchar *first, cuchar *last)
{
	m_buffer.insert(ix, first, last);
	updateBlocksAtInsert(ix, d, last - first);
}
void TextDocument::insert(index_t ix, cuchar *first, cuchar *last)
{
	//index_t blockPosition = 0;
	//index_t blockIndex = findBlockIndex(ix, &blockPosition);
	insert(ix, findBlockData(ix), first, last);
}

void TextDocument::setPlainText(const QString &text)
{
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QByteArray ba = codec->fromUnicode(text);
	const int sz = ba.length();
	const uchar *ptr = (const uchar *)(ba.data());
	m_buffer.clear();
	m_buffer.insert(0, ptr, ptr + sz);
	buildBlocks();
	//appendBlocks();
}
#if 0
void TextDocument::append(const QByteArray &utf8)
{
	const int sz = utf8.length();
	const uchar *ptr = (const uchar *)(utf8.data());
	m_buffer.insert(m_buffer.end(), ptr, ptr + sz);
	buildBlocks();
}
#endif
void TextDocument::deleteChar(TextCursor &cur)
{
	if( cur.isNull() || cur.document() != this )
		return;
	if( !cur.hasSelection() )
		cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor);
	else if( cur.position() < cur.anchor() )
		cur.swapPositionAnchor();
	index_t first = cur.anchor();
	index_t last = cur.position();
	if( first == last ) return;
	do_erase(first, last);
	cur.copyAnchorToPosition();
	m_blockData = cur.blockData();
	m_modified = true;
	emit contentsChange(first, last - first, 0);
	emit contentsChanged();
}
void TextDocument::deletePreviousChar(TextCursor &cur)
{
	if( cur.isNull() || cur.document() != this )
		return;
	if( cur.hasSelection() ) {
		deleteChar(cur);
		return;
	}
	cur.movePosition(TextCursor::Left, TextCursor::KeepAnchor);
	index_t first = cur.position();
	index_t last = cur.anchor();
	if( first == last ) return;
	do_erase(first, last);
	cur.copyPositionToAnchor();
	m_blockData = cur.blockData();
	m_modified = true;
	emit contentsChange(first, last - first, 0);
	emit contentsChanged();
}
void TextDocument::insertText(TextCursor &cur, const QString &text)
{
	if( cur.isNull() || cur.document() != this )
		return;
	const index_t position = cur.position();
	//if( position > m_buffer.size() ) position = m_buffer.size();
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QByteArray ba = codec->fromUnicode(text);
	const int sz = ba.length();
	const uchar *ptr = (const uchar *)(ba.data());
	//index_t hp_ix = m_undoMgr.addToHeap(ptr, ptr + sz);		//	undone P データはundo時に格納すべき
	size_t delSz = 0;
	if( position == cur.anchor() ) {
		m_buffer.insert(position, ptr, ptr + sz);
		updateBlocksAtInsert(position, cur.blockData(), sz);
#if 1
		m_undoMgr.push_back(GVUndoItem(GVUNDOITEM_TYPE_INSERT, position, position + sz, 0),
							isModified());
#else
		GVUndoItem *undoItem = new (m_pool_undoItem.malloc()) GVUndoItem(GVUNDOITEM_TYPE_INSERT, position, position + sz, 0);
		m_undoMgr.push_back(undoItem, isModified());
#endif
	} else {
		if( cur.anchor() < position )
			cur.swapPositionAnchor();
		const index_t first = cur.position();
		const index_t last = cur.anchor();
		//const index_t blockIndex = cur.blockIndex();
		//const index_t blockPosition = cur.blockPosition();
		const index_t hp_ix = m_undoMgr.addToHeap(m_buffer.begin() + first, m_buffer.begin() + last);
		erase(first, cur.blockData(), last);
		m_buffer.insert(first, ptr, ptr + sz);
		updateBlocksAtInsert(first, cur.blockData(), sz);
#if 1
		m_undoMgr.push_back(GVUndoItem(GVUNDOITEM_TYPE_REPLACE, first, last, hp_ix, first + sz),
							isModified());
#else
		GVUndoItem *undoItem = new (m_pool_undoItem.malloc()) GVUndoItem(GVUNDOITEM_TYPE_REPLACE,
										first, last, hp_ix, first + sz);
		m_undoMgr.push_back(undoItem, isModified());
#endif
		delSz = last - first;
	}
	cur.movePosition(TextCursor::Right, TextCursor::MoveAnchor, text.length());
	m_blockData = cur.blockData();
	m_modified = true;
	emit contentsChange(position, delSz, sz);
	emit contentsChanged();
}
void TextDocument::do_insert(index_t position, const QString &text)
{
	if( position > m_buffer.size() ) position = m_buffer.size();
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QByteArray ba = codec->fromUnicode(text);
	const int sz = ba.length();
	const uchar *ptr = (const uchar *)(ba.data());
	//index_t hp_ix = m_undoMgr.addToHeap(ptr, ptr + sz);		//	undone P データはundo時に格納すべき
	TextBlockData d = findBlockData(position);		//	編集によりキャッシュが無効になる前に取得しておく
	m_buffer.insert(position, ptr, ptr + sz);
	updateBlocksAtInsert(position, d, sz);
#if 1
	m_undoMgr.push_back(GVUndoItem(GVUNDOITEM_TYPE_INSERT, position, position + sz, 0),
						isModified());
#else
	GVUndoItem *undoItem = new (m_pool_undoItem.malloc()) GVUndoItem(GVUNDOITEM_TYPE_INSERT, position, position + sz, 0);
	m_undoMgr.push_back(undoItem, isModified());
#endif
	m_modified = true;
	emit contentsChange(position, 0, sz);
	emit contentsChanged();
}
void TextDocument::do_erase(index_t first, index_t last)
{
	const index_t hp_ix = m_undoMgr.addToHeap(m_buffer.begin() + first, m_buffer.begin() + last);
	erase(first, last);
#if 1
	m_undoMgr.push_back(GVUndoItem(GVUNDOITEM_TYPE_ERASE, first, last, hp_ix),
						isModified());
#else
	GVUndoItem *ptr = new (m_pool_undoItem.malloc()) GVUndoItem(GVUNDOITEM_TYPE_ERASE, first, last, hp_ix);
	m_undoMgr.push_back(ptr, isModified());
#endif
	emit contentsChange(first, last - first, 0);
}
void TextDocument::do_replace(index_t first, index_t last, const QString &text)
{
	if( last > m_buffer.size() ) last = m_buffer.size();
	if( first >= last ) {
		do_insert(first, text);
		return;
	}
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QByteArray ba = codec->fromUnicode(text);
	const int sz = ba.length();
	const uchar *ptr = (const uchar *)(ba.data());
	const index_t hp_ix = m_undoMgr.addToHeap(m_buffer.begin() + first, m_buffer.begin() + last);
	TextBlockData d = findBlockData(first);
	erase(first, last);
	m_buffer.insert(first, ptr, ptr + sz);
	updateBlocksAtInsert(first, d, sz);
#if 1
	m_undoMgr.push_back(GVUndoItem(GVUNDOITEM_TYPE_REPLACE, first, last, hp_ix, first + sz),
						isModified());
#else
	GVUndoItem *undoItem = new (m_pool_undoItem.malloc()) GVUndoItem(GVUNDOITEM_TYPE_REPLACE,
									first, last, hp_ix, first + sz);
	m_undoMgr.push_back(undoItem, isModified());
#endif
	emit contentsChange(first, last - first, sz);
}
void TextDocument::doUndo(index_t &pos)
{
	setModified(m_undoMgr.doUndo(this, pos));
	emit contentsChanged();
}
void TextDocument::doRedo(index_t &pos)
{
	setModified(m_undoMgr.doRedo(this, pos));
	emit contentsChanged();
}

bool TextDocument::isMatch(index_t position, cuchar *first, cuchar *last) const
{
	while( first < last ) {
		if( *first != m_buffer[position] )
			return false;
		++first;
		++position;
	}
	return true;
}
TextCursor TextDocument::find(const QString &text, index_t position)
{
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QByteArray ba = codec->fromUnicode(text);
	const int sz = ba.length();
	const uchar *ptr = (const uchar *)(ba.data());
	//	単純線形検索アルゴリズム
	while( position < size() ) {
		if( isMatch(position, ptr, ptr + sz) )
			return TextCursor(this, position);
		++position;
	}
	return TextCursor();	//	null cursor
}
