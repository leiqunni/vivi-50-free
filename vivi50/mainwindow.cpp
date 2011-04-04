#include <QtGui>
#include "mainwindow.h"
#include "PlainTextEdit.h"

#define	VERSION_STR			"5.0.000 Dev"

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
    readSettings();			//	createActions() の後にコールすること

    statusBar()->showMessage("Ready");
}
void MainWindow::createActions()
{
	newAct = new QAction(QIcon(":vivi/Resources/images/new.png"), tr("&New"), this);
	newAct->setIconText(tr("new doc"));
	//newAct->setToolTip(tr("new doc (tool tip)"));
	newAct->setShortcuts(QKeySequence::New);
	newAct->setStatusTip(tr("Create a new document"));
	connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

	openAct = new QAction(QIcon(":vivi/Resources/images/open.png"), tr("&Open..."), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("Open an existing file"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	saveAct = new QAction(QIcon(":vivi/Resources/images/save.png"), tr("&Save"), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("Save the document to disk"));
	connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

	saveAsAct = new QAction(/*QIcon(":vivi/Resources/images/save.png"),*/ tr("Save&As..."), this);
	saveAsAct->setShortcuts(QKeySequence::SaveAs);
	saveAsAct->setStatusTip(tr("Save the document under a new fileName"));
	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

	closeAllAct = new QAction(tr("C&loseAll"), this);
	//closeAllAct->setShortcuts(QKeySequence::SaveAs);
	closeAllAct->setStatusTip(tr("Close all documents"));
	connect(closeAllAct, SIGNAL(triggered()), this, SLOT(closeAllViews()));

#if 0
    //	RecentFilesMenu のための初期化
    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }
#endif

    cutAct = new QAction(QIcon(":vivi/Resources/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
    connect(cutAct, SIGNAL(triggered()), m_editor, SLOT(cut()));

    copyAct = new QAction(QIcon(":vivi/Resources/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
    connect(copyAct, SIGNAL(triggered()), m_editor, SLOT(copy()));

    pasteAct = new QAction(QIcon(":vivi/Resources/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
    connect(pasteAct, SIGNAL(triggered()), m_editor, SLOT(paste()));

	undoAct = new QAction(QIcon(":vivi/Resources/images/editundo.png"), tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("undo edit commande"));
	undoAct->setEnabled(false);
    connect(undoAct, SIGNAL(triggered()), m_editor, SLOT(undo()));
    connect(m_editor, SIGNAL(undoAvailable(bool)), undoAct, SLOT(setEnabled(bool)));

	redoAct = new QAction(QIcon(":vivi/Resources/images/editredo.png"), tr("&Redo"), this);
    redoAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    //redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("redo edit commande"));
	redoAct->setEnabled(false);
    connect(redoAct, SIGNAL(triggered()), m_editor, SLOT(redo()));
    connect(m_editor, SIGNAL(redoAvailable(bool)), redoAct, SLOT(setEnabled(bool)));

    aboutAct = new QAction(QIcon(":vivi/Resources/images/Info.png"), tr("&About ViVi"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(showAboutDlg()));

    unitTestAct = new QAction(tr("&UnitTest"), this);
    unitTestAct->setStatusTip(tr("exec Unit Tests"));
    connect(unitTestAct, SIGNAL(triggered()), this, SLOT(doUnitTest()));
#if 0
    benchmarkAct = new QAction(tr("&Benchmark"), this);
    benchmarkAct->setStatusTip(tr("benchmark Tests"));
    connect(benchmarkAct, SIGNAL(triggered()), this, SLOT(doBenchmark()));
#endif
}
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAct);
	fileMenu->addAction(openAct);
	fileMenu->addAction(saveAct);
#if 0
    separatorMRUAct = fileMenu->addSeparator();
    //	RecentFilesMenu アイテム追加
    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(recentFileActs[i]);
    updateRecentFileActions();
#endif
    fileMenu->addSeparator();
	fileMenu->addAction(closeAllAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
	editMenu->addAction(cutAct);
	editMenu->addAction(copyAct);
	editMenu->addAction(pasteAct);

    viewMenu = menuBar()->addMenu(tr("&View"));

    otherMenu = menuBar()->addMenu(tr("&Other"));
	otherMenu->addAction(aboutAct);
}
void MainWindow::createToolBars()
{
	QToolBar *fileToolBar = addToolBar(tr("File"));
	fileToolBar->addAction(newAct);
	fileToolBar->addAction(openAct);
	fileToolBar->addAction(saveAct);

	QToolBar *editToolBar = addToolBar(tr("Edit"));
	editToolBar->addAction(undoAct);
	editToolBar->addAction(redoAct);
	editToolBar->addAction(cutAct);
	editToolBar->addAction(copyAct);
	editToolBar->addAction(pasteAct);

	QToolBar *otherToolBar = addToolBar(tr("Other"));
	otherToolBar->addAction(aboutAct);
    otherToolBar->addAction(unitTestAct);
    //QToolBar *testToolBar = addToolBar(tr("Edit"));
    //QToolBar *testToolBar = addToolBar(tr("Other"));
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
	output->viewport()->repaint();		//	強制再描画
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
void MainWindow::showAboutDlg()
{
	QMessageBox msgBox;
	msgBox.setIconPixmap(QPixmap(":vivi/Resources/images/ayabu-001.png"));
	msgBox.setText(tr("<div align=center><b><font size=5>ViVi</font></b> version %1</div>").arg(VERSION_STR));
	msgBox.setInformativeText(QString("<div align=center>Copyright (C) 2011 by N.Tsuda<br>"
								"mailto:ntsuda@master.email.ne.jp<br>"
								"<a href=\"http://vivi.dyndns.org/?from=vivi%1\">http://vivi.dyndns.org/</a><br><br>"
								"Powered by <a href=\"http://qt.nokia.com/\">Qt</a>"
								"<p>illustrated by <a href=\"http://www.pixiv.net/member.php?id=220294\"><img src=\":vivi/Resources/images/momoshiki.png\"></a>"
								"</div>").arg(VERSION_STR));
	msgBox.exec();
}
//----------------------------------------------------------------------
void MainWindow::doUnitTest()
{
}
