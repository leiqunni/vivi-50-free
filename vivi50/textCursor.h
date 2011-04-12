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
#include	"textBlockData.h"

class TextDocument;
class PlainTextEdit;
class TextBlock;

typedef size_t index_t;

#define		INVALID_INDEX		0xffffffff

class TextCursor
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
		StartOfBlock,
		EndOfBlock,
		StartOfDocument,
		EndOfDocument,
	};
public:
	TextCursor(TextDocument *document = 0, index_t position = 0)
		: m_document(document), m_position(position), m_anchor(position)
		{ updateBlockData(); }
	TextCursor(TextDocument *document, index_t position, index_t anchor)
		: m_document(document), m_position(position), m_anchor(anchor)
		{ updateBlockData(); }
	TextCursor(TextDocument *document, index_t position, index_t anchor,
				TextBlockData blockData)
		: m_document(document), m_position(position), m_anchor(anchor)
		, m_blockData(blockData)
		{}
	TextCursor(const TextCursor &x)
		: m_document(x.m_document), m_position(x.m_position), m_anchor(x.m_anchor)
		, m_blockData(x.m_blockData), m_anchorBlockData(x.m_anchorBlockData)
		{}
	~TextCursor() {}

public:
	const TextDocument	*document() const { return m_document; }
	index_t	position() const { return m_position; }
	index_t	anchor() const { return m_anchor; }
	int		prevCharsCount() const;		//	行頭からカーソルまでの文字数を返す
	bool	hasSelection() const { return m_position != m_anchor; }
	bool	isNull() const { return m_document == 0; }
	bool	atEnd() const;	// { return isNull() || m_position >= m_document->size(); }
	QString	selectedText() const;
	TextBlockData blockData() const { return m_blockData; }
	TextBlockData anchorBlock() const { return m_anchorBlockData; }
	index_t	blockIndex() const { return m_blockData.m_index; }
	index_t	blockPosition() const { return m_blockData.m_position; }
	index_t	ancBlockIndex() const { return m_anchorBlockData.m_index; }
	index_t	ancBlockPosition() const { return m_anchorBlockData.m_position; }

public:
	TextDocument	*document() { return m_document; }
	void	setAnchor(index_t anchor) { m_anchor = anchor; }
	void	clearSelection() { copyPositionToAnchor(); }
	TextBlock	block();
	void	copyPositionToAnchor();
	void	copyAnchorToPosition();
	void	swapPositionAnchor();
	void	setPosition(index_t position, uchar mode = MoveAnchor);
	void	setPosition(index_t position, TextBlockData, uchar mode = MoveAnchor);
	bool	movePosition(uchar move, uchar mode = MoveAnchor, uint n = 1);

	void	insertText(const QString &);
	void	deleteChar();
	void	deletePreviousChar();

protected:
	void	updateBlockData(uchar mode = MoveAnchor);		//	m_blockIndex, m_blockPosition 更新

protected:
	TextDocument	*m_document;
	index_t			m_position;		//	カーソル位置
	index_t			m_anchor;		//	アンカー位置
	TextBlockData	m_blockData;
	TextBlockData	m_anchorBlockData;
};

//----------------------------------------------------------------------
struct ViewTextBlockItem
{
	size_t		m_size;		//	ブロック内文字サイズ

public:
	ViewTextBlockItem(size_t size = 0) : m_size(size) {}
};

//----------------------------------------------------------------------
class ViewTextCursor : public TextCursor
{
public:
	ViewTextCursor(PlainTextEdit *view = 0, index_t position = 0);
		//: m_view(view), TextCursor(position)
		//{ updateBlockData(); }
	ViewTextCursor(PlainTextEdit *view, index_t position, index_t anchor);
		//: m_view(view), TextCursor(position, anchor)
		//{ updateBlockData(); }
	ViewTextCursor(PlainTextEdit *view, index_t position, index_t anchor,
				TextBlockData blockData);
		//: m_view(view), TextCursor(position, anchor, blockData)
		//{}
	ViewTextCursor(const ViewTextCursor &x);
		//: m_view(x.m_view), TextCursor(x)
		//{}
	~ViewTextCursor() {}

public:
	const PlainTextEdit	*view() const { return m_view; }

public:
	PlainTextEdit	*view() { return m_view; }
	void	setPosition(index_t position, uchar mode = MoveAnchor);
	void	setPosition(index_t position, TextBlockData, uchar mode = MoveAnchor);
	bool	movePosition(uchar move, uchar mode = MoveAnchor, uint n = 1);

	void	insertText(const QString &);
	void	deleteChar();
	void	deletePreviousChar();

protected:
	void	updateBlockData(uchar mode = MoveAnchor);		//	m_blockIndex, m_blockPosition 更新

private:
	PlainTextEdit	*m_view;
};

#endif		//_HEADER_TEXTCURSOR_H
