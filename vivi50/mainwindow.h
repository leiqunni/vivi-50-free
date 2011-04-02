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

private:
	PlainTextEdit	*m_editor;
	QTextEdit	*output;

    QMenu	*fileMenu;
    QMenu	*editMenu;
    QMenu	*viewMenu;
    QAction	*unitTestAct;
    QAction	*benchmarkAct;
};

#endif // MAINWINDOW_H
