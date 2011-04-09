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
#include	"unit_test.h"

class QAction;
class QMenu;
class QTextEdit;
class PlainTextEdit;

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
	void	showMessage(const QString & text);
	void	printBuffer();
	void doUnitTest();
	void doBenchmark();

private:
	PlainTextEdit	*m_editor;
	QTextEdit	*output;
	QString	m_curFile;
	bool	m_isUntitled;
	bool	m_isModified;

    QMenu	*fileMenu;
    QMenu	*editMenu;
    QMenu	*viewMenu;
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
    QAction *aboutAct;

	QAction	*printBufferAct;
    QAction	*unitTestAct;
    QAction	*benchmarkAct;

    QAction *separatorMRUAct;
    enum { MaxRecentFiles = 10 };
    QAction *recentFileActs[MaxRecentFiles];
};

#endif // MAINWINDOW_H
