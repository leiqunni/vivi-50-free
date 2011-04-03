#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

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
	void doUnitTest();
	//void doBenchmark();

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
