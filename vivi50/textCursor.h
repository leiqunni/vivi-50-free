//----------------------------------------------------------------------
//
//			File:			"textCursor.h"
//			Created:		12-Apr-2011
//			Author:			津田伸秀
//			Description:
//
//----------------------------------------------------------------------

#pragma once

#ifndef		_HEADER_TEXTCURSOR_H
#define		_HEADER_TEXTCURSOR_H

#include	<QtCore>
#include	"textBlock.h"

//	テキストカーソルがブロック情報を含むかどうか
#define		TEXT_CURSOR_BLOCK		1

class TextDocument;
class TextView;
class DocBlock;
class ViewBlock;

typedef size_t index_t;

#define		INVALID_INDEX		0xffffffff

class DocCursor
{
public:
	enum {
		MoveAnchor = 0,
		KeepAnchor,
	};
	enum {
		NoMove = 0,
		Right,
		Left,
		Up,
		Down,
		StartOfWord,
		EndOfWord,
		NextWord,
		PrevWord,
		NextBlock,
		PrevBlock,
		StartOfBlock,
		EndOfBlock,
		StartOfDocument,
		EndOfDocument,
	};
public:
	DocCursor(TextDocument *document = 0, index_t position = 0)
		: m_document(document), m_position(position), m_anchor(position)
		{
#if TEXT_CURSOR_BLOCK
			updateBlockData();
			m_offset = m_position - m_blockData.position();
#endif
		}
	DocCursor(TextDocument *document, index_t position, index_t anchor)
		: m_document(document), m_position(position), m_anchor(anchor)
		{
#if TEXT_CURSOR_BLOCK
			updateBlockData();
			m_offset = m_position - m_blockData.position();
#endif
		}
#if TEXT_CURSOR_BLOCK
	DocCursor(TextDocument *document, index_t position, index_t anchor,
				BlockData blockData)
		: m_document(document), m_position(position), m_anchor(anchor)
		, m_blockData(blockData)
		{
			m_offset = m_position - m_blockData.position();
		}
#endif
	DocCursor(const DocCursor &x)
		: m_document(x.m_document), m_position(x.m_position), m_anchor(x.m_anchor)
#if TEXT_CURSOR_BLOCK
		, m_offset(x.m_offset)
		, m_blockData(x.m_blockData), m_anchorBlockData(x.m_anchorBlockData)
#endif
		{}
	~DocCursor() {}

public:
	const TextDocument	*document() const { return m_document; }
	index_t	position() const { return m_position; }
	index_t	anchor() const { return m_anchor; }
	index_t firstPosition() const { return qMin(position(), anchor()); }
	index_t lastPosition() const { return qMax(position(), anchor()); }
	int		prevCharsCount() const;		//	行頭からカーソルまでの文字数を返す
	bool	hasSelection() const { return m_position != m_anchor; }
	bool	isNull() const { return m_document == 0; }
	bool	atEnd() const;	// { return isNull() || m_position >= m_document->size(); }
	bool	atBlockEnd() const;
	bool	isOverlapped(const DocCursor &) const;
	QString	selectedText() const;
#if TEXT_CURSOR_BLOCK
	DocBlock	block() const;
	BlockData blockData() const { return m_blockData; }
	BlockData anchorBlockData() const { return m_anchorBlockData; }
	index_t positionInBlock() const { return position() - blockPosition(); }
	index_t	blockNumber() const { return m_blockData.m_index; }
	index_t	blockIndex() const { return m_blockData.m_index; }
	index_t	blockPosition() const { return m_blockData.m_position; }
	index_t	ancBlockIndex() const { return m_anchorBlockData.m_index; }
	index_t	ancBlockPosition() const { return m_anchorBlockData.m_position; }
#endif

