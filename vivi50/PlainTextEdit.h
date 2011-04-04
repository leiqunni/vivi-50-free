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

public slots:
	void	paste();
	void	undo();
	void	redo();

protected:
	void	paintEvent(QPaintEvent * event);
    void	keyPressEvent ( QKeyEvent * keyEvent );
    //void	resizeEvent ( QResizeEvent * event );

protected slots:
	void	onBlockCountChanged();

private:
	TextDocument	*m_textDocument;
	TextCursor		*m_textCursor;		//	�b��I�A�{���̓r���[�p�J�[�\���𗘗p����
};

#endif // PLAINTEXTEDIT_H
