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
//#define		LINE_HEIGHT		20

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
	QFontMetrics fm = fontMetrics();
	QSize areaSize = viewport()->size();
	//QSize  widgetSize = widget->size();

	verticalScrollBar()->setPageStep(m_textDocument->blockCount() * fm.lineSpacing());
	verticalScrollBar()->setSingleStep(fm.lineSpacing());
	//horizontalScrollBar()->setPageStep(widgetSize.width());
	verticalScrollBar()->setRange(0, m_textDocument->blockCount() * fm.lineSpacing() - areaSize.height());
	//horizontalScrollBar()->setRange(0, widgetSize.width() - areaSize.width());
	//updateWidgetPosition();
}

void PlainTextEdit::paintEvent(QPaintEvent * event)
{
	//qDebug() << verticalScrollBar()->value();

	QWidget *vp = viewport();
	QRect vr = vp->rect();
	QPainter painter(vp);

	QFontMetrics fm = fontMetrics();
	const int spaceWidth = fm.width(QChar(' '));
	const int tabWidth = spaceWidth * 4;		//	とりあえず空白4文字分に固定

	int y = 0;
	TextBlock block = m_textDocument->findBlockByNumber(verticalScrollBar()->value() / fm.lineSpacing());
	//TextBlock block = m_textDocument->firstBlock();
	while( y < vr.height() && block.isValid() ) {
		if( m_textCursor->block() == block) {		//	カーソルがブロック内にある場合
			//TextCursor cur(m_textDocument);
			//cur.setPosition(block.position());
			//cur.setPosition(m_textCursor->position(), TextCursor::KeepAnchor);
			//const QString text = cur.selectedText();
			//QRect t = fm.boundingRect(text);
			//painter.fillRect(QRect(t.width() + MARGIN_LEFT, y+2, 2, 14), Qt::red);
			//	undone R カーソル位置座標計算はどこかにまとめる
			const QString text = block.text();
			const index_t offset = qMin(m_textCursor->position() - block.position(),
										(index_t)text.length());
			int x = 0;
			for(int ix = 0; ix < offset; ) {
				if( text[ix] == '\t' ) {
					++ix;
					x = (x / tabWidth + 1) * tabWidth;
				} else {
					int first = ix;
					while( ix < offset && text[ix] != '\t' )
						++ix;
					const QString buf = text.mid(first, ix - first);
					x += fm.boundingRect(buf).width();
				}
			}
			painter.fillRect(QRect(x + MARGIN_LEFT, y+2, 2, fm.height()), Qt::red);
		}
		const QString text = block.text();
		//painter.drawText(MARGIN_LEFT, y + fm.ascent(), text);
		int x = 0;
		for(int ix = 0; ix < text.length(); ) {
			if( text[ix] == '\t' ) {
				++ix;
				x = (x / tabWidth + 1) * tabWidth;
			} else {
				int first = ix;
				while( ix < text.length() && text[ix] != '\t' )
					++ix;
				const QString buf = text.mid(first, ix - first);
				painter.drawText(x + MARGIN_LEFT, y + fm.ascent(), buf);
				x += fm.boundingRect(buf).width();
			}
		}
		block = block.next();
		y += fm.lineSpacing();
	}
}
TextBlock PlainTextEdit::firstVisibleBlock()
{
	QFontMetrics fm = fontMetrics();
	return m_textDocument->findBlockByNumber(verticalScrollBar()->value() / fm.lineSpacing());
}
void PlainTextEdit::ensureCursorVisible()
{
	QFontMetrics fm = fontMetrics();
	TextBlock fvBlock = firstVisibleBlock();
	TextBlock curBlock = m_textCursor->block();
	if( curBlock.blockNumber() < fvBlock.blockNumber() ) {
		verticalScrollBar()->setValue(fm.lineSpacing() * curBlock.blockNumber());
		viewport()->update();
		return;
	}
}
void PlainTextEdit::keyPressEvent ( QKeyEvent * keyEvent )
{
	switch( keyEvent->key() ) {
	case Qt::Key_Right:
		m_textCursor->movePosition(TextCursor::Right);
		ensureCursorVisible();
		viewport()->update();
		return;
	case Qt::Key_Left:
		m_textCursor->movePosition(TextCursor::Left);
		ensureCursorVisible();
		viewport()->update();
		return;
	case Qt::Key_Backspace:
		m_textCursor->deletePreviousChar();
		ensureCursorVisible();
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
