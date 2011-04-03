#include <QtGui>
#include "PlainTextEdit.h"
#include	"TextDocument.h"

#define		MARGIN_LEFT		4

PlainTextEdit::PlainTextEdit(QWidget *parent)
	: QAbstractScrollArea(parent)
{
	m_textDocument = new TextDocument();
	m_textCursor = new TextCursor(m_textDocument);

	m_textDocument->setPlainText(QString("LINE-1\nLINE-2\nLINE-3\n"));
}

PlainTextEdit::~PlainTextEdit()
{
	delete m_textDocument;
	delete m_textCursor;
}
void PlainTextEdit::paintEvent(QPaintEvent * event)
{
	QWidget *vp = viewport();
	QRect r = vp->rect();
	QPainter painter(vp);
#if 0
	painter.drawLine(0, 0, r.width(), r.height());
	painter.drawLine(0, r.height(), r.width(), 0);
#endif

	int y = 0;
	TextBlock block = m_textDocument->firstBlock();
	while( block.isValid() ) {
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
		y += 20;
	}
}
void PlainTextEdit::keyPressEvent ( QKeyEvent * keyEvent )
{
	switch( keyEvent->key() ) {
	case Qt::Key_Right:
		m_textCursor->movePosition(TextCursor::Right);
		viewport()->update();
		break;
	case Qt::Key_Left:
		m_textCursor->movePosition(TextCursor::Left);
		viewport()->update();
		break;
	}
}
