#include <QtGui>
#include "FindDlg.h"

FindDlg::FindDlg(QWidget *parent, uchar matchCase)
	: QDialog(parent)
{
	setWindowTitle(tr("Find Dialog"));

	QLabel *findStringLabel = new QLabel(tr("Find &String:"));
	findStringLabel->setBuddy(m_findStringEdit = new QLineEdit);
	QHBoxLayout *hBoxLayout = new QHBoxLayout();
		hBoxLayout->addWidget(findStringLabel);
		hBoxLayout->addWidget(m_findStringEdit);

	m_caseComboBox = new QComboBox();
		m_caseComboBox->addItem(tr("Ignore Case"));
		m_caseComboBox->addItem(tr("Match Case"));
		m_caseComboBox->setCurrentIndex(matchCase);
	QVBoxLayout *vBoxLayout = new QVBoxLayout();
		vBoxLayout->addLayout(hBoxLayout);
		vBoxLayout->addWidget(m_caseComboBox);
		vBoxLayout->addStretch();

	QVBoxLayout *vBoxLayout2 = new QVBoxLayout();	//	ボタン配置用
	QPushButton *findNext = new QPushButton(tr("Find&Next"));
	connect(findNext, SIGNAL(clicked()), this, SLOT(findNextPushed()));
	findNext->setDefault(true);
	vBoxLayout2->addWidget(findNext);
	QPushButton *findClose = new QPushButton(tr("&FindClose"));
	vBoxLayout2->addWidget(findClose);
	QPushButton *cancel = new QPushButton(tr("Cancel"));
	vBoxLayout2->addWidget(cancel);

	QHBoxLayout *hBoxLayout0 = new QHBoxLayout();
	hBoxLayout0->addLayout(vBoxLayout);
	hBoxLayout0->addLayout(vBoxLayout2);

	setLayout(hBoxLayout0);

}

FindDlg::~FindDlg()
{

}
void FindDlg::findNextPushed()
{
	const QString findString = m_findStringEdit->text();
	if( !findString.isEmpty() )
		emit doFindNext(findString, m_caseComboBox->currentIndex());

}
