#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H

#include <QAbstractScrollArea>

class TextDocument;
class TextCursor;

class PlainTextEdit : public QAbstractScrollArea
{
	Q_OBJECT

public:
	PlainTextEdit(QWidget *parent = 0);
	~PlainTextEdit();

protected:
	void	paintEvent(QPaintEvent * event);
    void	keyPressEvent ( QKeyEvent * keyEvent );

private:
	TextDocument	*m_textDocument;
	TextCursor		*m_textCursor;		//	�b��I�A�{���̓r���[�p�J�[�\���𗘗p����
};

#endif // PLAINTEXTEDIT_H
