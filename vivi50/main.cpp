//----------------------------------------------------------------------
//
//			File:			"main.cpp"
//			Created:		01-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	
//
//----------------------------------------------------------------------

/*

	Copyright (C) 2011 by Nobuhide Tsuda

	本ソースコードは基本的に MIT ライセンスに従う。
	http://www.opensource.org/licenses/mit-license.php
	http://sourceforge.jp/projects/opensource/wiki/licenses%2FMIT_license

	ただし、おいらは不自由で使い勝手の悪い GPL が大嫌いなので、
	GPL ライセンスプロジェクトが本ソースを流用することを禁じる

	ある物が自由かどうかは、立場によって異なる。
	MIT ライセンスなどはプログラムソースを読み書きする人にとって真に
	自由なライセンスであるが、GPL系はソースを読み書き出来ない人にとっ
	て自由なライセンスであり、ソースを読み書きする人にとっては不自由
	極まりない。
	ソースコードはソースを読み書き出来る人々の成果であり、ソースを読み
	書き出来ない人のものではない。

*/

#include "mainwindow.h"
#include <QtGui/QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Shift-JIS"));
	QApplication app(argc, argv);
	app.setOrganizationName("N.Tsuda");
	app.setApplicationName("vivi5");
	MainWindow w;
	w.show();
	return app.exec();
}
