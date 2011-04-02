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
}
