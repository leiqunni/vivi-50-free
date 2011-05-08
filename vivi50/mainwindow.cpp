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
#include "TextView.h"
#include "TextDocument.h"
#include "charEncoding.h"

#define	VERSION_STR			"5.0.013 Dev"

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
	m_unitTestDoc = true;
	m_unitTestLaidoutBlocksMgr = true;
	m_unitTestView = true;
	m_view = new TextView;
	connect(m_view, SIGNAL(printBuffer()), this, SLOT(printBuffer()));
    QSettings settings;    const QString fontName = settings.value("fontName", "").toString();
    const int fontSize = settings.value("fontSize", 0).toInt();
    if( !fontName.isEmpty() && fontSize != 0 ) {
		m_view->setFont(QFont(fontName, fontSize));
		m_view->onFontChanged();
    }
	setCentralWidget(m_view);

	connect(m_view, SIGNAL(showMessage(const QString &)), this, SLOT(showMessage(const QString &)));
    connect(m_view->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
	statusBar()->addWidget(m_cmdLineEdit = new QLineEdit(), 1);
	m_cmdLineEdit->installEventFilter(this);
	connect(m_cmdLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(cmdLineTextChanged(const QString &)));
	connect(m_cmdLineEdit, SIGNAL(returnPressed()), this, SLOT(cmdLineReturnPressed()));
	connect(m_cmdLineEdit, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(cmdLineCursorPositionChanged(int, int)));

	createActions();
	createMenus();
	createToolBars();
	createDockWindows();
    readSettings();			//	createActions() の後にコールすること

	updateWindowTitle();
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
    connect(selectAllAct, SIGNAL(triggered()), m_view, SLOT(selectAll()));

    cutAct = new QAction(QIcon(":vivi/Resources/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
    //cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
    connect(cutAct, SIGNAL(triggered()), m_view, SLOT(cut()));

    copyAct = new QAction(QIcon(":vivi/Resources/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
    connect(copyAct, SIGNAL(triggered()), m_view, SLOT(copy()));

    pasteAct = new QAction(QIcon(":vivi/Resources/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
    connect(pasteAct, SIGNAL(triggered()), m_view, SLOT(paste()));

	undoAct = new QAction(QIcon(":vivi/Resources/images/editundo.png"), tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("undo edit commande"));
	//undoAct->setEnabled(false);
    connect(undoAct, SIGNAL(triggered()), m_view, SLOT(undo()));
    connect(m_view, SIGNAL(undoAvailable(bool)), undoAct, SLOT(setEnabled(bool)));

	redoAct = new QAction(QIcon(":vivi/Resources/images/editredo.png"), tr("&Redo"), this);
    redoAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    //redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("redo edit commande"));
	//redoAct->setEnabled(false);
    connect(redoAct, SIGNAL(triggered()), m_view, SLOT(redo()));
    connect(m_view, SIGNAL(redoAvailable(bool)), redoAct, SLOT(setEnabled(bool)));

	findAct = new QAction(QIcon(":vivi/Resources/images/Search.png"), tr("&Find..."), this);
    findAct->setShortcut(QKeySequence::Find);
    findAct->setStatusTip(tr("find strings..."));
    connect(findAct, SIGNAL(triggered()), m_view, SLOT(find()));
	findNextAct = new QAction(QIcon(":vivi/Resources/images/Paper-arrow.png"), tr("Find&Next"), this);
    findNextAct->setShortcut(QKeySequence(Qt::Key_F3));
    findNextAct->setStatusTip(tr("find next"));
    connect(findNextAct, SIGNAL(triggered()), m_view, SLOT(findNext()));
	findPrevAct = new QAction(QIcon(":vivi/Resources/images/Paper-arrow-back.png"), tr("Find&Prev"), this);
    findPrevAct->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_F3));
    findPrevAct->setStatusTip(tr("find prev"));
    connect(findPrevAct, SIGNAL(triggered()), m_view, SLOT(findPrev()));
	findCurWordAct = new QAction(tr("FindCur&Word"), this);
    findCurWordAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F3));
    findCurWordAct->setStatusTip(tr("find cursor position's word text"));
    connect(findCurWordAct, SIGNAL(triggered()), m_view, SLOT(findCurWord()));

	replaceAct = new QAction(tr("&Replace..."), this);
    replaceAct->setShortcut(QKeySequence(Qt::Key_F4));
    replaceAct->setStatusTip(tr("replace strings..."));
    connect(replaceAct, SIGNAL(triggered()), m_view, SLOT(replace()));

	linebreakAct = new QAction(tr("&Linebreak at right edge"), this);
    linebreakAct->setStatusTip(tr("Linebreak long lines at right edge"));
    linebreakAct->setCheckable(true);
    linebreakAct->setChecked(false);
    connect(linebreakAct, SIGNAL(toggled(bool)), m_view, SLOT(onLineBreak(bool)));

	fontAct = new QAction(/*QIcon(":vivi/Resources/images/editredo.png"),*/ tr("&Font..."), this);
    fontAct->setStatusTip(tr("select Font family and/or size"));
    connect(fontAct, SIGNAL(triggered()), this, SLOT(font()));

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
	benchmarkReplaceAct = new QAction(tr("benchmark&Replace"), this);
    benchmarkReplaceAct->setCheckable(true);
    benchmarkReplaceAct->setChecked(true);
    connect(benchmarkReplaceAct, SIGNAL(toggled(bool)), this, SLOT(onBenchmarkReplace(bool)));

	unitTestDocAct = new QAction(tr("unitTest&Document"), this);
    unitTestDocAct->setCheckable(true);
    unitTestDocAct->setChecked(true);
    connect(unitTestDocAct, SIGNAL(toggled(bool)), this, SLOT(onUnitTestDoc(bool)));
	unitTestViewAct = new QAction(tr("unitTest&View"), this);
    unitTestViewAct->setCheckable(true);
    unitTestViewAct->setChecked(true);
    connect(unitTestViewAct, SIGNAL(toggled(bool)), this, SLOT(onUnitTestView(bool)));
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

    searchMenu = menuBar()->addMenu(tr("Search(&K)"));
    searchMenu->addAction(findAct);
    searchMenu->addAction(findPrevAct);
    searchMenu->addAction(findNextAct);
    searchMenu->addAction(findCurWordAct);
    searchMenu->addAction(replaceAct);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(linebreakAct);

    settingsMenu = menuBar()->addMenu(tr("&Settings"));
	settingsMenu->addAction(fontAct);

    otherMenu = menuBar()->addMenu(tr("&Other"));
	otherMenu->addAction(aboutAct);
	otherMenu->addAction(printBufferAct);
	otherMenu->addAction(unitTestAct);
	QMenu *unitTestOpt = new QMenu("unitTestOptions");
		unitTestOpt->addAction(unitTestDocAct);
		unitTestOpt->addAction(unitTestViewAct);
	otherMenu->addMenu(unitTestOpt);
	otherMenu->addAction(benchmarkAct);
	QMenu *benchmarkOpt = new QMenu("benchmarkOptions");
		benchmarkOpt->addAction(benchmarkReplaceAct);
	otherMenu->addMenu(benchmarkOpt);
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

	QToolBar *searchToolBar = addToolBar(tr("Search"));
	searchToolBar->addAction(findAct);
	searchToolBar->addAction(findPrevAct);
	searchToolBar->addAction(findNextAct);

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
    m_outputDock = new QDockWidget(tr("Output"), this);
    m_outputDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_output = new QTextEdit(m_outputDock);
    m_output->setReadOnly(true);
    m_outputDock->setWidget(m_output);
    addDockWidget(Qt::BottomDockWidgetArea, m_outputDock);
    viewMenu->addAction(m_outputDock->toggleViewAction());
	m_output->viewport()->installEventFilter(this);
}
void MainWindow::onUnitTestDoc(bool b)
{
    QSettings settings;
    settings.setValue("unitTestDoc", m_unitTestDoc = b);
}
void MainWindow::onUnitTestView(bool b)
{
    QSettings settings;
    settings.setValue("unitTestView", m_unitTestView = b);
}
void MainWindow::onBenchmarkReplace(bool b)
{
    QSettings settings;
    settings.setValue("benchmarkReplace", m_benchmarkReplace = b);
}
void MainWindow::readSettings()
{
    QSettings settings;
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
    m_unitTestDoc = settings.value("unitTestDoc", true).toBool();
    unitTestDocAct->setChecked(m_unitTestDoc);
    m_unitTestView = settings.value("unitTestView", true).toBool();
    unitTestViewAct->setChecked(m_unitTestView);
    m_benchmarkReplace = settings.value("benchmarkReplace", true).toBool();
    benchmarkReplaceAct->setChecked(m_benchmarkReplace);
    m_view->setLineBreakMode(settings.value("linebreak", false).toBool());
    linebreakAct->setChecked(m_view->lineBreakMode());
}
void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    //settings.setValue("linebreak", m_view->lineBreakMode());
}
bool MainWindow::maybeSave()
{
	if( isWindowModified()) {
		QMessageBox::StandardButton ret;
		ret = QMessageBox::warning(this, tr("ViVi 5.0"),
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
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
	if( obj == m_output->viewport() ) {
		//qDebug() << event->type();
		if( event->type() == QEvent::MouseButtonDblClick ) {
			const QTextCursor cur = m_output->textCursor();
			const QTextBlock block = cur.block();
			//qDebug() << block.text();
			QString text = block.text();
			int ix, ix2;
			if( !text.isEmpty() && text[0] == '"' &&
				(ix = text.indexOf("\"(", 1)) > 1 &&
				(ix2 = text.indexOf(")", ix)) > 1 )
			{
				QString fileName = text.mid(1, ix - 1);
				int lineNum = text.mid(ix+2, ix2-(ix+2)).toInt();
				loadFile(fileName, lineNum);
			}
		}
	}
	
	return false;
}
void MainWindow::doOutput(const QString &text)
{
	m_outputDock->show();
	QTextCursor cur = m_output->textCursor();
	cur.movePosition(QTextCursor::End);
	cur.insertText(text);
	m_output->setTextCursor(cur);
	m_output->viewport()->repaint();		//	強制再描画
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
        open(action->data().toString());
}
void MainWindow::doJump(int lineNum)
{
	if( lineNum && m_view != 0 )
		m_view->doJump(lineNum);
}
MainWindow *MainWindow::findMainWindow(const QString &fileName)
{
#if 0
	QString fn;
	if( fileName.left(2) == "./" || fileName.left(2) == ".\\" )
		fn == fileName.mid(2);
	else
		fn = fileName;
    //QString canonicalFilePath = QFileInfo(fn).canonicalFilePath();
	QString absFilePath = QFileInfo(fn).absoluteFilePath
#else
	QString absFilePath = QFileInfo(fileName).absoluteFilePath();
#endif

    foreach( QWidget *widget, qApp->topLevelWidgets() ) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if( mainWin && mainWin->m_absFilePath == absFilePath )
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
	title += " - ViVi ";
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
    	m_view->document()->setFullPath(fileName);
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
		if( m_isUntitled && m_view->document()->isEmpty()
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
	//QMessageBox::warning(this, "test", "MainWindow::saveAs()");
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), m_curFile, "*.*");

	//QMessageBox::warning(this, "test", "fileName = " + fileName);
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
	//QMessageBox::warning(this, "ViVi 5.0", "MainWindow::saveFile " + fileName);
	QFile file(fileName);
	if( !file.open(QFile::WriteOnly /*| QFile::Text*/) ) {
		QMessageBox::warning(this, tr("ViVi 5.0"),
							 tr("Cannot write file %1:\n%2.")
							 .arg(fileName)
							 .arg(file.errorString()));
		return false;
	}
	//QMessageBox::warning(this, "ViVi 5.0", "MainWindow::saveFile() file opened.");
	QApplication::setOverrideCursor(Qt::WaitCursor);
	//QMessageBox::warning(this, "ViVi 5.0", "MainWindow::saveFile() setOverrideCursor()");
	QTextCodec *codec = 0;
	cchar *codecName = 0;
	switch( m_view->charEncoding() ) {
	case CharEncoding::UTF8:
		if( m_view->withBOM() )
			file.write((cchar*)UTF8_BOM, UTF8_BOM_LENGTH);
		codecName = "UTF-8";
		break;
	case CharEncoding::UTF16_LE:
		if( m_view->withBOM() )
			file.write((cchar*)UTF16LE_BOM, UTF16_BOM_LENGTH);
		codecName = "UTF-16LE";
		break;
	case CharEncoding::UTF16_BE:
		if( m_view->withBOM() )
			file.write((cchar*)UTF16BE_BOM, UTF16_BOM_LENGTH);
		codecName = "UTF-16BE";
		break;
	case CharEncoding::EUC:
		codecName = "EUC-JP";
		break;
	case CharEncoding::SJIS:
		codecName = "Shift-JIS";
		break;
	}
	if( codecName != 0 )
		codec = QTextCodec::codecForName(codecName);
	if( codec == 0 )
		codec = QTextCodec::codecForLocale();
	if( codec == 0 ) {
		QMessageBox::warning(this, tr("ViVi 5.0"),
							 tr("No QTextCodec for %1.")
							 .arg(QString(codecName ? codecName : "Locale")));
		return false;
	}
	//QMessageBox::warning(this, "ViVi 5.0", "MainWindow::saveFile() set codec");
	const QString text = m_view->toPlainText();
	//QMessageBox::warning(this, "ViVi 5.0",
	//				QString("MainWindow::saveFile() toPlaintext() finished. length = %1")
	//						.arg(text.length()));
	QByteArray ba = codec->fromUnicode(text);
	//QMessageBox::warning(this, "ViVi 5.0", "MainWindow::saveFile() set QByteArray");
	file.write(ba);
	//QMessageBox::warning(this, "ViVi 5.0", "MainWindow::saveFile() wrote file");
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
	QString fn;
	if( fileName.left(2) == "./" || fileName.left(2) == ".\\" )
		fn = fileName.mid(2);
	else
		fn = fileName;
    MainWindow *existing = findMainWindow(fn);
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
	if( !::loadFile(fn, buffer, errorString, &ce, &withBOM) ) {
		QMessageBox::warning(this, tr("ViVi 5.0"),
							 tr("Cannot read file %1:\n%2.")
							 .arg(fn)
							 .arg(errorString));
		QApplication::restoreOverrideCursor();
		return;
    }
	m_view->document()->setPlainText(buffer);
	m_view->document()->setCharEncodeing(ce);
	m_view->document()->setWithBOM(withBOM);
	QApplication::restoreOverrideCursor();
    QSettings settings;
    m_view->setLineBreakMode(settings.value("linebreak", false).toBool());
    linebreakAct->setChecked(m_view->lineBreakMode());
	m_view->doJump(lineNum);
	m_view->viewport()->update();

	setCurrentFile(fn);
	statusBar()->showMessage(tr("File loaded"), 2000);
}
void MainWindow::documentWasModified()
{
    setWindowModified(m_view->document()->isModified());
    updateWindowTitle();
}
void MainWindow::font()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, m_view->font(), this);
	if( ok ) {
		m_view->setFont(font);
		m_view->onFontChanged();
	    QSettings settings;
	    settings.setValue("fontName", font.family());
	    settings.setValue("fontSize", font.pointSize());
	} else {
	}
}
void MainWindow::cmdLineReturnPressed()
{
	QString text = m_cmdLineEdit->text();
#if 0	///
	if( !text.isEmpty() ) {
		if( text[0] == ':' )
			m_viEngine->doExCommand(text.mid(1));
		else
			m_viEngine->doFind(text.mid(1), text[0] == '/');
	}
	m_viEngine->setMode(CMD);
#endif
}
void MainWindow::cmdLineCursorPositionChanged(int oldPos, int newPos)
{
	//qDebug() << "MainWindow::cmdLineCursorPositionChanged()";
#if 0	///
	if( newPos == 0 )
		m_cmdLineEdit->setCursorPosition(1);
#endif
}
void MainWindow::cmdLineTextChanged(const QString & text)
{
	//qDebug() << "MainWindow::cmdLineTextChanged()";
	m_cmdText = text;
#if 0	///
	if( text.isEmpty() || (text[0] != ':' && text[0] != '/' && text[0] != '?') )
		m_viEngine->setMode(CMD);
#endif
}
void MainWindow::setFocusToCmdLine()
{
	//qDebug() << "MainWindow::setFocusToCmdLine()";
	m_cmdLineEdit->setFocus(Qt::OtherFocusReason);
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
	msgBox.setInformativeText(QString("<br><br><div align=center>Copyright (C) 2011 by N.Tsuda<br>"
								"mailto:ntsuda@master.email.ne.jp<br>"
								"<a href=\"http://vivi.dyndns.org/?from=vivi%1\">http://vivi.dyndns.org/</a><br><br>"
								"Powered by <a href=\"http://qt.nokia.com/\">Qt</a>"
								"<p>illustrated by <a href=\"http://www.pixiv.net/member.php?id=220294\"><img src=\":vivi/Resources/images/momoshiki.png\"></a>"
								"</div>").arg(VERSION_STR));
	msgBox.exec();
}
//----------------------------------------------------------------------
