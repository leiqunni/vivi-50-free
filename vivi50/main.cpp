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
	なのにソースを読み書き出来ない人の自由を優先するってバカじゃないの？

*/

#include "mainwindow.h"
#include <QtGui/QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QDebug>
#include "WinApplication.h"

#if	0	//def	MSVC
#include	<windows.h>
#include	<FPIEEE.H>
bool		g_initSE = false;
//LPEXCEPTION_POINTERS pep;
EXCEPTION_RECORD ERecord;
CONTEXT		EContext;
CString		g_regs, g_t;
CString		g_retAdrString;

int MyFilter( PEXCEPTION_POINTERS pep )
{
	if( !g_initSE ) {
		g_initSE = true;
		ERecord = *pep->ExceptionRecord;
		EContext = *pep->ContextRecord;
	}
	return EXCEPTION_EXECUTE_HANDLER;
}
#endif
int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Shift-JIS"));
	WinApplication app(argc, argv);
	app.setOrganizationName("N.Tsuda");
	app.setApplicationName("vivi5");
	QString locale = QLocale::system().name();
	QTranslator translator;
	bool rc = translator.load(QString("vivi50_") + locale);
	if( !rc ) {
		qDebug() << "can't open '" << QString("vivi50_") << locale << "'";
	}
	app.installTranslator(&translator);
	MainWindow w;
	w.show();
#ifdef	WIN32
	QObject::connect(&app, SIGNAL(imeOpenStatusChanged()), &w, SLOT(onImeOpenStatusChanged()));
#endif	//WIN32
#if	0	//def	MSVC
	__try {
		return app.exec();
	}
	__except(//pep = (LPEXCEPTION_POINTERS)GetExceptionInformation(),
		MyFilter(GetExceptionInformation()) )
	{
		qDebug() << "ExceptionRecord = " << (int)ERecord.ExceptionAddress;
	}
#else
	return app.exec();
#endif
}
