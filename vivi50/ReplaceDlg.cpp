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
#include "RadioButtonGroup.h"

bool isLowerText(const QString &text);

ReplaceDlg::ReplaceDlg(QWidget *parent, ushort matchCase)
	: QDialog(parent)
{
	setWindowTitle(tr("Replace Dialog"));
    QSettings settings;
	QVBoxLayout *vBoxLayoutLeft = new QVBoxLayout();
	{
		QHBoxLayout *hBoxLayout = new QHBoxLayout();
			hBoxLayout->addWidget(new QLabel(tr("Find String:")));
			(m_findStringCB = new QComboBox)->setEditable(true);
			m_findStringCB->setCompleter(0);	//	コンプリータ無し
			QFontMetrics fm = m_findStringCB->fontMetrics();
			m_findStringCB->setMinimumWidth(fm.width('8')*32);
			m_findStringCB->setSizePolicy( QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum) );
		    QStringList hist = settings.value("findStringHist").toStringList();
		    for(int ix = hist.size(); ix != 0; )
		    	m_findStringCB->addItem(hist[--ix]);
			m_findStringCB->lineEdit()->selectAll();
			hBoxLayout->addWidget(m_findStringCB);
			vBoxLayoutLeft->addLayout(hBoxLayout);
		QHBoxLayout *hBoxLayout2 = new QHBoxLayout();
			hBoxLayout2->addWidget(new QLabel(tr("Replace To:")));
			(m_replaceStringCB = new QComboBox)->setEditable(true);
			m_replaceStringCB->setSizePolicy( QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum) );
		    QStringList hist2 = settings.value("replaceStringHist").toStringList();
		    for(int ix = hist2.size(); ix != 0; )
		    	m_replaceStringCB->addItem(hist2[--ix]);
			hBoxLayout2->addWidget(m_replaceStringCB);
			vBoxLayoutLeft->addLayout(hBoxLayout2);
#if 0
		m_caseComboBox = new QComboBox();
			m_caseComboBox->addItem(tr("Ignore Case"));
			m_caseComboBox->addItem(tr("Match Case"));
			m_caseComboBox->setCurrentIndex(matchCase);
			m_caseComboBox->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum) );
			vBoxLayoutLeft->addWidget(m_caseComboBox);
#endif
		m_caseGroup = new RadioButtonGroup(tr("Upper Lower Case"));
			m_caseGroup->addRadioButton(tr("Ignore Case"));
			m_caseGroup->addRadioButton(tr("Ignore Case if Lower Text"));
			m_caseGroup->addRadioButton(tr("Case Sensitive"));
			m_caseGroup->setSelectedIndex(settings.value("findCaseOption", 0).toInt());
			vBoxLayoutLeft->addWidget(m_caseGroup);
#if 0
		m_caseGroup = new QGroupBox(tr("Upper Lower Case"));
		{
			QVBoxLayout *boxLayout = new QVBoxLayout();
			QRadioButton *ptr;
			boxLayout->addWidget(ptr = new QRadioButton(tr("Ignore Case")));
			boxLayout->addWidget(new QRadioButton(tr("Ignore Case if Lower Pat")));
			boxLayout->addWidget(new QRadioButton(tr("Case Sensitive")));
			ptr->setChecked(true);
			m_caseGroup->setLayout(boxLayout);
			m_caseGroup->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum) );
			vBoxLayoutLeft->addWidget(m_caseGroup);
		}
#endif
#if 0
		m_dirGroup = new QGroupBox(tr("direction"));
		{
			QHBoxLayout *hBoxLayout = new QHBoxLayout();
			QRadioButton *ptr;
			hBoxLayout->addWidget(ptr = new QRadioButton(tr("Forward")));
			hBoxLayout->addWidget(m_findBackWard = new QRadioButton(tr("Backward")));
			ptr->setChecked(true);
			m_dirGroup->setLayout(hBoxLayout);
			m_dirGroup->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum) );
			vBoxLayoutLeft->addWidget(m_dirGroup);
		}
#endif
		vBoxLayoutLeft->addStretch();
	}
	QVBoxLayout *vBoxLayoutRight = new QVBoxLayout();	//	ボタン配置用
		QPushButton *findPrev = new QPushButton(tr("FindPrev"));
			connect(findPrev, SIGNAL(clicked()), this, SLOT(onFindPrev()));
			vBoxLayoutRight->addWidget(findPrev);
		QPushButton *findNext = new QPushButton(tr("FindNext"));
			connect(findNext, SIGNAL(clicked()), this, SLOT(onFindNext()));
			findNext->setDefault(true);
			vBoxLayoutRight->addWidget(findNext);
		QPushButton *replaceFind = new QPushButton(tr("&ReplaceFind"));
			connect(replaceFind, SIGNAL(clicked()), this, SLOT(onReplaceFind()));
			vBoxLayoutRight->addWidget(replaceFind);
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
    QSettings settings;
    settings.setValue("findCaseOption", m_caseGroup->selectedIndex());
}
void ReplaceDlg::doFind(bool backward)
{
	const QString findString = m_findStringCB->currentText();
	//const QString findString = m_findStringEdit->text();
	if( findString.isEmpty() ) return;
	ushort options = 0;
	const int ix = m_caseGroup->selectedIndex();
	if( !(!ix || ix == 1 && isLowerText(findString)) )
		options |= MatchCase;
#if 0
	if( m_caseComboBox->currentIndex() == 1 )
		options |= MatchCase;
#endif
	if( backward )
		options |= FindBackWard;
	emit doFindNext(findString, options);
	addFindStringHist(findString);
}
void ReplaceDlg::onFindPrev()
{
	doFind(true);
}
void ReplaceDlg::onFindNext()
{
	doFind();
}
void ReplaceDlg::onReplaceFind()
{
	bool b = false;
	const QString findString = m_findStringCB->currentText();
	ushort options = 0;
	const int ix = m_caseGroup->selectedIndex();
	if( !(!ix || ix == 1 && isLowerText(findString)) )
		options |= MatchCase;
#if 0
	if( m_caseComboBox->currentIndex() == 1 )
		options |= MatchCase;
#endif
	emit isMatched(b, findString, options);
	if( !b ) return;
	emit doReplace(m_replaceStringCB->currentText());
	addStringToHist("replaceStringHist", m_replaceStringCB->currentText());
	doFind();
}
void ReplaceDlg::onReplaceAll()
{
	ushort options = 0;
	const QString findString = m_findStringCB->currentText();
	const int ix = m_caseGroup->selectedIndex();
	if( !(!ix || ix == 1 && isLowerText(findString)) )
		options |= MatchCase;
#if 0
	if( m_caseComboBox->currentIndex() == 1 )
		options |= MatchCase;
#endif
	emit doReplaceAll(findString, options,
						m_replaceStringCB->currentText());
	addStringToHist("replaceStringHist", m_replaceStringCB->currentText());
}
