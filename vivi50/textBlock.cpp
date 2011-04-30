//----------------------------------------------------------------------
//
//			File:			"textBlock.cpp"
//			Created:		22-Apr-2011
//			Author:			津田伸秀
//			Description:
//
//----------------------------------------------------------------------

#include "textBlock.h"
#include "textView.h"
#include "TextDocument.h"

size_t UTF8CharSize(uchar ch);

size_t DocBlock::size() const
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
index_t DocBlock::position() const
{
	return isValid() ? m_data.m_position : 0;
}
size_t DocBlock::newlineLength() const
{
	if( !isValid() ) return 0;
	const size_t sz = size();
	index_t np = position() + sz;
	if( np > position() ) {
		uchar uch = (*m_document)[np-1];
		if( uch == '\r' )
			return 1;
		if( uch == '\n' ) {
			if( np - 2 >= position() && (*m_document)[np-2] == '\r' )
				return 2;
			else
				return 1;
		}
	}
	return 0;
}
index_t DocBlock::EOLOffset() const
{
	if( !isValid() ) return 0;
	return size() - newlineLength();
#if 0
	const size_t sz = size();
	index_t np = position() + sz;
	if( np > position() ) {
		uchar uch = (*m_document)[np-1];
		if( uch == '\r' )
			return sz - 1;
		if( uch == '\n' ) {
			if( np - 2 >= position() && (*m_document)[np-2] == '\r' )
				return sz - 2;
			else
				return sz - 1;
		}
	}
	return sz;
#endif
}

bool DocBlock::isValid() const
{
	return m_document != 0 && blockNumber() < m_document->blockCount();
}

int DocBlock::charsCount(index_t position) const
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
	
QString DocBlock::text() const
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
DocBlock &DocBlock::operator++()
{
	if( !isValid() ) return *this;
	m_data = m_document->nextBlockData(m_data);
	if( index() >= m_document->blockCount() )
		m_data.m_index = INVALID_INDEX;
	return *this;
}
DocBlock DocBlock::next() const
{
	DocBlock b(*this);
	return ++b;
#if 0
	if( !isValid() ) return *this;
#if 1
	BlockData d = m_document->nextBlockData(m_data);
	if( d.index() >= m_document->blockCount() )
		d.m_index = INVALID_INDEX;
	return DocBlock(m_document, d);
#else
	index_t blockPosition = m_data.m_position + m_document->blockSize(m_data.m_index);
	int ix = m_data.m_index + 1;
	if( ix >= m_document->blockCount() )
		ix = INVALID_INDEX;
	return DocBlock(m_document, ix, blockPosition);
#endif
#endif
}
DocBlock DocBlock::prev() const
{
	if( !isValid() ) return *this;
	BlockData d = m_document->prevBlockData(m_data);
	return DocBlock(m_document, d);
}
//----------------------------------------------------------------------
ViewBlock::ViewBlock(TextView *view, const DocBlock &block, BlockData d)
	: m_view(view), DocBlock(block), m_viewBlock(d)
{
}

index_t ViewBlock::position() const
{
	if( !isLayouted() )
		return DocBlock::position();
	else
		return m_viewBlock.m_position;
}
bool ViewBlock::isFirstBlock() const
{
	if( !isLayouted() ) return true;
	return DocBlock::position() == m_viewBlock.m_position;
}

bool ViewBlock::isLayouted() const
{
	if( !isValid() ) return false;
	return m_view->isLayoutedDocBlock(DocBlock::index());
	//const index_t dbIndex = DocBlock::index();
	//return dbIndex >= m_view->firstViewLine() && dbIndex < m_view->lastViewLine();
}
size_t ViewBlock::size() const
{
	if( !isValid() ) return 0;
#if 0
	if( !isLayouted() )
		return DocBlock::size();
#endif
	return m_view->blockSize(m_viewBlock.index());
}

