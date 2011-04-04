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

#define	VERSION_STR			"5.0.000 Dev"

MainWindow *pMainWindow;

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	pMainWindow = this;
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
    otherToolBar->addAction(unitTestAct);
    otherToolBar->addAction(benchmarkAct);
    //QToolBar *testToolBar = addToolBar(tr("Edit"));
    //QToolBar *testToolBar = addToolBar(tr("Other"));
    //testToolBar->addAction(unitTestAct);
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
void q_output(const QString &mess)
{
	pMainWindow->doOutput(mess);
}
void test_TextDocument();
void MainWindow::doUnitTest()
{
	g_ut_output = q_output;
	g_total_test_count = 0;
	g_total_fail_count = 0;
	if( 0 ) {
		CUnitTest ut("test");
		ut.test_equal(1, 1);
		ut.test_equal("abc", "abc");
		ut.test_equal(QString("あいうえお"), QString("あいうえお"));
		ut.test_equal(1, 2);
		ut.test_equal("abc", "xyzzz");
		ut.test_equal(QString("あいうえお"), QString("かきくけこ"));
	}
	//test_getWordEditInfo();
	test_TextDocument();
	QString temp;
	if( !g_total_fail_count ) {
		temp = QString("おめでとぉ、テスト失敗がひとつもありませんでした。(%1 tested)\n")
						.arg(g_total_test_count);
		//soundFile = theApp.getGlobSettings()->getTextValue(GLOBSTG_UT_SUCCESS_SOUND);
	} else {
		temp = QString("%1件のテスト失敗がありました。(%2 tested)\n")
						.arg(g_total_fail_count).arg(g_total_test_count);
		//soundFile = theApp.getGlobSettings()->getTextValue(GLOBSTG_UT_FAILURE_SOUND);
	}
	g_ut_output(temp);
}

#include "TextDocument.h"

