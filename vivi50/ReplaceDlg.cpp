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

    QSettings settings;
	QVBoxLayout *vBoxLayoutLeft = new QVBoxLayout();
	{
		QHBoxLayout *hBoxLayout = new QHBoxLayout();
			hBoxLayout->addWidget(new QLabel(tr("Find &String:")));
			(m_findStringCB = new QComboBox)->setEditable(true);
		    QStringList hist = settings.value("findStringHist").toStringList();
		    for(int ix = hist.size(); ix != 0; )
		    	m_findStringCB->addItem(hist[--ix]);
			hBoxLayout->addWidget(m_findStringCB);
			vBoxLayoutLeft->addLayout(hBoxLayout);
		QHBoxLayout *hBoxLayout2 = new QHBoxLayout();
			hBoxLayout2->addWidget(new QLabel(tr("Replace &To:")));
			(m_replaceStringCB = new QComboBox)->setEditable(true);
		    QStringList hist2 = settings.value("replaceStringHist").toStringList();
		    for(int ix = hist2.size(); ix != 0; )
		    	m_replaceStringCB->addItem(hist2[--ix]);
			hBoxLayout2->addWidget(m_replaceStringCB);
			vBoxLayoutLeft->addLayout(hBoxLayout2);
		m_caseComboBox = new QComboBox();
			m_caseComboBox->addItem(tr("Ignore Case"));
			m_caseComboBox->addItem(tr("Match Case"));
			m_caseComboBox->setCurrentIndex(matchCase);
			vBoxLayoutLeft->addWidget(m_caseComboBox);
		m_dirGroup = new QGroupBox(tr("direction"));
		{
			QHBoxLayout *hBoxLayout = new QHBoxLayout();
			QRadioButton *ptr;
			hBoxLayout->addWidget(ptr = new QRadioButton(tr("Forward")));
			hBoxLayout->addWidget(m_findBackWard = new QRadioButton(tr("Backward")));
			ptr->setChecked(true);
			m_dirGroup->setLayout(hBoxLayout);
			vBoxLayoutLeft->addWidget(m_dirGroup);
		}
		vBoxLayoutLeft->addStretch();
	}
	QVBoxLayout *vBoxLayoutRight = new QVBoxLayout();	//	ボタン配置用
		QPushButton *findPrev = new QPushButton(tr("Find&Prev"));
			connect(findPrev, SIGNAL(clicked()), this, SLOT(onFindPrev()));
			vBoxLayoutRight->addWidget(findPrev);
		QPushButton *findNext = new QPushButton(tr("Find&Next"));
			connect(findNext, SIGNAL(clicked()), this, SLOT(onFindNext()));
			findNext->setDefault(true);
			vBoxLayoutRight->addWidget(findNext);
		QPushButton *findReplace = new QPushButton(tr("&FindReplace"));
			connect(findReplace, SIGNAL(clicked()), this, SLOT(onFindReplace()));
			vBoxLayoutRight->addWidget(findReplace);
		QPushButton *replaceAll = new QPushButton(tr("replace&All"));
			connect(replaceAll, SIGNAL(clicked()), this, SLOT(onReplaceAll()));
			vBoxLayoutRight->addWidget(replaceAll);
		QPushButton *undo = new QPushButton(tr("&Undo"));
			connect(undo, SIGNAL(clicked()), this, SLOT(onUndo()));
			vBoxLayoutRight->addWidget(undo);
		QPushButton *redo = new QPushButton(tr("Redo"));
			connect(redo, SIGNAL(clicked()), this, SLOT(onRedo()));
			vBoxLayoutRight->addWidget(redo);
		QPushButton *cancel = new QPushButton(tr("Close"));
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
void ReplaceDlg::onFindNext()
{
	const QString findString = m_findStringCB->currentText();
	//const QString findString = m_findStringEdit->text();
	if( !findString.isEmpty() ) {
		ushort options = 0;
		if( m_caseComboBox->currentIndex() == 1 )
			options |= MatchCase;
		if( m_findBackWard->isChecked() != 0 )
			options |= FindBackWard;
		emit doFindNext(findString, options);
		addFindStringHist(findString);
	}
}