	bool	operator<(const DocCursor &x) const { return position() < x.position(); }

public:
	TextDocument	*document() { return m_document; }
	void	setAnchor(index_t anchor);	// { m_anchor = anchor; }
	void	clearSelection() { copyPositionToAnchor(); }
	void	copyPositionToAnchor();
	void	copyAnchorToPosition();
	void	swapPositionAnchor();
	void	setPosition(index_t position, uchar mode = MoveAnchor);
	void	setPosition(index_t position, BlockData, uchar mode = MoveAnchor);
	bool	movePosition(uchar move, uchar mode = MoveAnchor, uint n = 1);
	void	move(int);

	size_t	insertText(const QString &);
	void	deleteChar();
	void	deletePreviousChar();

protected:
#if TEXT_CURSOR_BLOCK
	void	updateBlockData(uchar mode = MoveAnchor);		//	m_blockIndex, m_blockPosition 更新
#endif

protected:
	TextDocument	*m_document;
	index_t			m_position;		//	カーソル位置
	index_t			m_anchor;		//	アンカー位置
#if TEXT_CURSOR_BLOCK
	index_t			m_offset;		//	行頭からのオフセット値（本来の値）
	BlockData	m_blockData;
	BlockData	m_anchorBlockData;
#endif
};

//----------------------------------------------------------------------
struct ViewTextBlockItem
{
	size_t		m_size;		//	ブロック内文字サイズ

public:
	ViewTextBlockItem(size_t size = 0) : m_size(size) {}
};

//----------------------------------------------------------------------
class ViewCursor : public DocCursor
{
public:
	ViewCursor(TextView *view = 0, index_t position = 0);
	ViewCursor(TextView *view, index_t position, index_t anchor);
	ViewCursor(TextView *view, index_t position, index_t anchor,
				BlockData blockData);
	ViewCursor(TextView *view, const DocCursor &x);
	ViewCursor(const ViewCursor &x);
	//ViewCursor(const DocCursor &x);
	~ViewCursor() {}

public:
	const TextView	*view() const { return m_view; }
	DocBlock	docBlock() const { return DocCursor::block(); }
	DocBlock	docAnchorBlock() const;
	//DocBlockData	docBlockData() const { return DocBlock::blockData(); }
	ViewBlock	block() const;
	BlockData	viewBlockData() const { return m_viewBlockData; }
	BlockData	viewAnchorBlockData() const { return m_viewAnchorBlockData; }
	BlockData	docBlockData() const { return DocCursor::blockData(); }
	BlockData	docAnchorBlockData() const { return DocCursor::anchorBlockData(); }
	int		prevCharsCount() const;		//	行頭からカーソルまでの文字数を返す
	int		x() const { return m_x; }
	index_t	viewBlockNumber() const { return m_viewBlockData.index(); }
	index_t	viewAnchorBlockNumber() const { return m_viewAnchorBlockData.index(); }


public:
	TextView	*view() { return m_view; }
	void	setX(int x) { m_x = x; }
	void	updateViewBlock();
	//void	assign(const DocCursor &);
	void	setPosition(index_t position, uchar mode = MoveAnchor);
	void	setPosition(index_t position, BlockData, uchar mode = MoveAnchor);
	bool	movePosition(uchar move, uchar mode = MoveAnchor, uint n = 1);
	void	setViewBlockData(BlockData d) { m_viewBlockData = d; }
	void	setViewAnchorBlockData(BlockData d) { m_viewAnchorBlockData = d; }
	void	move(int);

	void	insertText(const QString &);
	void	deleteChar();
	void	deletePreviousChar();

	void	swapPositionAnchor();
	ViewCursor &operator=(const DocCursor &);

protected:
	void	copyPositionToAnchor();
	void	copyAnchorToPosition();
	void	updateBlockData(uchar mode = MoveAnchor);		//	m_blockIndex, m_blockPosition 更新

private:
	TextView	*m_view;
	int			m_x;
	BlockData	m_viewBlockData;
	BlockData	m_viewAnchorBlockData;
};

#endif		//_HEADER_TEXTCURSOR_H
