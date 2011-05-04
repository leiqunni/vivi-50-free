#ifndef LAYOUTTHREAD_H
#define LAYOUTTHREAD_H

#include <QThread>
#include <vector>
//#include "textBlock.h"

class TextDocument;

class LayoutThread : public QThread
{
	Q_OBJECT

public:
	LayoutThread(QObject *parent = 0);
	~LayoutThread();

public:
	void	run();

private:
	bool		m_toTerminate;		//	�������f�t���O
	//DocBlock	m_startDocBlock;	//	���C�A�E�g�J�n�ʒu
	//TextDocument	*m_document;
	std::vector<size_t>		m_vSize;	//	���C�A�E�g���ꂽ�s��
};

#endif // LAYOUTTHREAD_H
