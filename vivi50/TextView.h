//----------------------------------------------------------------------
//
//			File:			"TextView.h"
//			Created:		01-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	TextView �N���X�錾
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
#include	"textCursor.h"

class TextView;
class ViewTextCursor;
class TextDocument;
class TextCursor;
class TextBlock;
//class QElapsedTimer;
class QTimer;


class TextView : public QAbstractScrollArea
{
	Q_OBJECT

public:
	TextView(QWidget *parent = 0);
	~TextView();

public:
	uchar	charEncoding() const;
	bool	withBOM() const;
	bool	isModified() const;
	QString	toPlainText() const;
	const TextDocument	*document() const { return m_document; }
	TextBlockData	findBlockData(index_t position) const;
	TextBlockData	nextBlockData(TextBlockData d) const
	{ return TextBlockData(d.m_index + 1, d.m_position + m_blocks[d.m_index].m_size); }
	TextBlockData	prevBlockData(TextBlockData d) const
	{
		if( !d.m_index )
			return TextBlockData(INVALID_INDEX, 0);
		else {
			size_t sz = m_blocks[d.m_index - 1].m_size;
			return TextBlockData(d.m_index - 1, d.m_position - m_blocks[d.m_index - 1].m_size);
		}
	}
	size_t	size() const;	// { return document()->size(); }

public:
	TextDocument	*document() { return m_document; }
	void	doJump(int lineNum);

public:
	void	insertText(ViewTextCursor &, const QString &);
	void	deleteChar(ViewTextCursor &);
	void	deletePreviousChar(ViewTextCursor &);

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
	ViewTextCursor	*m_textCursor;		//	�r���[�p�J�[�\��
	QWidget	*m_lineNumberArea;
	int		m_lineNumberAreaWidth;
	int		m_lineNumberWidth;
	//int		m_lineNumberNDigits;		//	����
	QTimer	*m_timer;					//	�^�C�}�[�I�u�W�F�N�g
	mutable std::gap_vector<ViewTextBlockItem>	m_blocks;		//	�u���b�N�z��
	mutable TextBlockData	m_blockData;			//	�J�����g�u���b�N���
};

#endif // PLAINTEXTEDIT_H
