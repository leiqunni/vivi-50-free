//----------------------------------------------------------------------
//
//			File:			"textBlock.h"
//			Created:		22-Apr-2011
//			Author:			�Óc�L�G
//			Description:
//
//----------------------------------------------------------------------

#pragma once

#ifndef		_HEADER_TESTBLOCK_H
#define		_HEADER_TESTBLOCK_H

#include <QString>

typedef size_t index_t;

#define		BLOCK_HAS_SIZE		1
#define		INVALID_INDEX		0xffffffff

class TextDocument;
class TextView;

//----------------------------------------------------------------------
struct BlockData
{
public:
	index_t		m_index;
	index_t		m_position;
public:
	BlockData(index_t index = 0, index_t position = 0)
		: m_index(index), m_position(position)
		{}

public:
	index_t index() const { return m_index; }
	index_t position() const { return m_position; }
};

class DocBlock
{
public:
	DocBlock(TextDocument *document, index_t blockNumber, index_t blockPosition)
		: m_document(document), m_data(BlockData(blockNumber, blockPosition))
		{}
	DocBlock(TextDocument *document, BlockData block)
		: m_document(document), m_data(block)
		{}
	DocBlock(const DocBlock &x)
		: m_document(x.m_document)
#if	BLOCK_HAS_SIZE
		, m_data(x.m_data)
#endif
		{}
	~DocBlock() {}

public:
	size_t		size() const;		//	���s���܂߂��R�[�h��
	size_t		length() const { return size(); }
	index_t		EOLOffset() const;
	size_t		newlineLength() const;		//	���s�����̃o�C�g����Ԃ�
	bool		isValid() const { return m_document != 0 && blockNumber() != INVALID_INDEX; }
	index_t		index() const { return m_data.m_index; }
	index_t		blockNumber() const { return m_data.m_index; }
	index_t		position() const;	// { return isValid() ? m_document->blockPosition(m_index) : 0; }
	BlockData	data() const { return m_data; }
	QString		text() const;
	int			charsCount(index_t) const;		//	�s������w��ʒu�܂ł̕�������Ԃ�

	bool	operator==(const DocBlock &x) const
	{ return m_document == x.m_document && blockNumber() == x.blockNumber(); }
	bool	operator!=(const DocBlock &x) const
	{ return !this->operator==(x); }
	bool	operator<(const DocBlock &x) const
	{ return m_document == x.m_document && blockNumber() < x.blockNumber(); }

public:
	DocBlock	next() const;
	DocBlock	prev() const;

private:
	TextDocument	*m_document;
	BlockData	m_data;
};

class ViewBlock : public DocBlock
{
public:
	ViewBlock(TextView *view, const DocBlock &);

public:
	//index_t		index() const { return m_index; }
	QString		text() const;

private:
	TextView	*m_view;
	BlockData	m_viewBlock;
};


#endif		//_HEADER_TESTBLOCK_H