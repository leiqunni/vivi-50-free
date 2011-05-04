#ifndef LAYOUTTHREAD_H
#define LAYOUTTHREAD_H

#include <QThread>

class LayoutThread : public QThread
{
	Q_OBJECT

public:
	LayoutThread(QObject *parent);
	~LayoutThread();

private:
	
};

#endif // LAYOUTTHREAD_H
