#include <QtGui>
#include "mainwindow.h"
#include "PlainTextEdit.h"

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	init();
}

MainWindow::~MainWindow()
{

}

void MainWindow::init()
{
	m_editor = new PlainTextEdit;
	setCentralWidget(m_editor);

	createActions();
	createMenus();
	createToolBars();
	createDockWindows();
    readSettings();			//	createActions() ‚ÌŒã‚ÉƒR[ƒ‹‚·‚é‚±‚Æ
}
void MainWindow::createActions()
{
#if 0
    unitTestAct = new QAction(tr("&Unit Test"), this);
    unitTestAct->setStatusTip(tr("exec Unit Tests"));
    connect(unitTestAct, SIGNAL(triggered()), this, SLOT(doUnitTest()));
    benchmarkAct = new QAction(tr("&Benchmark"), this);
    benchmarkAct->setStatusTip(tr("benchmark Tests"));
    connect(benchmarkAct, SIGNAL(triggered()), this, SLOT(doBenchmark()));
#endif
}
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    editMenu = menuBar()->addMenu(tr("&Edit"));
    viewMenu = menuBar()->addMenu(tr("&View"));
}
void MainWindow::createToolBars()
{
    QToolBar *testToolBar = addToolBar(tr("Edit"));
    //testToolBar->addAction(unitTestAct);
    //testToolBar->addAction(benchmarkAct);
}
void MainWindow::readSettings()
{
    QSettings settings;
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}
void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}
void MainWindow::closeEvent(QCloseEvent *event)
{
#if 1
    writeSettings();
#else
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
#endif
}
void MainWindow::doOutput(const QString &text)
{
	QTextCursor cur = output->textCursor();
	cur.movePosition(QTextCursor::End);
	cur.insertText(text);
	output->setTextCursor(cur);
	output->viewport()->repaint();		//	‹­§Ä•`‰æ
}
void MainWindow::createDockWindows()
{
    QDockWidget *dock = new QDockWidget(tr("Output"), this);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    output = new QTextEdit(dock);
    output->setReadOnly(true);
    dock->setWidget(output);
    addDockWidget(Qt::BottomDockWidgetArea, dock);
    viewMenu->addAction(dock->toggleViewAction());
}
