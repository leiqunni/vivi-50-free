//----------------------------------------------------------------------
//
//			File:			"FindDlg.cpp"
//			Created:		01-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	FindDlg クラス実装
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
#include "FindDlg.h"

#define		HIST_LIMIT		30

void getStringFromHist(const QString &key, QString &text)
{
    QSettings settings;
    QStringList hist = settings.value(key).toStringList();
	if( hist.isEmpty() )
		text = QString();
	else {
		QStringList::const_iterator itr = hist.end() - 1;
		text = *itr;	//	最後の要素が最新検索文字列
	}
}
void addStringToHist(const QString &key, const QString &text)
{
    QSettings settings;
    QStringList hist = settings.value(key).toStringList();
	if( !hist.isEmpty() ) {
		int ix = hist.indexOf(text);
		if( ix >= 0 )
			hist.erase(hist.begin() + ix);		//	重複削除
	}
	hist.push_back(text);
	if( hist.size() > HIST_LIMIT )
		hist.erase(hist.begin());		//	要素数がHIST_LIMITを超えた場合は古いものを削除
	settings.setValue(key, hist);
}
void getLastFindString(/*ushort &opt,*/ QString &text)
{
	getStringFromHist("findStringHist", text);
}
void addFindStringHist(/*ushort opt,*/ const QString &text)
{
	addStringToHist("findStringHist", text);
}

FindDlg::FindDlg(QWidget *parent, ushort matchCase)
	: QDialog(parent)
{
	setWindowTitle(tr("Find Dialog"));

	QHBoxLayout *hBoxLayout = new QHBoxLayout();
		QLabel *findStringLabel = new QLabel(tr("FindString:"));
		//findStringLabel->setBuddy(m_findStringEdit = new QLineEdit);
		hBoxLayout->addWidget(findStringLabel);
		//hBoxLayout->addWidget(m_findStringEdit);
		(m_findStringCB = new QComboBox)->setEditable(true);
		QFontMetrics fm = m_findStringCB->fontMetrics();
		m_findStringCB->setMinimumWidth(fm.width('8')*32);
		m_findStringCB->setSizePolicy( QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum) );
	    QSettings settings;
	    QStringList hist = settings.value("findStringHist").toStringList();
	    for(int ix = hist.size(); ix != 0; )
	    	m_findStringCB->addItem(hist[--ix]);
		m_findStringCB->lineEdit()->selectAll();
		hBoxLayout->addWidget(m_findStringCB);
	QHBoxLayout *hBoxLayout2 = new QHBoxLayout();
		QVBoxLayout *vBoxLayout2 = new QVBoxLayout();
		m_caseComboBox = new QComboBox();
			m_caseComboBox->addItem(tr("Ignore Case"));
			m_caseComboBox->addItem(tr("Match Case"));
			m_caseComboBox->setCurrentIndex(matchCase);
			m_caseComboBox->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum) );
			vBoxLayout2->addWidget(m_caseComboBox);
		m_dirGroup = new QGroupBox(tr("direction"));
			QHBoxLayout *hBoxLayout3 = new QHBoxLayout();
			QRadioButton *ptr;
			hBoxLayout3->addWidget(ptr = new QRadioButton(tr("Forward")));
			hBoxLayout3->addWidget(m_findBackWard = new QRadioButton(tr("Backward")));
			ptr->setChecked(true);
			m_dirGroup->setLayout(hBoxLayout3);
			vBoxLayout2->addWidget(m_dirGroup);
		hBoxLayout2->addLayout(vBoxLayout2);
		hBoxLayout2->addStretch();
	QVBoxLayout *vBoxLayoutLeft = new QVBoxLayout();
		vBoxLayoutLeft->addLayout(hBoxLayout);
		vBoxLayoutLeft->addLayout(hBoxLayout2);
		vBoxLayoutLeft->addStretch();

	QVBoxLayout *vBoxLayoutRight = new QVBoxLayout();	//	ボタン配置用
		QPushButton *findNext = new QPushButton(tr("FindNext"));
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

FindDlg::~FindDlg()
{

}
void FindDlg::onFindClose()
{
	onFindNext();
	close();
}
void FindDlg::onFindNext()
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
