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
DocBlock DocBlock::next() const
{
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

ViewBlock ViewBlock::next() const
{
	const index_t dbIndex = DocBlock::index();
	ViewBlock b(*this);
	if( dbIndex >= m_view->firstUnlayoutedBlockCount() &&
		dbIndex < m_view->firstUnlayoutedBlockCount() + m_view->layoutedBlockCount() )
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
