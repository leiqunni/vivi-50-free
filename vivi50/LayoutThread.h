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
	bool		m_toTerminate;		//	処理中断フラグ
	//DocBlock	m_startDocBlock;	//	レイアウト開始位置
	//TextDocument	*m_document;
	std::vector<size_t>		m_vSize;	//	レイアウトされた行長
};

#endif // LAYOUTTHREAD_H
