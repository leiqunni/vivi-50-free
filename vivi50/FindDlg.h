#ifndef FINDDLG_H
#define FINDDLG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QComboBox;
class QGroupBox;
class QRadioButton;

enum {
	IgnoreCase = 0,
	MatchCase = 0x01,
	IgnoreLowerCase = 0x02,		//	¬•¶š‚Ì‚İŒŸõF‘å•¶š¬•¶š“¯ˆê‹
	FindBackWard = 0x04,
};

void addFindStringHist(/*ushort,*/ const QString &);
void getLastFindString(/*ushort &,*/ QString&);

class FindDlg : public QDialog
{
	Q_OBJECT
public:
	enum {
	};
public:
	FindDlg(QWidget *parent = 0, ushort = IgnoreCase);
	~FindDlg();

protected slots:
	void	onFindNext();
	void	onFindClose();

signals:
	void	doFindNext(const QString &, ushort);

private:
	//QLineEdit	*m_findStringEdit;
	QComboBox	*m_findStringCB;
	QComboBox	*m_caseComboBox;
	QGroupBox	*m_dirGroup;
	QRadioButton	*m_findBackWard;
};

#endif // FINDDLG_H
