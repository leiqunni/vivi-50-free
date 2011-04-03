#include <QtGui>
#include "PlainTextEdit.h"
#include	"TextDocument.h"

PlainTextEdit::PlainTextEdit(QWidget *parent)
	: QAbstractScrollArea(parent)
{
	m_textDocument = new TextDocument();

	m_textDocument->setPlainText(QString("LINE-1\nLINE-2\nLINE-3\n"));
}

PlainTextEdit::~PlainTextEdit()
{
	delete m_textDocument;
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
		const QString text = block.text();
		painter.drawText(4, y + 16, text);
		block = block.next();
		y += 20;
	}
}
