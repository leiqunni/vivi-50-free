//----------------------------------------------------------------------
//
//			File:			"FindDlg.h"
//			Created:		01-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	FindDlg クラス宣言
//
//----------------------------------------------------------------------

/*

	Copyright (C) 2011 by Nobuhide Tsuda

	本ソースコードは基本的に MIT ライセンスに従う。
	http://www.opensource.org/licenses/mit-license.php
	http://sourceforge.jp/projects/opensource/wiki/licenses%2FMIT_license

	ただし、おいらは不自由で使い勝手の悪い GPL が大嫌いなので、
	GPL ライセンスプロジェクトが本ソースを流用することを禁じる

*/

#ifndef FINDDLG_H
#define FINDDLG_H

#include <QDialog>
#include "findOpt.h"

class QLabel;
class QLineEdit;
class QComboBox;
class QGroupBox;
class QRadioButton;
class RadioButtonGroup;
class QCheckBox;

void getStringFromHist(const QString &key, QString &text);
void addStringToHist(const QString &key, const QString &text);
void addFindStringHist(/*ushort,*/ const QString &);
void getLastFindString(/*ushort &,*/ QString&);

class FindDlg : public QDialog
{
	Q_OBJECT
public:
	enum {
	};
public:
	FindDlg(const QString &, QWidget *parent = 0, ushort = IgnoreCase);
	~FindDlg();

protected slots:
	void	onFindNext();
	void	onFindPrev();
	void	onFindClose();

signals:
	void	doFindNext(const QString &, ushort);

private:
	//QLineEdit	*m_findStringEdit;
	QComboBox	*m_findStringCB;
	QComboBox	*m_caseComboBox;
	RadioButtonGroup	*m_caseGroup;
	//QGroupBox	*m_caseGroup;
	QCheckBox	*m_regexp;
	QGroupBox	*m_dirGroup;
	QRadioButton	*m_findBackWard;
};

#endif // FINDDLG_H
