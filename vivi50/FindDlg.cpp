#include <QtGui>
#include "FindDlg.h"

void getLastFindString(/*ushort &opt,*/ QString &text)
{
    QSettings settings;
    QStringList hist = settings.value("findStringHist").toStringList();
	if( hist.isEmpty() )
		text = QString();
	else {
		QStringList::const_iterator itr = hist.end() - 1;
		text = *itr;	//	最後の要素が最新検索文字列
	}
}
void addFindStringHist(/*ushort opt,*/ const QString &text)
{
    QSettings settings;
    QStringList hist = settings.value("findStringHist").toStringList();
	if( !hist.isEmpty() ) {
		int ix = hist.indexOf(text);
		if( ix >= 0 )
			hist.erase(hist.begin() + ix);		//	重複削除
	}
	hist.push_back(text);
	if( hist.size() > 100 )
		hist.erase(hist.begin());		//	要素数が100を超えた場合は古いものを削除
	settings.setValue("findStringHist", hist);
}

FindDlg::FindDlg(QWidget *parent, ushort matchCase)
	: QDialog(parent)
{
	setWindowTitle(tr("Find Dialog"));

	QHBoxLayout *hBoxLayout = new QHBoxLayout();
		QLabel *findStringLabel = new QLabel(tr("Find &String:"));
		findStringLabel->setBuddy(m_findStringEdit = new QLineEdit);
		hBoxLayout->addWidget(findStringLabel);
		hBoxLayout->addWidget(m_findStringEdit);
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
	QVBoxLayout *vBoxLayout = new QVBoxLayout();
		vBoxLayout->addLayout(hBoxLayout);
		vBoxLayout->addWidget(m_caseComboBox);
		vBoxLayout->addWidget(m_dirGroup);
		vBoxLayout->addStretch();

	QVBoxLayout *vBoxLayout2 = new QVBoxLayout();	//	ボタン配置用
		QPushButton *findNext = new QPushButton(tr("Find&Next"));
		connect(findNext, SIGNAL(clicked()), this, SLOT(onFindNext()));
		findNext->setDefault(true);
		vBoxLayout2->addWidget(findNext);
		QPushButton *findClose = new QPushButton(tr("&FindClose"));
		connect(findClose, SIGNAL(clicked()), this, SLOT(onFindClose()));
		vBoxLayout2->addWidget(findClose);
		QPushButton *cancel = new QPushButton(tr("Cancel"));
		connect(cancel, SIGNAL(clicked()), this, SLOT(close()));
		vBoxLayout2->addWidget(cancel);
		vBoxLayout2->addStretch();

	QHBoxLayout *hBoxLayout0 = new QHBoxLayout();
		hBoxLayout0->addLayout(vBoxLayout);
		hBoxLayout0->addLayout(vBoxLayout2);

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
	const QString findString = m_findStringEdit->text();
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
