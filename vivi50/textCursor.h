//----------------------------------------------------------------------
//
//			File:			"textCursor.h"
//			Created:		12-Apr-2011
//			Author:			�Óc�L�G
//			Description:
//
//----------------------------------------------------------------------

#pragma once

#ifndef		_HEADER_TEXTCURSOR_H
#define		_HEADER_TEXTCURSOR_H

#include	<QtCore>
#include	"textBlockData.h"

class TextDocument;
class TextView;
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
		{
			updateBlockData();
			m_offset = m_position - m_blockData.index();
		}
	TextCursor(TextDocument *document, index_t position, index_t anchor)
		: m_document(document), m_position(position), m_anchor(anchor)
		{
			updateBlockData();
			m_offset = m_position - m_blockData.index();
		}
	TextCursor(TextDocument *document, index_t position, index_t anchor,
				TextBlockData blockData)
		: m_document(document), m_position(position), m_anchor(anchor)
		, m_blockData(blockData)
		{
			m_offset = m_position - m_blockData.index();
		}
	TextCursor(const TextCursor &x)
		: m_document(x.m_document), m_position(x.m_position), m_anchor(x.m_anchor)
		, m_offset(x.m_offset)
		, m_blockData(x.m_blockData), m_anchorBlockData(x.m_anchorBlockData)
		{}
	~TextCursor() {}

public:
	const TextDocument	*document() const { return m_document; }
	index_t	position() const { return m_position; }
	index_t	anchor() const { return m_anchor; }
	index_t firstPosition() const { return qMin(position(), anchor()); }
	index_t lastPosition() const { return qMax(position(), anchor()); }
	int		prevCharsCount() const;		//	�s������J�[�\���܂ł̕�������Ԃ�
	bool	hasSelection() const { return m_position != m_anchor; }
	bool	isNull() const { return m_document == 0; }
	bool	atEnd() const;	// { return isNull() || m_position >= m_document->size(); }
	bool	isOverlapped(const TextCursor &) const;
	QString	selectedText() const;
	TextBlock	block() const;
	TextBlockData blockData() const { return m_blockData; }
	TextBlockData anchorBlockData() const { return m_anchorBlockData; }
	index_t	blockIndex() const { return m_blockData.m_index; }
	index_t	blockPosition() const { return m_blockData.m_position; }
	index_t	ancBlockIndex() const { return m_anchorBlockData.m_index; }
	index_t	ancBlockPosition() const { return m_anchorBlockData.m_position; }

	bool	operator<(const TextCursor &x) const { return position() < x.position(); }

public:
	TextDocument	*document() { return m_document; }
	void	setAnchor(index_t anchor) { m_anchor = anchor; }
	void	clearSelection() { copyPositionToAnchor(); }
	void	copyPositionToAnchor();
	void	copyAnchorToPosition();
	void	swapPositionAnchor();
	void	setPosition(index_t position, uchar mode = MoveAnchor);
	void	setPosition(index_t position, TextBlockData, uchar mode = MoveAnchor);
	bool	movePosition(uchar move, uchar mode = MoveAnchor, uint n = 1);
	void	move(int);

	size_t	insertText(const QString &);
	void	deleteChar();
	void	deletePreviousChar();

protected:
	void	updateBlockData(uchar mode = MoveAnchor);		//	m_blockIndex, m_blockPosition �X�V

protected:
	TextDocument	*m_document;
	index_t			m_position;		//	�J�[�\���ʒu
	index_t			m_offset;		//	�s������̃I�t�Z�b�g�l�i�{���̒l�j
	index_t			m_anchor;		//	�A���J�[�ʒu
	TextBlockData	m_blockData;
	TextBlockData	m_anchorBlockData;
};

//----------------------------------------------------------------------
struct ViewTextBlockItem
{
	size_t		m_size;		//	�u���b�N�������T�C�Y

public:
	ViewTextBlockItem(size_t size = 0) : m_size(size) {}
};

//----------------------------------------------------------------------
class ViewTextCursor : public TextCursor
{
public:
	ViewTextCursor(TextView *view = 0, index_t position = 0);
	ViewTextCursor(TextView *view, index_t position, index_t anchor);
	ViewTextCursor(TextView *view, index_t position, index_t anchor,
				TextBlockData blockData);
	ViewTextCursor(const ViewTextCursor &x);
	//ViewTextCursor(const TextCursor &x);
	~ViewTextCursor() {}

public:
	const TextView	*view() const { return m_view; }

public:
	TextView	*view() { return m_view; }
	//void	assign(const TextCursor &);
	void	setPosition(index_t position, uchar mode = MoveAnchor);
	void	setPosition(index_t position, TextBlockData, uchar mode = MoveAnchor);
	bool	movePosition(uchar move, uchar mode = MoveAnchor, uint n = 1);

	void	insertText(const QString &);
	void	deleteChar();
	void	deletePreviousChar();

	ViewTextCursor &operator=(const TextCursor &);

protected:
	void	updateBlockData(uchar mode = MoveAnchor);		//	m_blockIndex, m_blockPosition �X�V

private:
	TextView	*m_view;
	TextBlockData	m_viewBlockData;
	TextBlockData	m_viewAnchorBlockData;
};

#endif		//_HEADER_TEXTCURSOR_H
