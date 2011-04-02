#include <QtGui>
#include "PlainTextEdit.h"

PlainTextEdit::PlainTextEdit(QWidget *parent)
	: QAbstractScrollArea(parent)
{

}

PlainTextEdit::~PlainTextEdit()
{

}
void PlainTextEdit::paintEvent(QPaintEvent * event)
{
	QWidget *vp = viewport();
	QRect r = vp->rect();
	QPainter painter(vp);
	painter.drawLine(0, 0, r.width(), r.height());
	painter.drawLine(0, r.height(), r.width(), 0);
}
