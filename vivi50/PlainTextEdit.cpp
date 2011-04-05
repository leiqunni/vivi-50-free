//----------------------------------------------------------------------
//
//			File:			"PlainTextEdit.cpp"
//			Created:		01-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	PlainTextEdit クラス実装
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

#include <QtGui>
#include "PlainTextEdit.h"
#include	"TextDocument.h"
#include	<QDebug>

#define		MARGIN_LEFT		4
#define		LINE_HEIGHT		20

PlainTextEdit::PlainTextEdit(QWidget *parent)
	: QAbstractScrollArea(parent)
{
	m_textDocument = new TextDocument();
	m_textCursor = new TextCursor(m_textDocument);
	connect(m_textDocument, SIGNAL(blockCountChanged()), this, SLOT(onBlockCountChanged()));

	m_textDocument->setPlainText(QString("LINE-1\nLINE-2\nLINE-3\n"));
}

PlainTextEdit::~PlainTextEdit()
{
	delete m_textDocument;
	delete m_textCursor;
}
void PlainTextEdit::onBlockCountChanged()
{
	QSize areaSize = viewport()->size();
	//QSize  widgetSize = widget->size();

	verticalScrollBar()->setPageStep(m_textDocument->blockCount() * LINE_HEIGHT);
	verticalScrollBar()->setSingleStep(LINE_HEIGHT);
	//horizontalScrollBar()->setPageStep(widgetSize.width());
	verticalScrollBar()->setRange(0, m_textDocument->blockCount() * LINE_HEIGHT - areaSize.height());
	//horizontalScrollBar()->setRange(0, widgetSize.width() - areaSize.width());
	//updateWidgetPosition();
}

void PlainTextEdit::paintEvent(QPaintEvent * event)
{
	//qDebug() << verticalScrollBar()->value();

	QWidget *vp = viewport();
	QRect vr = vp->rect();
	QPainter painter(vp);

	int y = 0;
	TextBlock block = m_textDocument->findBlockByNumber(verticalScrollBar()->value() / LINE_HEIGHT);
	//TextBlock block = m_textDocument->firstBlock();
	while( y < vr.height() && block.isValid() ) {
		if( m_textCursor->block() == block) {		//	カーソルがブロック内にある場合
			TextCursor cur(m_textDocument);
			cur.setPosition(block.position());
			cur.setPosition(m_textCursor->position(), TextCursor::KeepAnchor);
			const QString text = cur.selectedText();
			QRect t = painter.fontMetrics().boundingRect(text);
			painter.fillRect(QRect(t.width() + MARGIN_LEFT, y+2, 2, 14), Qt::red);
		}
		const QString text = block.text();
		painter.drawText(MARGIN_LEFT, y + 16, text);
		block = block.next();
		y += LINE_HEIGHT;
	}
}
void PlainTextEdit::keyPressEvent ( QKeyEvent * keyEvent )
{
	switch( keyEvent->key() ) {
	case Qt::Key_Right:
		m_textCursor->movePosition(TextCursor::Right);
		viewport()->update();
		return;
	case Qt::Key_Left:
		m_textCursor->movePosition(TextCursor::Left);
		viewport()->update();
		return;
	case Qt::Key_Backspace:
		m_textCursor->deletePreviousChar();
		viewport()->update();
		return;
	case Qt::Key_Delete:
		m_textCursor->deleteChar();
		viewport()->update();
		return;
	}
	QString text = keyEvent->text();
	if( !text.isEmpty() ) {
		m_textCursor->insertText(text);
		viewport()->update();
	}
}
void PlainTextEdit::paste()
{
	QClipboard *clipboard = QApplication::clipboard();
	QString text = clipboard->text();
	if( !text.isEmpty() ) {
		m_textCursor->insertText(text);
		viewport()->update();
	}
}
void PlainTextEdit::undo()
{
	index_t pos = 0;
	m_textDocument->doUndo(pos);
	m_textCursor->setPosition(pos);
	viewport()->update();
}
void PlainTextEdit::redo()
{
	index_t pos = 0;
	m_textDocument->doRedo(pos);
	m_textCursor->setPosition(pos);
	viewport()->update();
}
