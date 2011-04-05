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
	~MainWindow();

public:
    void doOutput(const QString &);

protected:
    void closeEvent(QCloseEvent *event);

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
    //void	documentWasModified(bool = true);

	//void	newFile();
	//void	open();
	//bool	save();
	//bool	saveAs();
	//void	closeAll();
    void	showAboutDlg();
	void doUnitTest();
	void doBenchmark();

private:
	PlainTextEdit	*m_editor;
	QTextEdit	*output;

    QMenu	*fileMenu;
    QMenu	*editMenu;
    QMenu	*viewMenu;
    QMenu	*otherMenu;

	QAction *newAct;
	QAction *openAct;
	QAction *saveAct;
	QAction *saveAsAct;
	QAction *closeAllAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *aboutAct;

    QAction	*unitTestAct;
    QAction	*benchmarkAct;
};

#endif // MAINWINDOW_H
