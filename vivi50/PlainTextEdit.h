//----------------------------------------------------------------------
//
//			File:			"PlainTextEdit.h"
//			Created:		01-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	PlainTextEdit クラス宣言
//
//----------------------------------------------------------------------

/*

	Copyright (C) 2011 by Nobuhide Tsuda

	本ソースコードは基本的に MIT ライセンスに従う。
	http://www.opensource.org/licenses/mit-license.php
	http://sourceforge.jp/projects/opensource/wiki/licenses%2FMIT_license

	ただし、おいらは不自由で使い勝手の悪い GPL が大嫌いなので、
	GPL ライセンスプロジェクトが本ソースを流用することを禁じる

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

public slots:
	void	paste();
	void	undo();
	void	redo();

protected:
	void	paintEvent(QPaintEvent * event);
    void	keyPressEvent ( QKeyEvent * keyEvent );
    //void	resizeEvent ( QResizeEvent * event );

    TextBlock	firstVisibleBlock();
    void	ensureCursorVisible();

protected slots:
	void	onBlockCountChanged();

private:
	TextDocument	*m_document;
	TextCursor		*m_textCursor;		//	暫定的、本当はビュー用カーソルを利用する
	int		m_lineNumberAreaWidth;
	int		m_lineNumberWidth;
};

#endif // PLAINTEXTEDIT_H
