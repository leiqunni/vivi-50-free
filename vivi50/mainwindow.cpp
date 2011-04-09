//----------------------------------------------------------------------
//
//			File:			"MainWindow.cpp"
//			Created:		01-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	MainWindow クラス実装
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
#include "mainwindow.h"
#include "PlainTextEdit.h"
#include "TextDocument.h"
#include "charEncoding.h"

#define	VERSION_STR			"5.0.001 Dev"

MainWindow *pMainWindow;

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	pMainWindow = this;
	init();
}
MainWindow::MainWindow(const QString &fileName, QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	init();
	if( !fileName.isEmpty() )
		loadFile(fileName);
}

MainWindow::~MainWindow()
{

}

void MainWindow::init()
{
	m_isUntitled = true;
	m_isModified = false;
	m_editor = new PlainTextEdit;
	setCentralWidget(m_editor);

	connect(m_editor, SIGNAL(showMessage(const QString &)), this, SLOT(showMessage(const QString &)));
    connect(m_editor->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));

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

#if 1
    //	RecentFilesMenu のための初期化
    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }
#endif

    selectAllAct = new QAction(tr("Select&All"), this);
    selectAllAct->setShortcuts(QKeySequence::SelectAll);
    selectAllAct->setStatusTip(tr("Select All text"));
    connect(selectAllAct, SIGNAL(triggered()), m_editor, SLOT(selectAll()));

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
	//undoAct->setEnabled(false);
    connect(undoAct, SIGNAL(triggered()), m_editor, SLOT(undo()));
    connect(m_editor, SIGNAL(undoAvailable(bool)), undoAct, SLOT(setEnabled(bool)));

	redoAct = new QAction(QIcon(":vivi/Resources/images/editredo.png"), tr("&Redo"), this);
    redoAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    //redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("redo edit commande"));
	//redoAct->setEnabled(false);
    connect(redoAct, SIGNAL(triggered()), m_editor, SLOT(redo()));
    connect(m_editor, SIGNAL(redoAvailable(bool)), redoAct, SLOT(setEnabled(bool)));

    aboutAct = new QAction(QIcon(":vivi/Resources/images/Info.png"), tr("&About ViVi"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(showAboutDlg()));

    printBufferAct = new QAction(QIcon(":vivi/Resources/images/Warning.png"), tr("&PrintBuffer"), this);
    printBufferAct->setStatusTip(tr("print buffer"));
    connect(printBufferAct, SIGNAL(triggered()), this, SLOT(printBuffer()));

    unitTestAct = new QAction(QIcon(":vivi/Resources/images/Checkmark.png"), tr("&UnitTest"), this);
    unitTestAct->setStatusTip(tr("exec Unit Tests"));
    connect(unitTestAct, SIGNAL(triggered()), this, SLOT(doUnitTest()));
    benchmarkAct = new QAction(QIcon(":vivi/Resources/images/Clock.png"), tr("&Benchmark"), this);
    benchmarkAct->setStatusTip(tr("benchmark Tests"));
    connect(benchmarkAct, SIGNAL(triggered()), this, SLOT(doBenchmark()));
}
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAct);
	fileMenu->addAction(openAct);
	fileMenu->addAction(saveAct);
	fileMenu->addAction(saveAsAct);
