//----------------------------------------------------------------------
//
//			File:			"testBlockData.h"
//			Created:		11-Apr-2011
//			Author:			í√ìcêLèG
//			Description:
//
//----------------------------------------------------------------------

#pragma once

#ifndef		_HEADER_TESTBLOCKDATA_H
#define		_HEADER_TESTBLOCKDATA_H

typedef size_t index_t;

//----------------------------------------------------------------------
struct TextBlockData
{
public:
	index_t		m_index;
	index_t		m_position;
public:
	TextBlockData(index_t index = 0, index_t position = 0)
		: m_index(index), m_position(position)
		{}

public:
	index_t index() const { return m_index; }
	index_t position() const { return m_position; }
};


#endif		//_HEADER_TESTBLOCKDATA_H
