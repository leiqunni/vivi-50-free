//----------------------------------------------------------------------
//
//			File:			"FindDlg.h"
//			Created:		01-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	FindDlg �N���X�錾
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
