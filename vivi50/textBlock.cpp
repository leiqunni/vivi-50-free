//----------------------------------------------------------------------
//
//			File:			"textBlock.cpp"
//			Created:		22-Apr-2011
//			Author:			�Óc�L�G
//			Description:
//
//----------------------------------------------------------------------

#include "textBlock.h"

ViewBlock::ViewBlock(TextView *view, const DocBlock &block)
	: m_view(view), DocBlock(block)
{
}

