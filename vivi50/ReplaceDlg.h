//----------------------------------------------------------------------
//
//			File:			"ReplaceDlg.h"
//			Created:		17-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	ReplaceDlg �N���X�錾
//
//----------------------------------------------------------------------

/*

	Copyright (C) 2011 by Nobuhide Tsuda

	�{�\�[�X�R�[�h�͊�{�I�� MIT ���C�Z���X�ɏ]���B
	http://www.opensource.org/licenses/mit-license.php
	http://sourceforge.jp/projects/opensource/wiki/licenses%2FMIT_license

	�������A������͕s���R�Ŏg������̈��� GPL ���匙���Ȃ̂ŁA
	GPL ���C�Z���X�v���W�F�N�g���{�\�[�X�𗬗p���邱�Ƃ��ւ���

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
