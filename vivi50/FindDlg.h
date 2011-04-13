#ifndef FINDDLG_H
#define FINDDLG_H

#include <QDialog>

class QLabel;
class QLineEdit;

class FindDlg : public QDialog
{
	Q_OBJECT

public:
	FindDlg(QWidget *parent = 0);
	~FindDlg();

protected slots:
	void	findNextPushed();

signals:
	void	doFindNext(const QString &);

private:
	QLineEdit	*m_findStringEdit;
};

#endif // FINDDLG_H
