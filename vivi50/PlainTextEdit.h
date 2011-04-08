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

typedef size_t index_t;

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

protected:
	bool	eventFilter(QObject *obj, QEvent *event);
	bool	event ( QEvent * event );
	void	paintEvent(QPaintEvent * event);
    void	keyPressEvent ( QKeyEvent * keyEvent );
    void	inputMethodEvent ( QInputMethodEvent * event );
	void	wheelEvent ( QWheelEvent * event );
    void	focusInEvent ( QFocusEvent * event );
    void	resizeEvent ( QResizeEvent * event );
	void	updateLineNumberAreaSize();
	void	updateScrollBarData();
	void	drawLineNumbers();
	void	onFontChanged();

    TextBlock	firstVisibleBlock();
    void	ensureCursorVisible();
	int		lineNumberLength() const;			//	�ő�s�ԍ�����

protected slots:
	void	onBlockCountChanged();

signals:
	void	showMessage(const QString &);

private:
	bool	m_toDeleteIMEPreeditText;
	TextDocument	*m_document;
	TextCursor		*m_textCursor;		//	�b��I�A�{���̓r���[�p�J�[�\���𗘗p����
	QWidget	*m_lineNumberArea;
	int		m_lineNumberAreaWidth;
	int		m_lineNumberWidth;
	//int		m_lineNumberNDigits;		//	����
};

#endif // PLAINTEXTEDIT_H
