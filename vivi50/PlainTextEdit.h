#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H

#include <QScrollArea>

class PlainTextEdit : public QScrollArea
{
	Q_OBJECT

public:
	PlainTextEdit(QWidget *parent = 0);
	~PlainTextEdit();

private:
	
};

#endif // PLAINTEXTEDIT_H
