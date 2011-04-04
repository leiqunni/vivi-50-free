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
	if( !m_document )
		m_blockIndex = m_blockPosition = m_ancBlockIndex = m_ancBlockPosition = 0;
	else {
		m_blockIndex = m_document->findBlockIndex(m_position, &m_blockPosition);
		if( mode == MoveAnchor ) {
			m_ancBlockIndex = m_blockIndex;
			m_ancBlockPosition = m_blockPosition;
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
TextBlock TextCursor::block()
{
	return TextBlock(m_document, m_blockIndex, m_blockPosition);
}

void TextCursor::copyPositionToAnchor()
{
	m_anchor = m_position;
	m_ancBlockIndex = m_blockIndex;
	m_ancBlockPosition = m_blockPosition;
}
void TextCursor::copyAnchorToPosition()
{
	m_position = m_anchor;
	m_blockIndex = m_ancBlockIndex;
	m_blockPosition = m_ancBlockPosition;
}

void TextCursor::swapPositionAnchor()
{
	index_t t;
	t = m_position; m_position = m_anchor; m_anchor = t;
	t = m_blockIndex; m_blockIndex = m_ancBlockIndex; m_ancBlockIndex = t;
	t = m_blockPosition; m_blockPosition = m_ancBlockPosition; m_ancBlockPosition = t;
}

void TextCursor::setPosition(index_t position, uchar mode)
{
	if( isNull() ) return;
	m_position = position;
	updateBlockData(KeepAnchor);
	if( mode == MoveAnchor ) {
		m_anchor = m_position;
		m_ancBlockIndex = m_blockIndex;
		m_ancBlockPosition = m_blockPosition;
	}
}
bool TextCursor::movePosition(uchar move, uchar mode, uint n)
{
	if( isNull() ) return false;
	switch( move ) {
	case Right:
		if( m_document->size() - m_position <= n ) {
			m_position = m_document->size();
			updateBlockData(KeepAnchor);
		} else {
			//	for UTF-8
			while( n != 0 ) {
				m_position += UTF8CharSize((*m_document)[m_position]);
				--n;
			}
			//	for UTF-16
			//m_position += n;
			index_t next;
			index_t bixLimit = m_document->blockCount() - 1;
			while( m_blockIndex < bixLimit && 
				m_position >= (next = m_blockPosition + m_document->blockSize(m_blockIndex)) )
			{
				++m_blockIndex;
				m_blockPosition = next;
			}
		}
		break;
	case Left:
		if( n >= m_position ) {
			m_position = 0;
			m_blockIndex = 0;
			m_blockPosition = 0;
		} else {
			while( n != 0 ) {
				do { } while( !isUTF8FirstChar((*m_document)[--m_position]) );
				--n;
			}
			while( m_blockIndex > 0 && m_blockPosition > m_position ) {
				m_blockPosition -= m_document->blockSize(--m_blockIndex);
			}
		}
		break;
	default:
		return false;
	}
	if( mode == MoveAnchor ) {
		m_anchor = m_position;
		m_ancBlockIndex = m_blockIndex;
		m_ancBlockPosition = m_blockPosition;
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
void GVUndoMgr::push_back(GVUndoItem *ptr, bool modified)
{
	if( m_current < m_items.size() ) {
		//	undone M 不要なヒープの解放
		m_items.erase(m_items.begin() + m_current, m_items.end());
	}
	if( !modified )
		ptr->m_flags |= BBUNDOITEM_UNDO_MF_OFF;
//#if		!UNDOMGR_USE_OBJECT_POOL
//	m_items.push_back(boost::shared_ptr<GVUndoItem>(ptr));
//#else
	m_items.push_back(ptr);
//#endif
	m_current = m_items.size();
}
//	@return		Undo の結果、モディファイフラグがＯＮの場合は TRUE を返す
bool GVUndoMgr::doUndo(TextDocument *bb, uint& pos)
{
	if( !m_current ) return false;
	//boost::shared_ptr<GVUndoItem> undoItem = m_items[--m_current];
#if 1
	GVUndoItem *ptr = m_items[--m_current];
	switch( ptr->m_type ) {
	case BBUNDOITEM_TYPE_ERASE: {
		cuchar *heap = &m_heap[ptr->m_hp_ix];
		bb->insert(ptr->m_first, heap, heap + ptr->data_size());
		pos = ptr->m_first + ptr->data_size();
		break;
	}
	case BBUNDOITEM_TYPE_INSERT:
		if( ptr->m_rhp_ix == 0 )
			ptr->m_rhp_ix = addToRedoHeap(bb->begin() + ptr->m_first, bb->begin() + ptr->m_last);
		bb->erase(ptr->m_first, ptr->m_last);
		pos = ptr->m_first;
		break;
	case BBUNDOITEM_TYPE_REPLACE: {
		if( ptr->m_rhp_ix == 0 )
			ptr->m_rhp_ix = addToRedoHeap(bb->begin() + ptr->m_first, bb->begin() + ptr->m_last2);
		bb->erase(ptr->m_first, ptr->m_last2);
		cuchar *heap = &m_heap[ptr->m_hp_ix];
		bb->insert(ptr->m_first, heap, heap + ptr->data_size());
		pos = ptr->m_first + ptr->data_size();
		break;
	}
	}
	return !(ptr->m_flags & BBUNDOITEM_UNDO_MF_OFF) ? true : false;
#else
	m_items[--m_current]->doUndo(bb, pos);
	return !(m_items[m_current]->m_flags & BBUNDOITEM_UNDO_MF_OFF) ? true : false;
#endif
	//return true;
}
bool GVUndoMgr::doRedo(TextDocument *bb, uint& pos)
{
	if( m_current >= m_items.size() ) return false;
	//boost::shared_ptr<GVUndoItem> undoItem = m_items[m_current];
	//++m_current;
#if 1
	const GVUndoItem *ptr = m_items[m_current++];
	switch( ptr->m_type ) {
	case BBUNDOITEM_TYPE_ERASE:
		bb->erase(ptr->m_first, ptr->m_last);
		pos = ptr->m_first;
		break;
	case BBUNDOITEM_TYPE_INSERT: {
		cuchar *heap = &m_redoHeap[ptr->m_rhp_ix];
		bb->insert(ptr->m_first, heap, heap + ptr->data_size());
		pos = ptr->m_first + ptr->data_size();
		break;
	}
	case BBUNDOITEM_TYPE_REPLACE: {
		bb->erase(ptr->m_first, ptr->m_last);
		cuchar *heap = &m_redoHeap[ptr->m_rhp_ix];
		bb->insert(ptr->m_first, heap, heap + ptr->data_size2());
		pos = ptr->m_first + ptr->data_size();
		break;
	}
	}
	return !(ptr->m_flags & BBUNDOITEM_UNDO_MF_OFF) ? true : false;
#else
	m_items[m_current]->doRedo(bb, pos);
	return !(m_items[m_current++]->m_flags & BBUNDOITEM_REDO_MF_OFF) ? true : false;;
#endif
}
//----------------------------------------------------------------------
uint TextBlock::size() const
{
#if 1
	return m_document->blockSize(m_blockNumber);
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
	return isValid() ? m_blockPosition : 0;
#else
	return isValid() ? m_document->blockPosition(m_blockNumber) : 0;
#endif
}
	
QString TextBlock::text() const
{
	const size_t sz = size();
	if( !sz ) return QString();
	QByteArray ba;
	ba.reserve(sz);
	for(index_t ix = position(), iend = position() + sz; ix != iend; ++ix)
		ba += (*m_document)[ix];
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	return codec->toUnicode(ba);
}
TextBlock TextBlock::next() const
{
	if( !isValid() ) return *this;
#if BLOCK_HAS_SIZE
	index_t blockPosition = m_blockPosition + m_document->blockSize(m_blockNumber);
	int ix = m_blockNumber + 1;
	if( ix >= m_document->blockCount() )
		ix = INVALID_INDEX;
	return TextBlock(m_document, ix, blockPosition);
#else
	int ix = m_blockNumber + 1;
	if( ix >= m_document->blockCount() )
		ix = INVALID_INDEX;
	return TextBlock(m_document, ix);
#endif
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
	m_buffer.clear();
	m_blocks.clear();
	m_blocks.push_back(TextBlockItem(0));
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
#if BLOCK_HAS_OFFSET
index_t	TextDocument::findBlockIndex(index_t first, index_t last,		//	検索範囲 m_blocks index
										index_t val) const				//	検索インデックス
{
	if( val < blockPosition(first) ) return INVALID_INDEX;
	while( first < last ) {
		uint sz = last - first;
		if( sz == 1 )
			return first;
		index_t bix = first + sz / 2;		//	中央インデックス
		if( val >= blockPosition(bix) )
			first = bix;
		else
			last = bix;
	}
	return INVALID_INDEX;
}
#elif	BLOCK_HAS_SIZE
/**

	┌─────┐
    │          │
    ├─────┤
    │          │
    ├─────┤
    │          │
    ├─────┤
    │    ：    │
    ├─────┤
    │          │
    ├─────┤
    │          │
    └─────┘

*/
index_t	TextDocument::findBlockIndex(index_t position, index_t *pBlockPos) const
{
	if( m_blocks.size() == 1 ) {
		if( pBlockPos != 0 )
			*pBlockPos = 0;
		return 0;
	}
	if( position <= size() / 2 ) {
		int ix = 0;
		index_t blockPos = 0;
		while( ix < m_blocks.size() && position != 0 && position >= m_blocks[ix].m_size ) {
			blockPos += m_blocks[ix].m_size;
			position -= m_blocks[ix].m_size;
			++ix;
		}
		if( pBlockPos != 0 )
			*pBlockPos = blockPos;
		return ix;
	} else {
		int revPos = (int)(size() - position);
		int ix = m_blocks.size() - 1;
		index_t blockPos = size() - m_blocks[ix].m_size;
		while( ix > 0 && revPos > 0 /*&& revPos > m_blocks[ix-1].m_size*/ ) {
			--ix;
			blockPos -= m_blocks[ix].m_size;
			revPos -= m_blocks[ix].m_size;
		}
		if( pBlockPos != 0 )
			*pBlockPos = blockPos;
		return ix;
	}
}
#endif

#if		BLOCK_HAS_SIZE
size_t TextDocument::blockPosition(index_t ix) const
{
	size_t pos = 0;
	for(int i = 0; i < ix; ++i)
		pos += m_blocks[i].m_size;
	return pos;
}
#endif

TextBlock TextDocument::findBlock(index_t position)
{
#if BLOCK_HAS_OFFSET
	if( position > size() ) return TextBlock(this, INVALID_INDEX);
	const index_t ix = findBlockIndex(0, blockCount(), position);
	return TextBlock(this, ix);
#elif	BLOCK_HAS_SIZE
	if( position > size() ) return TextBlock(this, INVALID_INDEX, 0);
	index_t blockPosition;
	const index_t ix = findBlockIndex(position, &blockPosition);
	return TextBlock(this, ix, blockPosition);
#endif
}

void TextDocument::buildBlocks()
{
	m_blocks.clear();
#if	BLOCK_HAS_OFFSET
	m_blocks.push_back(TextBlockItem(0));
	for(uint ix = 0; ix < m_buffer.size();) {
		uchar ch = m_buffer[ix++];
		if( ch == '\n' )
			m_blocks.push_back(TextBlockItem(ix));
		else if( ch == '\r' ) {
			if( ix < m_buffer.size() && m_buffer[ix] == '\n' )
				++ix;
			m_blocks.push_back(TextBlockItem(ix));
		}
	}
#elif	BLOCK_HAS_SIZE
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
#endif
}
void TextDocument::updateBlocksAtInsert(index_t first, index_t blockIndex, index_t blockPosition, size_t sz)
{
	index_t offset = first - blockPosition;		//	挿入位置から行頭までの文字バイト数
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
		m_blocks.insert(blockIndex, TextBlockItem(first - blockPosition));
		++blockIndex;
		//offset = 0;
		blockPosition = first;
	}
	if( ch != '\n' && ch != '\n' )	//	最後の挿入文字が改行で無い場合
		m_blocks[blockIndex].m_size += (last - blockPosition) - offset;
}
void TextDocument::updateBlocksAtInsert(index_t first, size_t sz)
{
#if BLOCK_HAS_OFFSET
	index_t bix = findBlockIndex(0, blockCount(), first);
	index_t blockPos = blockPosition(bix);
	//index_t first = position;
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
		m_blocks.insert(bix, TextBlockItem(blockPos));
		++bix;
		blockPos = first;
	}
	if( ch != '\n' && ch != '\n' )	//	最後の挿入文字が改行で無い場合
		m_blocks[bix++].m_index = blockPos;		//	途中に改行があって、最後に改行が無い場合の処理
	while( bix < blockCount() )
		m_blocks[bix++].m_index += sz;
#elif BLOCK_HAS_SIZE
	index_t blockPos = 0;
	index_t bix = findBlockIndex(first, &blockPos);
	updateBlocksAtInsert(first, bix, blockPos, sz);
#endif
}
//	erase 処理が行われる直前にコールされ、
//	m_blocks を更新
void TextDocument::updateBlocksAtErase(index_t first, index_t blockIndex, index_t blockPosition, index_t last)
{
	size_t sz = last - first;
	index_t bixLast = blockIndex;
	index_t offset = first - blockPosition;		//	削除位置から行頭までの文字バイト数
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
		blockPosition = first;
		++bixLast;
	}
	if( bixLast != blockIndex )
		m_blocks.erase(blockIndex, bixLast);
	m_blocks[blockIndex].m_size += offset - (last - blockPosition);
	//size_t t = m_blocks[blockIndex].m_size;	//	for debug
}
void TextDocument::updateBlocksAtErase(index_t first, index_t last)
{
#if BLOCK_HAS_OFFSET
	size_t sz = last - first;
	index_t bix = findBlockIndex(0, blockCount(), first);
	index_t bixLast = bix;
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
		++bixLast;
	}
	if( bixLast != bix )
		m_blocks.erase(bix + 1, bixLast + 1);
	while( ++bix < blockCount() )
		m_blocks[bix].m_index -= sz;
#elif	BLOCK_HAS_SIZE
	index_t blockPosition = 0;
	index_t blockIndex = findBlockIndex(first, &blockPosition);
	updateBlocksAtErase(first, blockIndex, blockPosition, last);
#endif
}

void TextDocument::erase(index_t first, index_t last)
{
	if( last > size() ) last = size();
	updateBlocksAtErase(first, last);
	m_buffer.erase(first, last);
}
void TextDocument::erase(index_t first, index_t blockIndex, index_t blockPosition, index_t last)
{
	if( last > size() ) last = size();
	updateBlocksAtErase(first, blockIndex, blockPosition, last);
	m_buffer.erase(first, last);
}
void TextDocument::insert(index_t position, index_t blockIndex, index_t blockPosition, const QString &text)
{
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QByteArray ba = codec->fromUnicode(text);
	const int sz = ba.length();
	const uchar *ptr = (const uchar *)(ba.data());
	if( position > m_buffer.size() ) position = m_buffer.size();
	m_buffer.insert(position, ptr, ptr + sz);
	updateBlocksAtInsert(position, blockIndex, blockPosition, sz);
}
void TextDocument::insert(index_t position, const QString &text)
{
	index_t blockPosition = 0;
	index_t blockIndex = findBlockIndex(position, &blockPosition);
	insert(position, blockIndex, blockPosition, text);
}
void TextDocument::insert(index_t ix, index_t blockIndex, index_t blockPosition,
							cuchar *first, cuchar *last)
{
	m_buffer.insert(ix, first, last);
	updateBlocksAtInsert(ix, blockIndex, blockPosition, last - first);
}
void TextDocument::insert(index_t ix, cuchar *first, cuchar *last)
{
	index_t blockPosition = 0;
	index_t blockIndex = findBlockIndex(ix, &blockPosition);
	insert(ix, blockIndex, blockPosition, first, last);
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
	if( position == cur.anchor() ) {
		m_buffer.insert(position, ptr, ptr + sz);
		updateBlocksAtInsert(position, cur.blockIndex(), cur.blockPosition(), sz);
		GVUndoItem *undoItem = new (m_pool_undoItem.malloc()) GVUndoItem(BBUNDOITEM_TYPE_INSERT, position, position + sz, 0);
		m_undoMgr.push_back(undoItem);
		emit contentsChange(position, 0, sz);
	} else {
		if( cur.anchor() < position )
			cur.swapPositionAnchor();
		const index_t first = cur.position();
		const index_t last = cur.anchor();
		const index_t blockIndex = cur.blockIndex();
		const index_t blockPosition = cur.blockPosition();
		const index_t hp_ix = m_undoMgr.addToHeap(m_buffer.begin() + first, m_buffer.begin() + last);
		erase(first, blockIndex, blockPosition, last);
		m_buffer.insert(first, ptr, ptr + sz);
		updateBlocksAtInsert(first, blockIndex, blockPosition, sz);
		GVUndoItem *undoItem = new (m_pool_undoItem.malloc()) GVUndoItem(BBUNDOITEM_TYPE_REPLACE,
										first, last, hp_ix, first + sz);
		m_undoMgr.push_back(undoItem);
		emit contentsChange(first, last - first, sz);
	}
	cur.movePosition(TextCursor::Right, TextCursor::MoveAnchor, text.length());
}
void TextDocument::do_insert(index_t position, const QString &text)
{
	if( position > m_buffer.size() ) position = m_buffer.size();
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QByteArray ba = codec->fromUnicode(text);
	const int sz = ba.length();
	const uchar *ptr = (const uchar *)(ba.data());
	//index_t hp_ix = m_undoMgr.addToHeap(ptr, ptr + sz);		//	undone P データはundo時に格納すべき
	m_buffer.insert(position, ptr, ptr + sz);
	updateBlocksAtInsert(position, sz);
	GVUndoItem *undoItem = new (m_pool_undoItem.malloc()) GVUndoItem(BBUNDOITEM_TYPE_INSERT, position, position + sz, 0);
	m_undoMgr.push_back(undoItem);
	emit contentsChange(position, 0, sz);
}
void TextDocument::do_erase(index_t first, index_t last)
{
	const index_t hp_ix = m_undoMgr.addToHeap(m_buffer.begin() + first, m_buffer.begin() + last);
	erase(first, last);
	GVUndoItem *ptr = new (m_pool_undoItem.malloc()) GVUndoItem(BBUNDOITEM_TYPE_ERASE, first, last, hp_ix);
	m_undoMgr.push_back(ptr);
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
	erase(first, last);
	m_buffer.insert(first, ptr, ptr + sz);
	updateBlocksAtInsert(first, sz);
	GVUndoItem *undoItem = new (m_pool_undoItem.malloc()) GVUndoItem(BBUNDOITEM_TYPE_REPLACE,
									first, last, hp_ix, first + sz);
	m_undoMgr.push_back(undoItem);
	emit contentsChange(first, last - first, sz);
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
