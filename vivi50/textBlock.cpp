//----------------------------------------------------------------------
//
//			File:			"textBlock.cpp"
//			Created:		22-Apr-2011
//			Author:			í√ìcêLèG
//			Description:
//
//----------------------------------------------------------------------

#include "textBlock.h"

ViewBlock::ViewBlock(TextView *view, const DocBlock &block)
	: m_view(view), DocBlock(block)
{
}

QString ViewBlock::text() const
{
	if( !isValid() ) return QString();
	//if( index()
	return QString();
}
