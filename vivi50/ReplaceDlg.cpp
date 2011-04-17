//----------------------------------------------------------------------
//
//			File:			"ReplaceDlg.cpp"
//			Created:		17-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	ReplaceDlg クラス実装
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

#include <QtGui>
#include "ReplaceDlg.h"

ReplaceDlg::ReplaceDlg(QWidget *parent, ushort matchCase)
	: QDialog(parent)
{
	setWindowTitle(tr("Replace Dialog"));

	QHBoxLayout *hBoxLayout = new QHBoxLayout();
		QLabel *findStringLabel = new QLabel(tr("Find &String:"));
		hBoxLayout->addWidget(findStringLabel);
		(m_findStringCB = new QComboBox)->setEditable(true);
	    QSettings settings;
	    QStringList hist = settings.value("findStringHist").toStringList();
	    for(int ix = hist.size(); ix != 0; )
	    	m_findStringCB->addItem(hist[--ix]);
		hBoxLayout->addWidget(m_findStringCB);
	m_caseComboBox = new QComboBox();
		m_caseComboBox->addItem(tr("Ignore Case"));
		m_caseComboBox->addItem(tr("Match Case"));
		m_caseComboBox->setCurrentIndex(matchCase);
	m_dirGroup = new QGroupBox(tr("direction"));
	{
		QHBoxLayout *hBoxLayout = new QHBoxLayout();
		QRadioButton *ptr;
		hBoxLayout->addWidget(ptr = new QRadioButton(tr("Forward")));
		hBoxLayout->addWidget(m_findBackWard = new QRadioButton(tr("Backward")));
		ptr->setChecked(true);
		m_dirGroup->setLayout(hBoxLayout);
	}
	QVBoxLayout *vBoxLayoutLeft = new QVBoxLayout();
		vBoxLayoutLeft->addLayout(hBoxLayout);
		vBoxLayoutLeft->addWidget(m_caseComboBox);
		vBoxLayoutLeft->addWidget(m_dirGroup);
		vBoxLayoutLeft->addStretch();
	QVBoxLayout *vBoxLayoutRight = new QVBoxLayout();	//	ボタン配置用
		QPushButton *findNext = new QPushButton(tr("Find&Next"));
		connect(findNext, SIGNAL(clicked()), this, SLOT(onFindNext()));
		findNext->setDefault(true);
		vBoxLayoutRight->addWidget(findNext);
		QPushButton *findClose = new QPushButton(tr("&FindClose"));
		connect(findClose, SIGNAL(clicked()), this, SLOT(onFindClose()));
		vBoxLayoutRight->addWidget(findClose);
		QPushButton *cancel = new QPushButton(tr("Cancel"));
		connect(cancel, SIGNAL(clicked()), this, SLOT(close()));
		vBoxLayoutRight->addWidget(cancel);
		vBoxLayoutRight->addStretch();

	QHBoxLayout *hBoxLayout0 = new QHBoxLayout();
		hBoxLayout0->addLayout(vBoxLayoutLeft);
		hBoxLayout0->addLayout(vBoxLayoutRight);

	setLayout(hBoxLayout0);
}

ReplaceDlg::~ReplaceDlg()
{

}
