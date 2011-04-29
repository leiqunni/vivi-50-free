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
#include	"gap_vector.h"

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
	bool		isValid() const;
	//{ return m_document != 0 && blockNumber() >= m_document->blockCount(); }
	index_t		index() const { return m_data.m_index; }
	index_t		blockNumber() const { return m_data.m_index; }
	index_t		position() const;	// { return isValid() ? m_document->blockPosition(m_index) : 0; }
	BlockData	data() const { return m_data; }
	QString		text() const;
	int			charsCount(index_t) const;		//	�s������w��ʒu�܂ł̕�������Ԃ�
	const TextDocument	*document() const { return m_document; }

	bool	operator==(const DocBlock &x) const
	{ return m_document == x.m_document && blockNumber() == x.blockNumber(); }
	bool	operator!=(const DocBlock &x) const
	{ return !this->operator==(x); }
	bool	operator<(const DocBlock &x) const
	{ return m_document == x.m_document && blockNumber() < x.blockNumber(); }

public:
	DocBlock	next() const;
	DocBlock	prev() const;

protected:
	TextDocument	*m_document;
	BlockData	m_data;
};

class ViewBlock : public DocBlock
{
public:
	ViewBlock(TextView *view, const DocBlock &, BlockData);

public:
	//index_t		index() const { return m_index; }
	bool		isFirstBlock() const;	//	DocBlock �̍ŏ��̍s���H
	bool		isLayouted() const;
	size_t		size() const;
	index_t		position() const;
	index_t		docPosition() const { return DocBlock::position(); };
	index_t		docIndex() const { return DocBlock::index(); }
	index_t		docBlockNumber() const { return DocBlock::blockNumber(); }
	index_t		index() const { return m_viewBlock.m_index; }
	index_t		blockNumber() const { return m_viewBlock.m_index; }
	index_t		viewBlockNumber() const { return m_viewBlock.index(); }
	BlockData	data() const { return m_viewBlock; }
	ViewBlock	next() const;
	ViewBlock	prev() const;
	QString		text() const;
	int			charsCount(index_t) const;		//	�s������w��ʒu�܂ł̕�������Ԃ�
	const TextView	*view() const { return m_view; }

	bool	operator==(const ViewBlock &x) const
	{ return m_view == x.m_view && blockNumber() == x.blockNumber(); }
	bool	operator!=(const ViewBlock &x) const
	{ return !this->operator==(x); }
	bool	operator<(const ViewBlock &x) const
	{ return m_view == x.m_view && blockNumber() < x.blockNumber(); }
protected:
	TextView	*m_view;
	BlockData	m_viewBlock;		//	m_position	�s�������ʒu
									//	m_index		�r���[�s�ԍ��i0..*�j
};

struct LaidoutChunk
{
	size_t	m_unLaidoutDocBlockCount;	//	��s���関���C�A�E�g�u���b�N��
	size_t	m_laidoutDocBlockCount;		//	���C�A�E�g�ς݃h�L�������g�u���b�N��
	std::gap_vector<size_t>	m_blocks;	//	���C�A�E�g�ς݃u���b�N���z��

public:
	LaidoutChunk()
		: m_unLaidoutDocBlockCount(0), m_laidoutDocBlockCount(0)
		{}
	LaidoutChunk(size_t unLaidoutDocBlockCount,
					size_t laidoutDocBlockCount,
					const std::gap_vector<size_t> &blocks)
		: m_unLaidoutDocBlockCount(unLaidoutDocBlockCount)
		, m_laidoutDocBlockCount(laidoutDocBlockCount)
		, m_blocks(blocks)
		{}
	LaidoutChunk(const LaidoutChunk &x)
		: m_unLaidoutDocBlockCount(x.m_unLaidoutDocBlockCount)
		, m_laidoutDocBlockCount(x.m_laidoutDocBlockCount)
		, m_blocks(x.m_blocks)
		{}

public:
	size_t	docBlockCount() const
	{ return m_unLaidoutDocBlockCount + m_laidoutDocBlockCount; }
	size_t	viewBlockCount() const
	{ return m_unLaidoutDocBlockCount + m_blocks.size(); }

public:
	LaidoutChunk	&operator=(const LaidoutChunk &x)
	{
		m_unLaidoutDocBlockCount = x.m_unLaidoutDocBlockCount;
		m_laidoutDocBlockCount = x.m_laidoutDocBlockCount;
		m_blocks = x.m_blocks;
		return *this;
	}
};

class LaidoutBlocksMgr
{
public:
	LaidoutBlocksMgr() {}

public:
	size_t	docBlockCount() const;
	size_t	viewcBlockCount() const;
	size_t	viewcBlockSize(index_t, const TextDocument *) const;

public:
	bool	insert(index_t docBlockNumber,		//	�h�L�������g�u���b�N�ԍ��i0..*�j
					size_t docBlockCount,		//	���C�A�E�g�s���i�h�L�������g�u���b�N���j
					const std::gap_vector<size_t> &);		//	���C�A�E�g����

private:
	std::gap_vector<LaidoutChunk>	m_chunks;
};

#endif		//_HEADER_TESTBLOCK_H
