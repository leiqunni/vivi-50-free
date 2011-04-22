//----------------------------------------------------------------------
//
//			File:			"textBlock.cpp"
//			Created:		22-Apr-2011
//			Author:			í√ìcêLèG
//			Description:
//
//----------------------------------------------------------------------

#include "textBlock.h"
#include "textView.h"

ViewBlock::ViewBlock(TextView *view, const DocBlock &block, BlockData d)
	: m_view(view), DocBlock(block), m_viewBlock(d)
{
}

bool ViewBlock::isLayouted() const
{
	if( !isValid() ) return false;
	return m_view->isLayouted(DocBlock::index());
	//const index_t dbIndex = DocBlock::index();
	//return dbIndex >= m_view->firstViewLine() && dbIndex < m_view->lastViewLine();
}
size_t ViewBlock::size() const
{
	if( !isValid() ) return 0;
	return m_view->blockSize(m_viewBlock.index());
#if 0
	if( !isLayouted() )
		return DocBlock::size();
#endif
}

ViewBlock ViewBlock::next() const
{
	index_t dbIndex = DocBlock::index();
	if( dbIndex < m_view->firstViewLine() ) {
	} else if( dbIndex >= m_view->lastViewLine() ) {
	} else {
	}
	return *this;		//	ébíËÉRÅ[Éh
}
ViewBlock ViewBlock::prev() const
{
	return *this;		//	ébíËÉRÅ[Éh
}

QString ViewBlock::text() const
{
	if( !isValid() ) return QString();
	if( !isLayouted() )
		return DocBlock::text();
	const index_t position = m_viewBlock.position();
	return QString();
}
