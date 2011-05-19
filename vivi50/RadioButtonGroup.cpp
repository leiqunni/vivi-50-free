#include "RadioButtonGroup.h"
#include	<QtGui>

RadioButtonGroup::RadioButtonGroup(const QString &title,
									Qt::Orientation ori,
									QWidget *parent)
	: QGroupBox(title, parent)
{
	if( ori == Qt::Horizontal )
		m_boxLayout = new QHBoxLayout();
	else
		m_boxLayout = new QVBoxLayout();
	setLayout(m_boxLayout);
}

RadioButtonGroup::~RadioButtonGroup()
{
}

void RadioButtonGroup::addRadioButton(const QString &text)
{
	QRadioButton *btn = new QRadioButton(text);
	m_boxLayout->addWidget(btn);
	m_buttonList.push_back(btn);
}

int RadioButtonGroup::selectedIndex() const
{
	int ix = 0;
	QRadioButton *btn;
	foreach(btn, m_buttonList) {
		if( btn->isChecked() ) return ix;
		++ix;
	}
	return -1;		//	no selected
}

void RadioButtonGroup::setSelectedIndex(int ix)
{
	m_buttonList[ix]->setChecked(true);
}
