//----------------------------------------------------------------------
//
//			File:			"PlainTextEdit.h"
//			Created:		01-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	PlainTextEdit �N���X�錾
//
//----------------------------------------------------------------------

/*

	Copyright (C) 2011 by Nobuhide Tsuda

	�{�\�[�X�R�[�h�͊�{�I�� MIT ���C�Z���X�ɏ]���B
	http://www.opensource.org/licenses/mit-license.php
	http://sourceforge.jp/projects/opensource/wiki/licenses%2FMIT_license

	�������A������͕s���R�Ŏg������̈��� GPL ���匙���Ȃ̂ŁA
	GPL ���C�Z���X�v���W�F�N�g���{�\�[�X�𗬗p���邱�Ƃ��ւ���

*/

#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H

#include <QAbstractScrollArea>
#include	"gap_vector.h"
#include	"textBlockData.h"

class PlainTextEdit;
class TextDocument;
class TextCursor;
class TextBlock;
//class QElapsedTimer;
class QTimer;

//----------------------------------------------------------------------
struct ViewTextBlockItem
{
	size_t		m_size;		//	�u���b�N�������T�C�Y

public:
	ViewTextBlockItem(size_t size = 0) : m_size(size) {}
};

//----------------------------------------------------------------------
class ViewTextCursor
{
public:
	enum {
		MoveAnchor = 0,
		KeepAnchor,
	};
public:
	ViewTextCursor(PlainTextEdit *view = 0, index_t position = 0)
		: m_view(view), m_position(position), m_anchor(position)
		{ updateBlockData(); }
	ViewTextCursor(PlainTextEdit *view, index_t position, index_t anchor)
		: m_view(view), m_position(position), m_anchor(anchor)
		{ updateBlockData(); }
	ViewTextCursor(PlainTextEdit *view, index_t position, index_t anchor,
				TextBlockData blockData)
		: m_view(view), m_position(position), m_anchor(anchor)
		, m_blockData(blockData)
		{}
	ViewTextCursor(const ViewTextCursor &x)
		: m_view(x.m_view), m_position(x.m_position), m_anchor(x.m_anchor)
		, m_blockData(x.m_blockData), m_anchorBlockData(x.m_anchorBlockData)
		{}
	~ViewTextCursor() {}

public:
	const PlainTextEdit	*view() const { return m_view; }
	index_t	position() const { return m_position; }
	index_t	anchor() const { return m_anchor; }
	int		prevCharsCount() const;		//	�s������J�[�\���܂ł̕�������Ԃ�
	bool	hasSelection() const { return m_position != m_anchor; }
	bool	isNull() const { return m_view == 0; }
	bool	atEnd() const;	// { return isNull() || m_position >= m_document->size(); }
	QString	selectedText() const;
	TextBlockData blockData() const { return m_blockData; }
	TextBlockData anchorBlock() const { return m_anchorBlockData; }
	index_t	blockIndex() const { return m_blockData.m_index; }
	index_t	blockPosition() const { return m_blockData.m_position; }
	index_t	ancBlockIndex() const { return m_anchorBlockData.m_index; }
	index_t	ancBlockPosition() const { return m_anchorBlockData.m_position; }

public:
	PlainTextEdit	*view() { return m_view; }
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
	void	updateBlockData(uchar mode = MoveAnchor);		//	m_blockIndex, m_blockPosition �X�V

private:
	PlainTextEdit	*m_view;
	index_t			m_position;
	index_t			m_anchor;
	TextBlockData	m_blockData;
	TextBlockData	m_anchorBlockData;
};

class PlainTextEdit : public QAbstractScrollArea
{
	Q_OBJECT

public:
	PlainTextEdit(QWidget *parent = 0);
	~PlainTextEdit();

public:
	uchar	charEncoding() const;
	bool	withBOM() const;
	bool	isModified() const;
	QString	toPlainText() const;
	const TextDocument	*document() const { return m_document; }

public:
	TextDocument	*document() { return m_document; }
	void	doJump(int lineNum);

public slots:
	void	copy();
	void	cut();
	void	paste();
	void	undo();
	void	redo();
	void	selectAll();
	void	setFontPointSize(int);
	void	setFontFamily(const QString &);
	void	makeFontBigger(bool);

protected:
	int		offsetToX(const QString &, int) const;		//	��Q�����͕�����
	int		xToOffset(const QString &, int) const;		//	
	TextBlock	yToTextBlock(int) const;
	int		textBlockToY(const TextBlock&) const;		//	�����X�N���[�����l�������Ablock Y���W��Ԃ�
														//	��ʊO�̏ꍇ�� -1 ��Ԃ�
protected:
	bool	eventFilter(QObject *obj, QEvent *event);
	bool	event ( QEvent * event );
	void	paintEvent(QPaintEvent * event);
    void	keyPressEvent ( QKeyEvent * keyEvent );
    void	inputMethodEvent ( QInputMethodEvent * event );
	void	wheelEvent ( QWheelEvent * event );
    void	focusInEvent ( QFocusEvent * event );
    void	resizeEvent ( QResizeEvent * event );
    void	mousePressEvent ( QMouseEvent * event );
    void	mouseReleaseEvent ( QMouseEvent * event );
    void	mouseMoveEvent ( QMouseEvent * event );
    void	mouseDoubleClickEvent ( QMouseEvent * event );
    QVariant	inputMethodQuery ( Qt::InputMethodQuery query ) const;
	void	updateLineNumberAreaSize();
	void	updateScrollBarData();
	void	drawLineNumbers();
	void	onFontChanged();
	void	resetCursorBlinkTimer();

    TextBlock	firstVisibleBlock() const;
    void	ensureCursorVisible();
	int		lineNumberLength() const;			//	�ő�s�ԍ�����

protected slots:
	void	onBlockCountChanged();
	void	onTimer();

signals:
	void	showMessage(const QString &);

private:
	bool	m_mouseCaptured;
	bool	m_toDeleteIMEPreeditText;
	bool	m_drawCursor;
	ViewTextCursor	m_viewTextCursor;
	TextCursor	*m_preeditPosCursor;
	QString	m_preeditString;
	TextDocument	*m_document;
	TextCursor		*m_textCursor;		//	�b��I�A�{���̓r���[�p�J�[�\���𗘗p����
	QWidget	*m_lineNumberArea;
	int		m_lineNumberAreaWidth;
	int		m_lineNumberWidth;
	//int		m_lineNumberNDigits;		//	����
	QTimer	*m_timer;					//	�^�C�}�[�I�u�W�F�N�g
	mutable std::gap_vector<ViewTextBlockItem>	m_blocks;		//	�u���b�N�z��
	mutable TextBlockData	m_blockData;			//	�J�����g�u���b�N���
};

#endif // PLAINTEXTEDIT_H
