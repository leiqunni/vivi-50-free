#include "mainwindow.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setOrganizationName("N.Tsuda");
	app.setApplicationName("vivi5");
	MainWindow w;
	w.show();
	return app.exec();
}
