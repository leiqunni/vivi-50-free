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

#define		SIMPLE_LAIDOUT_BLOCKS	1
#define		BLOCK_HAS_SIZE		1
#define		INVALID_INDEX		0xffffffff

class TextDocument;
class TextView;
class LaidoutBlock;

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

	DocBlock	&operator++();

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

public:
	ViewBlock	&operator++();

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
	{
		return m_unLaidoutDocBlockCount + m_laidoutDocBlockCount;
	}
	size_t	viewBlockCount() const
	{
		return m_unLaidoutDocBlockCount + m_blocks.size();
	}

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
	LaidoutBlocksMgr(TextDocument *document);
		//: m_document(document)
		//{}

public:
	size_t	docBlockCount() const;		//	LaidoutBlocksMgr ���Ǘ����Ă���s��
	size_t	viewBlockCount() const;		//	LaidoutBlocksMgr ���Ǘ����Ă���s��
	size_t	size() const;				//	�r���[�̃g�[�^���s��
	size_t	blockCount() const { return size(); }
	size_t	viewBlockSize(index_t) const;
	size_t	blockSize(index_t ix) const { return viewBlockSize(ix); }

	size_t	blockNumberFromDocBlockNumber(index_t) const;
	size_t	docBlockNumberFromNumber(index_t) const;

	int		width() const { return m_width; }

public:
	void	clear();
	void	setWidth(int width) { m_width = width; }
	bool	insert(index_t docBlockNumber,		//	�h�L�������g�u���b�N�ԍ��i0..*�j
					size_t docBlockCount,		//	���C�A�E�g�s���i�h�L�������g�u���b�N���j
					const std::gap_vector<size_t> &);		//	���C�A�E�g����

	LaidoutBlock	begin() const;
	LaidoutBlock	end() const;
	LaidoutBlock	findBlock(index_t position) const;
	LaidoutBlock	findBlockByNumber(index_t) const;
	LaidoutBlock	findBlockByDocNumber(index_t) const;

public:
	void	erase(index_t first, index_t last) { m_blockSize.erase(first, last); }
	void	buildBlocks(TextView *,
						DocBlock block,		//	[���C�A�E�g�J�n�ʒu
						index_t vIndex = 0,		//	[���C�A�E�g�J�n�ʒu
						int ht = 0,				//	���C�A�E�g�͈�);
						index_t diLimit = 0);	//	���C�A�E�g�͈�);
	void	buildBlocksUntillDocBlockNumber(TextView *,
						DocBlock block,		//	[���C�A�E�g�J�n�ʒu
						index_t vIndex = 0,		//	[���C�A�E�g�J�n�ʒu
						int ht = 0,				//	���C�A�E�g�͈�);
						index_t docBlockNumber = 0);	//	���C�A�E�g�͈�);

protected:
	const TextDocument	*document() const { return m_document; }
	TextDocument	*document() { return m_document; }


private:
	TextDocument	*m_document;
	int		m_width;
	mutable LaidoutBlock	*m_cacheBlock;
#if SIMPLE_LAIDOUT_BLOCKS
	std::gap_vector<size_t>	m_blockSize;		//	�r���[�u���b�N���A0 �Ȃ�͖����C�A�E�g
#else
	std::gap_vector<LaidoutChunk>	m_chunks;
#endif

	friend class LaidoutBlock;
};

class LaidoutBlock
{
public:
	LaidoutBlock(/*TextView *view,*/ LaidoutBlocksMgr *lbMgr)
		: /*m_view(view),*/ m_lbMgr(lbMgr)
		, m_viewBlockData(0, 0), m_docBlockData(0, 0)
#if !SIMPLE_LAIDOUT_BLOCKS
		, m_chunkIndex(0), m_indexInChunk(0)
#endif
		{}
	LaidoutBlock(LaidoutBlocksMgr *lbMgr, BlockData viewBlockData, BlockData docBlockData)
		: m_lbMgr(lbMgr)
		, m_viewBlockData(viewBlockData), m_docBlockData(docBlockData)
		{}

public:
	bool	isValid() const;
	bool	isLayouted() const;
	index_t	position() const { return m_viewBlockData.m_position; }
	index_t	index() const { return m_viewBlockData.m_index; }
	index_t	blockNumber() const { return m_viewBlockData.m_index; }
	index_t	docPosition() const { return m_docBlockData.m_position; }
	index_t	docIndex() const { return m_docBlockData.m_index; }
	index_t	docBlockNumber() const { return m_docBlockData.m_index; }
	BlockData	viewBlockData() const { return BlockData(blockNumber(), position()); }
	BlockData	docBlockData() const { return BlockData(docBlockNumber(), docPosition()); }

	size_t	size() const;
	QString	text() const;


	bool	operator==(const LaidoutBlock &x) const { return index() == x.index(); }

public:
	TextDocument	*document() { return m_lbMgr->m_document; }
	void			moveToEndOfDocument();
	LaidoutBlock	&operator++();
	LaidoutBlock	&operator--();

private:
	LaidoutBlocksMgr	*m_lbMgr;
	BlockData	m_viewBlockData;
	BlockData	m_docBlockData;
#if		SIMPLE_LAIDOUT_BLOCKS
#else
	index_t		m_chunkIndex;
	index_t		m_indexInChunk;
	//TextView	*m_view;
#endif
};

#endif		//_HEADER_TESTBLOCK_H
