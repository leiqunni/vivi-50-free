#ifndef FINDDLG_H
#define FINDDLG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QComboBox;

class FindDlg : public QDialog
{
	Q_OBJECT
public:
	enum {
		IgnoreCase = 0,
		MatchCase,
	};
public:
	FindDlg(QWidget *parent = 0, uchar = IgnoreCase);
	~FindDlg();

protected slots:
	void	findNextPushed();

signals:
	void	doFindNext(const QString &, uchar);

private:
	QLineEdit	*m_findStringEdit;
	QComboBox	*m_caseComboBox;
};

#endif // FINDDLG_H
