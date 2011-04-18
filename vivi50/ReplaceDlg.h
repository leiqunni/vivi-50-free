//----------------------------------------------------------------------
//
//			File:			"ReplaceDlg.h"
//			Created:		17-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	ReplaceDlg クラス宣言
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

#ifndef REPLACEDLG_H
#define REPLACEDLG_H

#include <QDialog>
#include "findOpt.h"
#include "findDlg.h"

class QLabel;
class QLineEdit;
class QComboBox;
class QGroupBox;
class QRadioButton;

class ReplaceDlg : public QDialog
{
	Q_OBJECT

public:
	ReplaceDlg(QWidget *parent = 0, ushort = IgnoreCase);
	~ReplaceDlg();

protected:
	void	doFind(bool backward = false);

protected slots:
	void	onFindNext();
	void	onFindPrev();
	void	onReplaceFind();

signals:
	void	doFindNext(const QString &, ushort);
	void	isMatched(bool &, const QString &, ushort);
	void	doReplace(const QString &);

private:
	QComboBox	*m_findStringCB;
	QComboBox	*m_replaceStringCB;
	QComboBox	*m_caseComboBox;
	//QGroupBox	*m_dirGroup;
	QRadioButton	*m_findBackWard;
	
};

#endif // REPLACEDLG_H
