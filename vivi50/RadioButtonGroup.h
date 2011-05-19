#ifndef RADIOBUTTONGROUP_H
#define RADIOBUTTONGROUP_H

#include <QGroupBox>

class QBoxLayout;
class QRadioButton;

class RadioButtonGroup : public QGroupBox
{
	Q_OBJECT

public:
	RadioButtonGroup(const QString &title = QString(),
						Qt::Orientation = Qt::Vertical,
						QWidget *parent = 0);
	~RadioButtonGroup();

public:
	int		selectedIndex() const;

public:
	void	addRadioButton(const QString &);
	void	setSelectedIndex(int);
	
private:
	QBoxLayout	*m_boxLayout;
	QList<QRadioButton *>	m_buttonList;
};

#endif // RADIOBUTTONGROUP_H