ViewBlock &ViewBlock::operator++()
{
	const index_t dbIndex = DocBlock::index();
	if( dbIndex >= m_view->firstUnlayoutedBlockCount() &&
		dbIndex < m_view->firstUnlayoutedBlockCount() + m_view->layoutedDocBlockCount() )
	{
		//	レイアウト済みブロックの場合
		const size_t docBlockSize = document()->blockSize(m_data.m_index);
		const index_t nextDocBlockPos = m_data.m_position + docBlockSize;
		const size_t sz = view()->blockSize(m_viewBlock.m_index++);
		if( !sz ||		//	空のEOF行の場合はサイズ０
			(m_viewBlock.m_position += sz) == nextDocBlockPos )
		{
			m_data.m_position = nextDocBlockPos;
			++m_data.m_index;
		}
	} else {	//	非レイアウトブロックの場合
		const size_t sz = document()->blockSize(m_data.m_index++);
		m_viewBlock.m_position = m_data.m_position += sz;
		++m_viewBlock.m_index;
	}
	return *this;
}
ViewBlock ViewBlock::next() const
{
	ViewBlock b(*this);
	return ++b;
#if 0
	const index_t dbIndex = DocBlock::index();
	ViewBlock b(*this);
	if( dbIndex >= m_view->firstUnlayoutedBlockCount() &&
		dbIndex < m_view->firstUnlayoutedBlockCount() + m_view->layoutedDocBlockCount() )
	{
		//	レイアウト済みブロックの場合
		const size_t docBlockSize = document()->blockSize(b.m_data.m_index);
		const index_t nextDocBlockPos = b.m_data.m_position + docBlockSize;
		const size_t sz = view()->blockSize(b.m_viewBlock.m_index++);
		if( !sz ||		//	空のEOF行の場合はサイズ０
			(b.m_viewBlock.m_position += sz) == nextDocBlockPos )
		{
			b.m_data.m_position = nextDocBlockPos;
			++b.m_data.m_index;
		}
	} else {	//	非レイアウトブロックの場合
		const size_t sz = document()->blockSize(b.m_data.m_index++);
		b.m_viewBlock.m_position = b.m_data.m_position += sz;
		++b.m_viewBlock.m_index;
	}
	return b;
#endif
}
ViewBlock ViewBlock::prev() const
{
	return *this;		//	暫定コード
}

QString ViewBlock::text() const
{
	if( !isValid() ) return QString();
	if( !isLayouted() )
		return DocBlock::text();
	const index_t position = m_viewBlock.position();
	const size_t sz = m_view->blockSize(m_viewBlock.index());
	QByteArray ba;
	ba.reserve(sz);
	const TextDocument *doc = document();
	for(index_t ix = position, iend = position + sz; ix != iend; ++ix)
		ba += doc->at(ix);
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	return codec->toUnicode(ba);
}
int ViewBlock::charsCount(index_t position) const
{
	if( !isValid() || position <= m_viewBlock.position() ||
		position > m_viewBlock.position() + view()->blockSize(m_viewBlock.index()) )
	{
		return 0;
	}
	int cnt = 0;
	int ix = m_viewBlock.position();
	while( ix < position ) {
		++cnt;
		ix += UTF8CharSize((*m_document)[ix]);
	}
	return cnt;
}
//----------------------------------------------------------------------
LaidoutBlocksMgr::LaidoutBlocksMgr(TextDocument *document)
	: m_document(document)
{
	m_cacheBlock = new LaidoutBlock(this);
}