#if 1
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
	editMenu->addAction(selectAllAct);

    viewMenu = menuBar()->addMenu(tr("&View"));

    otherMenu = menuBar()->addMenu(tr("&Other"));
	otherMenu->addAction(aboutAct);
	otherMenu->addAction(printBufferAct);
	otherMenu->addAction(unitTestAct);
	otherMenu->addAction(benchmarkAct);
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
    otherToolBar->addAction(printBufferAct);
    otherToolBar->addAction(unitTestAct);
    otherToolBar->addAction(benchmarkAct);
    //QToolBar *testToolBar = addToolBar(tr("Edit"));
    //QToolBar *testToolBar = addToolBar(tr("Other"));
    //testToolBar->addAction(unitTestAct);
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
bool MainWindow::maybeSave()
{
	if( isWindowModified()) {
		QMessageBox::StandardButton ret;
		ret = QMessageBox::warning(this, tr("qvi"),
					 tr("The document has been modified.\n"
						"Do you want to save your changes?"),
					 QMessageBox::Save | QMessageBox::Discard
			 | QMessageBox::Cancel);
		if( ret == QMessageBox::Save )
			return save();
		else if( ret == QMessageBox::Cancel )
			return false;
	}
	return true;
}
void MainWindow::closeEvent(QCloseEvent *event)
{
#if 0
    writeSettings();
#else
    if( maybeSave() ) {
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
//	settings から RecentFile 情報を取り出し、recentFileActs に設定
void MainWindow::updateRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);
    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg((i + 1) % 10).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setStatusTip(files[i]);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);

    separatorMRUAct->setVisible(numRecentFiles > 0);
}
QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        loadFile(action->data().toString());
}
void MainWindow::doJump(int lineNum)
{
	if( lineNum && m_editor != 0 )
		m_editor->doJump(lineNum);
}
MainWindow *MainWindow::findMainWindow(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach( QWidget *widget, qApp->topLevelWidgets() ) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if( mainWin && mainWin->m_curFile == canonicalFilePath )
            return mainWin;
    }
    return 0;
}
void MainWindow::updateCurFile()
{
    static int sequenceNumber = 0;
    if( m_curFile.isEmpty() )
        m_curFile = tr("document%1.txt").arg(++sequenceNumber);
}
void MainWindow::updateWindowTitle()
{
	updateCurFile();
	QString title = m_curFile;
	if( isWindowModified() )
		title += "*";
	title += " - qvi ";
	title += VERSION_STR;
    setWindowTitle(title);
}
void MainWindow::setCurrentFile(const QString &fileName)
{

    m_isUntitled = fileName.isEmpty();
    if( m_isUntitled ) {
        m_curFile.clear();
        updateCurFile();
    } else {
    	m_editor->document()->setFullPath(fileName);
        m_curFile = QFileInfo(fileName).canonicalFilePath();
    }
    setWindowModified(false);
    updateWindowTitle();

#if 1
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();
    settings.setValue("recentFileList", files);
    updateRecentFileActions();
#endif
}
void MainWindow::newFile()
{
	MainWindow *other = new MainWindow;
	other->move(x() + 40, y() + 40);
	other->show();
}
void MainWindow::open(const QString &fileName)
{
	if( fileName.isEmpty() )
		open();
	else {
		if( m_isUntitled && m_editor->document()->isEmpty()
				&& !isWindowModified() )
		{
			loadFile(fileName);
		} else {
			MainWindow *other = new MainWindow(fileName);
			if( other->m_isUntitled ) {
				delete other;
				return;
			}
			other->move(x() + 40, y() + 40);
			other->show();
		}
	}
}
bool MainWindow::save()
{
	if( m_isUntitled ) {
		return saveAs();
	} else {
		return saveFile(m_curFile);
	}
}
bool MainWindow::saveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), m_curFile, "*.*");

	if( fileName.isEmpty() )
		return false;

	return saveFile(fileName);
}
void MainWindow::save(const QString &fileName)
{
	if( fileName.isEmpty() )
		save();
	else {
		saveFile(fileName, false);
	}
}
bool MainWindow::saveFile(const QString &fileName, bool replace)
{
	QFile file(fileName);
	if( !file.open(QFile::WriteOnly /*| QFile::Text*/) ) {
		QMessageBox::warning(this, tr("qvi"),
							 tr("Cannot write file %1:\n%2.")
							 .arg(fileName)
							 .arg(file.errorString()));
		return false;
	}
	QApplication::setOverrideCursor(Qt::WaitCursor);
	QTextCodec *codec = 0;
	switch( m_editor->charEncoding() ) {
	case CharEncoding::UTF8:
		if( m_editor->withBOM() )
			file.write((cchar*)UTF8_BOM, UTF8_BOM_LENGTH);
		codec = QTextCodec::codecForName("UTF-8");
		break;
	case CharEncoding::UTF16_LE:
		if( m_editor->withBOM() )
			file.write((cchar*)UTF16LE_BOM, UTF16_BOM_LENGTH);
		codec = QTextCodec::codecForName("UTF-16LE");
		break;
	case CharEncoding::UTF16_BE:
		if( m_editor->withBOM() )
			file.write((cchar*)UTF16BE_BOM, UTF16_BOM_LENGTH);
		codec = QTextCodec::codecForName("UTF-16BE");
		break;
	case CharEncoding::EUC:
		codec = QTextCodec::codecForName("EUC-JP");
		break;
	case CharEncoding::UNKNOWN:
	default:
		codec = QTextCodec::codecForName("Shift-JIS");
	}
	QByteArray ba = codec->fromUnicode(m_editor->toPlainText());
	file.write(ba);
	QApplication::restoreOverrideCursor();

	if( replace )
		setCurrentFile(fileName);
	statusBar()->showMessage(tr("File saved"), 2000);
	return true;
}
void MainWindow::open()
{
	const QString dir = QDir::currentPath();
	QString fileName = QFileDialog::getOpenFileName(this,
										tr("Open File"),
										dir,
										"*.*");
	//setDefaultSuffix("svg");
	if( !fileName.isEmpty()) {
		open(fileName);
		//loadFile(fileName);
	}
}
void MainWindow::loadFile(const QString &fileName, int lineNum)
{
    MainWindow *existing = findMainWindow(fileName);
    if( existing ) {
    	if( lineNum ) existing->doJump(lineNum);
        existing->show();
        existing->raise();
        existing->activateWindow();
        return;
    }

	QApplication::setOverrideCursor(Qt::WaitCursor);
    QString buffer;
    QString errorString;
    uchar ce;
    bool withBOM;
	if( !::loadFile(fileName, buffer, errorString, &ce, &withBOM) ) {
		QMessageBox::warning(this, tr("qvi"),
							 tr("Cannot read file %1:\n%2.")
							 .arg(fileName)
							 .arg(errorString));
		QApplication::restoreOverrideCursor();
		return;
    }
	m_editor->document()->setPlainText(buffer);
	m_editor->document()->setCharEncodeing(ce);
	m_editor->document()->setWithBOM(withBOM);
	QApplication::restoreOverrideCursor();
	m_editor->doJump(lineNum);
	m_editor->viewport()->update();

	setCurrentFile(fileName);
	statusBar()->showMessage(tr("File loaded"), 2000);
}
void MainWindow::documentWasModified()
{
    setWindowModified(m_editor->document()->isModified());
    updateWindowTitle();
}
void MainWindow::showMessage(const QString & text)
{
	//qDebug() << "MainWindow::showMessage()";
	//if( m_viEngine->mode() != CMDLINE ) {
		//m_cmdLineEdit->hide();
		//qDebug() << "text = " << text;
		statusBar()->showMessage(text);
	//}
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
