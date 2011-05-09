//----------------------------------------------------------------------
//
//			File:			"MainWindow.h"
//			Created:		01-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	MainWindow クラス宣言
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include	"vi.h"
#include	"unit_test.h"

class QAction;
class QMenu;
class QTextEdit;
class TextView;
class ViEngine;
class QLineEdit;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
    MainWindow(const QString &fileName, QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainWindow();

public:
    void doOutput(const QString &);

protected:
	bool	eventFilter(QObject *obj, QEvent *event);
    void closeEvent(QCloseEvent *event);
    MainWindow	*findMainWindow(const QString &fileName);
    void	updateWindowTitle();
    void	updateCurFile();
	bool	maybeSave();
	bool	saveFile(const QString &fileName, bool = true);
	void	loadFile(const QString &fileName, int lineNum = 1);
    void	setCurrentFile(const QString &fileName);
    void	updateRecentFileActions();
    QString strippedName(const QString &fullFileName);

private:
    void init();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();
    void readSettings();
    void writeSettings();

private slots:
	void	onModeChanged(Mode, ushort=0);
    void	documentWasModified();
	void	newFile();
	void	open();
	bool	save();
	bool	saveAs();
	//void	closeAll();
	void	open(const QString &);
	void	save(const QString &);
    void	openRecentFile();
	void	doJump(int lineNum);
    void	showAboutDlg();
	void	cmdLineTextChanged(const QString & text);
	void	cmdLineReturnPressed();
	void	cmdLineCursorPositionChanged(int, int);
	void	setFocusToCmdLine();
	void	showMessage(const QString & text);
	void	printBuffer();
	void	font();
	void	doUnitTest();
	void	onUnitTestDoc(bool);
	void	onUnitTestView(bool);
	void	doBenchmark();
	void	onBenchmarkReplace(bool b);

private:
	TextView	*m_view;
	ViEngine	*m_viEngine;
	QLineEdit	*m_cmdLineEdit;
	int			m_exCmdsIx;
	QString		m_cmdText;			//	QLineEdit に実際に入力されたテキスト
	QTextEdit	*m_output;
	QString	m_curFile;
	QString	m_absFilePath;
	bool	m_isUntitled;
	bool	m_isModified;
	bool	m_unitTestDoc;
	bool	m_unitTestLaidoutBlocksMgr;
	bool	m_unitTestView;
	bool	m_benchmarkReplace;

	QDockWidget	*m_outputDock;

    QMenu	*fileMenu;
    QMenu	*editMenu;
    QMenu	*searchMenu;
    QMenu	*viewMenu;
    QMenu	*settingsMenu;
    QMenu	*otherMenu;

	QAction *newAct;
	QAction *openAct;
	QAction *saveAct;
	QAction *saveAsAct;
	QAction *closeAllAct;
    QAction *selectAllAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *findAct;
    QAction *findNextAct;
    QAction *findPrevAct;
    QAction *findCurWordAct;	//	カーソル位置単語検索
    QAction *replaceAct;
    QAction *linebreakAct;		//	右端で折り返し
    QAction *fontAct;			//	暫定的
    QAction *aboutAct;

	QAction	*printBufferAct;
    QAction	*unitTestAct;
    QAction	*unitTestDocAct;
    QAction	*unitTestViewAct;
    QAction	*benchmarkAct;
    QAction	*benchmarkReplaceAct;

    QAction *separatorMRUAct;
    enum { MaxRecentFiles = 10 };
    QAction *recentFileActs[MaxRecentFiles];
};

#endif // MAINWINDOW_H