LaidoutBlock LaidoutBlocksMgr::begin()
{
	return LaidoutBlock(this);
}
LaidoutBlock LaidoutBlocksMgr::end()
{
	LaidoutBlock b = LaidoutBlock(this);
	b.moveToEndOfDocument();
	return b;
}
LaidoutBlock LaidoutBlocksMgr::findBlock(index_t pos)
{
	LaidoutBlock b = begin();
	if( !m_cacheBlock->position() ) {
		if( pos < document()->size() / 2 ) {
			while( b.position() + b.size() <= pos )
				++b;
		} else {
			b = end();
			do { --b; } while( b.position() > pos );
		}
	} else {
		if( pos == m_cacheBlock->position() )
			return *m_cacheBlock;
		if( pos < m_cacheBlock->position() ) {
			if( pos < m_cacheBlock->position() / 2 ) {
				while( b.position() + b.size() <= pos )
					++b;
			} else {
				b = *m_cacheBlock;
				do { --b; } while( b.position() > pos );
			}
		} else {
			if( pos < m_cacheBlock->position() +
				(document()->size() - m_cacheBlock->position())/ 2 )
			{
				b = *m_cacheBlock;
				while( b.position() + b.size() <= pos )
					++b;
			} else {
				b = end();
				do { --b; } while( b.position() > pos );
			}
		}
	}
	return *m_cacheBlock = b;
}
LaidoutBlock LaidoutBlocksMgr::findBlockByNumber(index_t number)
{
	LaidoutBlock b = begin();
	if( !m_cacheBlock->position() ) {
		if( number < size() / 2 ) {
			while( b.index() < number )
				++b;
		} else {
			b = end();
			while( b.index() > number )
				--b;
		}
	} else {
		if( number == m_cacheBlock->index() )
			return *m_cacheBlock;
		if( number < m_cacheBlock->index() ) {
			if( number < m_cacheBlock->index() / 2 ) {
				while( b.index() < number )
					++b;
			} else {
				b = *m_cacheBlock;
				while( b.index() > number )
					--b;
			}
		} else {
			if( number < m_cacheBlock->index() + (size() - m_cacheBlock->index())/ 2 ) {
				b = *m_cacheBlock;
				while( b.index() < number )
					++b;
			} else {
				b = end();
				while( b.index() > number )
					--b;
			}
		}
	}
	return *m_cacheBlock = b;
}

