#ifndef FINDDLG_H
#define FINDDLG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QComboBox;
class QGroupBox;
class QRadioButton;

class FindDlg : public QDialog
{
	Q_OBJECT
public:
	enum {
		IgnoreCase = 0,
		MatchCase = 0x01,
		IgnoreLowerCase = 0x02,		//	¬•¶š‚Ì‚İŒŸõF‘å•¶š¬•¶š“¯ˆê‹
		FindBackWard = 0x04,
	};
	enum {
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
	QGroupBox	*m_dirGroup;
	QRadioButton	*m_findBackWard;
};

#endif // FINDDLG_H
