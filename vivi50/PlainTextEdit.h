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

class TextDocument;
class TextCursor;
class TextBlock;

class PlainTextEdit : public QAbstractScrollArea
{
	Q_OBJECT

public:
	PlainTextEdit(QWidget *parent = 0);
	~PlainTextEdit();

public:
	TextDocument	*document() { return m_document; }
	void	doJump(int lineNum);

public slots:
	void	paste();
	void	undo();
	void	redo();
	void	setFontPointSize(int);
	void	setFontFamily(const QString &);
	void	makeFontBigger(bool);

protected:
	bool	eventFilter(QObject *obj, QEvent *event);
	bool	event ( QEvent * event );
	void	paintEvent(QPaintEvent * event);
    void	keyPressEvent ( QKeyEvent * keyEvent );
	void	wheelEvent ( QWheelEvent * event );
    void	focusInEvent ( QFocusEvent * event );
    void	resizeEvent ( QResizeEvent * event );
	void	updateLineNumberAreaSize();
	void	drawLineNumbers();
	void	onFontChanged();

    TextBlock	firstVisibleBlock();
    void	ensureCursorVisible();

protected slots:
	void	onBlockCountChanged();

signals:
	void	showMessage(const QString &);

private:
	TextDocument	*m_document;
	TextCursor		*m_textCursor;		//	�b��I�A�{���̓r���[�p�J�[�\���𗘗p����
	QWidget	*m_lineNumberArea;
	int		m_lineNumberAreaWidth;
	int		m_lineNumberWidth;
};

#endif // PLAINTEXTEDIT_H