size_t LaidoutBlocksMgr::docBlockCount() const
{
	size_t sum = 0;
	for(std::gap_vector<LaidoutChunk>::const_iterator itr = m_chunks.begin(),
														iend = m_chunks.end();
		itr != iend; ++itr)
	{
		sum += itr->docBlockCount();
	}
	return sum;
}
size_t LaidoutBlocksMgr::viewBlockCount() const
{
	size_t sum = 0;
	for(std::gap_vector<LaidoutChunk>::const_iterator itr = m_chunks.begin(),
														iend = m_chunks.end();
		itr != iend; ++itr)
	{
		sum += itr->viewBlockCount();
	}
	return sum;
}
size_t LaidoutBlocksMgr::size() const
{
	size_t sz = document()->size();
	return viewBlockCount() + sz - docBlockCount();
}
size_t LaidoutBlocksMgr::viewBlockSize(index_t viewBlockNumber) const
{
	size_t sumVBC = 0;
	index_t docBlockNumber = 0;
	for(std::gap_vector<LaidoutChunk>::const_iterator itr = m_chunks.begin(),
														iend = m_chunks.end();
		itr != iend; ++itr)
	{
		if( viewBlockNumber < sumVBC + itr->m_unLaidoutDocBlockCount )
			return m_document->blockSize(viewBlockNumber - sumVBC + docBlockNumber);
		const size_t nextVBC = sumVBC + itr->viewBlockCount();
		if( viewBlockNumber < nextVBC )
			return itr->m_blocks[viewBlockNumber - sumVBC - itr->m_unLaidoutDocBlockCount];
		sumVBC = nextVBC;
		docBlockNumber += itr->docBlockCount();
	}
	return m_document->blockSize(viewBlockNumber - sumVBC + docBlockNumber);
}
bool LaidoutBlocksMgr::insert(index_t docBlockNumber,		//	挿入位置
						size_t docLaidoutBlockCount,		//	レイアウト行数（ドキュメントブロック数）
						const std::gap_vector<size_t> &v)		//	レイアウト結果
{
	const size_t dbCount = docBlockCount();
	if( m_chunks.empty() || docBlockNumber > dbCount ) {
		m_chunks.push_back(LaidoutChunk(docBlockNumber - dbCount, docLaidoutBlockCount, v));
		return true;
	}
	if( docBlockNumber == dbCount ) {	//	最後のチャンク直後に追加の場合
		std::gap_vector<LaidoutChunk>::iterator itr = m_chunks.end() - 1;
		itr->m_laidoutDocBlockCount += docLaidoutBlockCount;
		itr->m_blocks.insert(itr->m_blocks.end(), v.begin(), v.end());
		return true;
	}
	if( docBlockNumber + docLaidoutBlockCount == m_chunks[0].m_unLaidoutDocBlockCount ) {
		//	最初のチャンクの直前に追加
		m_chunks[0].m_unLaidoutDocBlockCount -= docLaidoutBlockCount;
		m_chunks[0].m_laidoutDocBlockCount += docLaidoutBlockCount;
		m_chunks[0].m_blocks.insert(m_chunks[0].m_blocks.begin(), v.begin(), v.end());
		return true;
	}
	if( docBlockNumber + docLaidoutBlockCount < m_chunks[0].m_unLaidoutDocBlockCount ) {
		//	最初のチャンクの前に追加
		m_chunks[0].m_unLaidoutDocBlockCount -= docBlockNumber + docLaidoutBlockCount;
		m_chunks.insert(0, LaidoutChunk(docBlockNumber, docLaidoutBlockCount, v));
		return true;
	}
	index_t dbn = 0;
	for(std::gap_vector<LaidoutChunk>::iterator itr = m_chunks.begin(), iend = m_chunks.end();;)
	{
		std::gap_vector<LaidoutChunk>::iterator inext = itr + 1;
		if( inext == iend ) break;
		dbn += itr->docBlockCount();
		if( docBlockNumber == dbn ) {		//	itr の直後に追加の場合
			if( docLaidoutBlockCount < inext->m_unLaidoutDocBlockCount ) {
				//	次のチャンクに連続していない場合
				itr->m_laidoutDocBlockCount += docLaidoutBlockCount;
				itr->m_blocks.insert(itr->m_blocks.end(), v.begin(), v.end());
				inext->m_unLaidoutDocBlockCount -= docLaidoutBlockCount;
			} else {
				//	次のチャンクに連続している場合
				itr->m_laidoutDocBlockCount += docLaidoutBlockCount + inext->m_laidoutDocBlockCount;
				itr->m_blocks.insert(itr->m_blocks.end(), v.begin(), v.end());
				itr->m_blocks.insert(itr->m_blocks.end(),
										inext->m_blocks.begin(), inext->m_blocks.end());
				m_chunks.erase(inext);
			}
			return true;
		}
		itr = inext;
	}
	//	undone B チャンクが連続している場合等
	return false;
}
//----------------------------------------------------------------------
bool LaidoutBlock::isValid() const
{
	return m_docBlockData.m_index < m_lbMgr->m_document->blockCount();
}
size_t LaidoutBlock::size() const
{
	if( m_chunkIndex >= m_lbMgr->m_chunks.size() )
		return m_lbMgr->m_document->blockSize(m_docBlockData.m_index);
	const size_t u = m_lbMgr->m_chunks[m_chunkIndex].m_unLaidoutDocBlockCount;
	if( m_indexInChunk < u )
		return m_lbMgr->m_document->blockSize(m_docBlockData.m_index);
	else
		return m_lbMgr->m_chunks[m_chunkIndex].m_blocks[m_indexInChunk - u];
}
QString LaidoutBlock::text() const
{
	const size_t sz = size();
	if( !sz ) return QString();
	const index_t pos = position();
	const TextDocument *doc = m_lbMgr->m_document;
	if( pos >= doc->size() ) return QString();
	QByteArray ba;
	ba.reserve(sz);
	for(index_t ix = pos, iend = pos + sz; ix != iend; ++ix)
		ba += doc->at(ix);
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	return codec->toUnicode(ba);
}
void LaidoutBlock::moveToEndOfDocument()
{
	m_indexInChunk = document()->blockCount()- m_lbMgr->docBlockCount();
	m_chunkIndex = m_lbMgr->m_chunks.size();
	m_docBlockData.m_index = document()->blockCount();
	m_docBlockData.m_position = m_viewBlockData.m_position = document()->size();
	m_viewBlockData.m_index = m_lbMgr->viewBlockCount() + m_indexInChunk;
}
LaidoutBlock &LaidoutBlock::operator++()
{
	++m_viewBlockData.m_index;
	index_t nextDocPosition = m_docBlockData.m_position +
								m_lbMgr->m_document->blockSize(m_docBlockData.m_index);
	if( m_chunkIndex >= m_lbMgr->m_chunks.size() ) {
		++m_docBlockData.m_index;
		m_viewBlockData.m_position = m_docBlockData.m_position = nextDocPosition;
		return *this;
	}
	const size_t u = m_lbMgr->m_chunks[m_chunkIndex].m_unLaidoutDocBlockCount;
	if( m_indexInChunk < u ) {
		++m_indexInChunk;
		++m_docBlockData.m_index;
		m_viewBlockData.m_position = m_docBlockData.m_position = nextDocPosition;
		return *this;
	}
	const size_t indexLimit = u + m_lbMgr->m_chunks[m_chunkIndex].m_blocks.size();
	if( m_indexInChunk < indexLimit ) {
		m_viewBlockData.m_position += m_lbMgr->m_chunks[m_chunkIndex].m_blocks[m_indexInChunk - u];
		if( m_viewBlockData.m_position == nextDocPosition ) {
			++m_docBlockData.m_index;
			m_docBlockData.m_position = nextDocPosition;
		}
		if( ++m_indexInChunk == indexLimit ) {
			++m_chunkIndex;
			m_indexInChunk = 0;
		}
		return *this;
	}
	++m_chunkIndex;
	m_indexInChunk = 0;
	return *this;
}
LaidoutBlock &LaidoutBlock::operator--()
{
	--m_viewBlockData.m_index;
	if( !m_indexInChunk ) {
		if( !m_chunkIndex ) {
			moveToEndOfDocument();
			return *this;
		} else {
			--m_chunkIndex;
			//const size_t u = m_lbMgr->m_chunks[m_chunkIndex].m_unLaidoutDocBlockCount;
			m_indexInChunk = m_lbMgr->m_chunks[m_chunkIndex].m_unLaidoutDocBlockCount + 
								m_lbMgr->m_chunks[m_chunkIndex].m_blocks.size() - 1;
		}
	} else {
		--m_indexInChunk;
		if( m_chunkIndex >= m_lbMgr->m_chunks.size() ) {
			m_viewBlockData.m_position =
			m_docBlockData.m_position -= m_lbMgr->m_document->blockSize(--m_docBlockData.m_index);
			return *this;
		}
	}
	const size_t u = m_lbMgr->m_chunks[m_chunkIndex].m_unLaidoutDocBlockCount;
	if( m_indexInChunk < u ) {
		m_viewBlockData.m_position =
		m_docBlockData.m_position -= m_lbMgr->m_document->blockSize(--m_docBlockData.m_index);
	} else {
		m_viewBlockData.m_position -= m_lbMgr->m_chunks[m_chunkIndex].m_blocks[m_indexInChunk - u];
		if( m_viewBlockData.m_position < m_docBlockData.m_position )
			m_docBlockData.m_position -= m_lbMgr->m_document->blockSize(--m_docBlockData.m_index);
	}
	return *this;
}