void test_TextDocument()
{
	CUnitTest ut("TextDocument");
	if( 1 ) {
		TextDocument doc;
		ut.test(doc.isEmpty());
		ut.test_equal(0, doc.size());
		ut.test_equal(1, doc.blockCount());		//	EOF行のみ

		doc.setPlainText(QString("123\nabcde\r\nxyzzz\r"));
		ut.test(!doc.isEmpty());
		ut.test_equal(17, doc.size());
		ut.test_equal(4, doc.blockCount());

		TextBlock block = doc.firstBlock();
		ut.test_equal(4, block.size());			//	改行を含めたサイズ
		ut.test_equal("123\n", block.text());	//	改行を含めたテキスト
		block = block.next();
		ut.test(block.isValid());
		ut.test_equal(7, block.size());
		ut.test_equal("abcde\r\n", block.text());

		ut.test_equal(0, doc.findBlock(0).blockNumber());
		ut.test_equal(0, doc.findBlock(1).blockNumber());
		ut.test_equal(0, doc.findBlock(2).blockNumber());
		ut.test_equal(0, doc.findBlock(3).blockNumber());
		ut.test_equal(1, doc.findBlock(4).blockNumber());
		ut.test_equal(1, doc.findBlock(5).blockNumber());
		ut.test_equal(1, doc.findBlock(6).blockNumber());
		ut.test_equal(1, doc.findBlock(7).blockNumber());
		ut.test_equal(1, doc.findBlock(8).blockNumber());
		ut.test_equal(1, doc.findBlock(9).blockNumber());
		ut.test_equal(1, doc.findBlock(10).blockNumber());
		ut.test_equal(2, doc.findBlock(11).blockNumber());
		ut.test_equal(2, doc.findBlock(12).blockNumber());
		ut.test_equal(2, doc.findBlock(13).blockNumber());
		ut.test_equal(2, doc.findBlock(14).blockNumber());
		ut.test_equal(2, doc.findBlock(15).blockNumber());
		ut.test_equal(2, doc.findBlock(16).blockNumber());
		ut.test_equal(3, doc.findBlock(17).blockNumber());
		ut.test_equal(INVALID_INDEX, doc.findBlock(18).blockNumber());

		doc.setPlainText(QString("123\nabc"));			//	EOF行が空でない場合
		ut.test(!doc.isEmpty());
		ut.test_equal(7, doc.size());
		ut.test_equal(2, doc.blockCount());
	}
	if( 1 ) {
		TextDocument doc;
		doc.setPlainText(QString("123\nあいう\nかきくけこ\r\n漢字\r"));
		TextCursor cur(&doc);		//	先頭位置
		ut.test_equal(0, cur.position());
		cur.movePosition(TextCursor::Right);
		ut.test_equal(1, cur.position());
		cur.movePosition(TextCursor::Right);
		ut.test_equal(2, cur.position());
		cur.movePosition(TextCursor::Right);
		ut.test_equal(3, cur.position());		//	\n
		cur.movePosition(TextCursor::Right);
		ut.test_equal(4, cur.position());		//	あ
		cur.movePosition(TextCursor::Right);
		ut.test_equal(7, cur.position());		//	い

	}
	if( 1 ) {
		TextDocument doc;
		doc.setPlainText(QString("line-1\nline-2\nline-3\nline-4\n"));
		index_t bp;
		ut.test_equal(0, doc.findBlockIndex(0, &bp));
		ut.test_equal(0, bp);
		ut.test_equal(0, doc.findBlockIndex(5, &bp));		//	1
		ut.test_equal(0, bp);
		ut.test_equal(0, doc.findBlockIndex(6, &bp));		//	\n
		ut.test_equal(0, bp);
		ut.test_equal(1, doc.findBlockIndex(7, &bp));		//	line
		ut.test_equal(7, bp);
	}
	if( 1 ) {
		TextDocument doc;
		doc.setPlainText(QString("あいう\nかきくけこ\r\n漢字\r"));
	}
	if( 1 ) {
		TextDocument doc;
		doc.insert(0, "123\n");
		ut.test_equal(2, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	123\n
		ut.test_equal(4, doc.blockPosition(1));		//	EOF
		doc.insert(0, "xyz");		//	行の先頭に挿入
		ut.test_equal(2, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	xyz123\n
		ut.test_equal(7, doc.blockPosition(1));		//	EOF
		doc.insert(2, "12\n12");		//	行の途中に改行入テキスト挿入
		ut.test_equal(3, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	xy12\n
		ut.test_equal(5, doc.blockPosition(1));		//	12z123\n
		ut.test_equal(12, doc.blockPosition(2));	//	EOF
		doc.insert(4, "\r");		//	\n の直前に \r を挿入
		ut.test_equal(3, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	xy12\r\n
		ut.test_equal(6, doc.blockPosition(1));		//	12z123\n
		ut.test_equal(13, doc.blockPosition(2));	//	EOF
		doc.insert(1, "\r");		//	単に \r を挿入
		ut.test_equal(4, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	x\r
		ut.test_equal(2, doc.blockPosition(1));		//	y12\r\n
		ut.test_equal(7, doc.blockPosition(2));		//	12z123\n
		ut.test_equal(14, doc.blockPosition(3));	//	EOF
		doc.clear();
		doc.insert(0, "123");
		ut.test_equal(1, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	123
		doc.insert(2, "x\ny\nz");					//	改行を２つ含むテキスト挿入
		ut.test_equal(3, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	12x\n
		ut.test_equal(4, doc.blockPosition(1));		//	y\n
		ut.test_equal(6, doc.blockPosition(2));		//	z3
	}
	if( 1 ) {
		TextDocument doc;
		doc.insert(0, "123\n");
		doc.erase(1, 2);
		ut.test_equal(2, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	13\n
		ut.test_equal(3, doc.blockPosition(1));		//	EOF
		doc.clear();
		doc.insert(0, "123\nxyz\n");
		doc.erase(2, 5);		//	改行を含んで3文字削除
		ut.test_equal(2, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	12yz\n
		ut.test_equal(5, doc.blockPosition(1));		//	EOF
		doc.clear();
		doc.insert(0, "123\r\nxyz\n");
		doc.erase(2, 4);		//	\n の前の \r まで削除
		ut.test_equal(3, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	12\n
		ut.test_equal(3, doc.blockPosition(1));		//	xyz\n
		ut.test_equal(7, doc.blockPosition(2));		//	EOF
		doc.clear();
		doc.insert(0, "123\rxyz\n");
		ut.test_equal(3, doc.blockCount());
		doc.erase(2, 4);		//	\r 削除
		ut.test_equal(2, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	12xyz\n
		ut.test_equal(6, doc.blockPosition(1));		//	EOF

		doc.setPlainText(QString("123\nXYZ\n"));
		ut.test_equal(3, doc.blockCount());
		ut.test_equal(4, doc.firstBlock().size());
		ut.test_equal(QString("123\n"), doc.firstBlock().text());
		doc.erase(2, 5);		//	3文字削除
		ut.test_equal(2, doc.blockCount());
		ut.test_equal(5, doc.firstBlock().size());
		ut.test_equal(QString("12YZ\n"), doc.firstBlock().text());

		doc.setPlainText(QString("123\nXYZ\n"));
		ut.test_equal(3, doc.blockCount());
		ut.test_equal(4, doc.firstBlock().size());
		ut.test_equal(QString("123\n"), doc.firstBlock().text());
		doc.erase(2, 4);		//	2文字削除
		ut.test_equal(2, doc.blockCount());
		ut.test_equal(6, doc.firstBlock().size());
		ut.test_equal(QString("12XYZ\n"), doc.firstBlock().text());

		doc.setPlainText(QString("123\nXYZ\n"));
		ut.test_equal(3, doc.blockCount());
		ut.test_equal(4, doc.firstBlock().size());
		ut.test_equal(QString("123\n"), doc.firstBlock().text());
		doc.erase(3, 5);		//	2文字削除
		ut.test_equal(2, doc.blockCount());
		ut.test_equal(6, doc.firstBlock().size());
		ut.test_equal(QString("123YZ\n"), doc.firstBlock().text());
	}
	if( 1 ) {		//	undo/redo 対応 do_insert() テスト
		TextDocument doc;
		doc.do_insert(0, "123\n");
		ut.test_equal(QString("123\n"), doc.toPlainText());
		doc.doUndo();
		ut.test_equal(QString(""), doc.toPlainText());
		doc.doRedo();
		ut.test_equal(QString("123\n"), doc.toPlainText());
	}
	if( 1 ) {		//	undo/redo 対応 do_replace() テスト
		TextDocument doc;
		doc.setPlainText(QString("123\nxyzzz\n12345\n"));
		ut.test_equal(4, doc.firstBlock().size());
		doc.do_replace(0, 1, "XYZ");
		ut.test_equal(6, doc.firstBlock().size());
		ut.test_equal(QString("XYZ23\n"), doc.firstBlock().text());
		doc.doUndo();
		ut.test_equal(4, doc.firstBlock().size());
		ut.test_equal(QString("123\n"), doc.firstBlock().text());
		doc.doRedo();
		ut.test_equal(6, doc.firstBlock().size());
		ut.test_equal(QString("XYZ23\n"), doc.firstBlock().text());

		doc.setPlainText(QString("123\n456\n"));
		ut.test_equal(3, doc.blockCount());
		ut.test_equal(4, doc.firstBlock().size());
		ut.test_equal(QString("123\n"), doc.firstBlock().text());
		doc.do_replace(2, 5, "XYZ");
		ut.test_equal(2, doc.blockCount());
		ut.test_equal(8, doc.firstBlock().size());
		ut.test_equal(QString("12XYZ56\n"), doc.firstBlock().text());
	}
	if( 1 ) {
		TextDocument doc;
		doc.do_insert(0, "123\nxyzzz\nxyZZZ\n");
		ut.test( doc.find(QString("abc")).isNull() );
		ut.test( !doc.find(QString("xyZ")).isNull() );
		ut.test_equal(10, doc.find(QString("xyZ")).position() );
	}
	if( 1 ) {		//	TextCursor::insert() テスト
		TextDocument doc;
		doc.setPlainText(QString("0123\n"));
		ut.test_equal(5, doc.firstBlock().size());
		TextCursor cur(&doc);
		cur.setPosition(1);
		cur.insertText(QString("XYZ"));
		ut.test_equal(4, cur.position());
		ut.test_equal(8, doc.firstBlock().size());
		ut.test_equal(QString("0XYZ123\n"), doc.firstBlock().text());
		doc.doUndo();
		//ut.test_equal(4, cur.position());
		ut.test_equal(5, doc.firstBlock().size());
		ut.test_equal(QString("0123\n"), doc.firstBlock().text());
		doc.doRedo();
		ut.test_equal(4, cur.position());
		ut.test_equal(8, doc.firstBlock().size());
		ut.test_equal(QString("0XYZ123\n"), doc.firstBlock().text());

		cur.setPosition(1);
		cur.setPosition(4, TextCursor::KeepAnchor);		//	[1, 4) を選択
		cur.insertText(QString("7"));
		ut.test_equal(2, cur.position());
		ut.test_equal(6, doc.firstBlock().size());
		ut.test_equal(QString("07123\n"), doc.firstBlock().text());
		doc.doUndo();
		//ut.test_equal(4, cur.position());
		ut.test_equal(8, doc.firstBlock().size());
		ut.test_equal(QString("0XYZ123\n"), doc.firstBlock().text());
		doc.doRedo();
		ut.test_equal(2, cur.position());
		ut.test_equal(6, doc.firstBlock().size());
		ut.test_equal(QString("07123\n"), doc.firstBlock().text());
	}
	if( 1 ) {
		TextDocument *doc = new TextDocument();
		doc->setPlainText(QString("12345\n98765\n"));
		ut.test_equal(QString("12345\n"), doc->firstBlock().text());
		ut.test_equal(6, doc->firstBlock().size());
		TextCursor cur(doc);
		cur.setPosition(5);
		cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor, 3);	//	３文字選択
		cur.insertText("abcde");
		ut.test_equal(QString("12345abcde765\n"), doc->firstBlock().text());
		ut.test_equal(14, doc->firstBlock().size());
		//delete doc;
	}
	if( 1 ) {
		TextDocument *doc = new TextDocument();
		doc->setPlainText(QString("12345\n98765\n"));
		ut.test_equal(QString("12345\n"), doc->firstBlock().text());
		ut.test_equal(6, doc->firstBlock().size());
		TextCursor cur(doc);
		cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor, 3);	//	３文字選択
		cur.insertText("XYZ");
		cur.movePosition(TextCursor::Right, TextCursor::MoveAnchor, 2);	//	２文字進める
		ut.test_equal(5, cur.position());
		cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor, 3);	//	３文字選択
		cur.insertText("ABCDE");
		ut.test_equal(QString("XYZ45ABCDE765\n"), doc->firstBlock().text());
		ut.test_equal(14, doc->firstBlock().size());
		delete doc;
	}
	if( 1 ) {
		TextDocument *doc = new TextDocument();
		doc->setPlainText(QString("0123456789\n9876543210\n"));
		ut.test_equal(QString("0123456789\n"), doc->firstBlock().text());
		ut.test_equal(11, doc->firstBlock().size());
		TextCursor cur(doc);
		if( !cur.atEnd() ) {
			cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor, 3);	//	３文字選択
			cur.insertText("abcde");
			ut.test_equal(QString("abcde3456789\n"), doc->firstBlock().text());
			ut.test_equal(13, doc->firstBlock().size());
			ut.test_equal(5, cur.position());
			cur.movePosition(TextCursor::Right, TextCursor::MoveAnchor, 2);	//	２文字進める
			ut.test_equal(7, cur.position());
		}
		if( !cur.atEnd() ) {
			cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor, 3);	//	３文字選択
			cur.insertText("abcde");
			ut.test_equal(QString("abcde34abcde89\n"), doc->firstBlock().text());
			ut.test_equal(15, doc->firstBlock().size());
			ut.test_equal(12, cur.position());
			cur.movePosition(TextCursor::Right, TextCursor::MoveAnchor, 2);	//	２文字進める
			ut.test_equal(14, cur.position());
			ut.test_equal(0, cur.blockIndex());
			ut.test_equal(0, cur.blockPosition());
		}
		if( !cur.atEnd() ) {
			cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor, 3);	//	３文字選択
			ut.test_equal(1, cur.blockIndex());
			ut.test_equal(15, cur.blockPosition());
			ut.test_equal(3, doc->blockCount());
			cur.insertText("ABCDE");
			ut.test_equal(2, doc->blockCount());
			ut.test_equal(28, doc->blockSize(0));
			ut.test_equal(28, doc->firstBlock().size());
			ut.test_equal(QString("abcde34abcde89ABCDE76543210\n"), doc->firstBlock().text());
			ut.test_equal(19, cur.position());
			cur.movePosition(TextCursor::Right, TextCursor::MoveAnchor, 2);	//	２文字進める
			ut.test_equal(21, cur.position());
		}
		if( !cur.atEnd() ) {
			cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor, 3);	//	３文字選択
			cur.insertText("abcde");
			ut.test_equal(26, cur.position());
			cur.movePosition(TextCursor::Right, TextCursor::MoveAnchor, 2);	//	２文字進める
			ut.test_equal(28, cur.position());
		}
		if( !cur.atEnd() ) {
			cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor, 3);	//	３文字選択
			cur.insertText("abcde");
			ut.test_equal(33, cur.position());
			cur.movePosition(TextCursor::Right, TextCursor::MoveAnchor, 2);	//	２文字進める
			ut.test( cur.atEnd() );
			ut.test_equal(33, cur.position());
		}
		if( !cur.atEnd() ) {
			cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor, 3);	//	３文字選択
			cur.insertText("abcde");
			cur.movePosition(TextCursor::Right, TextCursor::MoveAnchor, 2);	//	２文字進める
		}
		delete doc;
	}
}
