#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H

#include <QAbstractScrollArea>

class TextDocument;

class PlainTextEdit : public QAbstractScrollArea
{
	Q_OBJECT

public:
	PlainTextEdit(QWidget *parent = 0);
	~PlainTextEdit();

protected:
	void	paintEvent(QPaintEvent * event);

private:
	TextDocument	*m_textDocument;
};

#endif // PLAINTEXTEDIT_H
