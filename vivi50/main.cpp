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

	�{�\�[�X�R�[�h�͊�{�I�� MIT ���C�Z���X�ɏ]���B
	http://www.opensource.org/licenses/mit-license.php
	http://sourceforge.jp/projects/opensource/wiki/licenses%2FMIT_license

	�������A������͕s���R�Ŏg������̈��� GPL ���匙���Ȃ̂ŁA
	GPL ���C�Z���X�v���W�F�N�g���{�\�[�X�𗬗p���邱�Ƃ��ւ���

	���镨�����R���ǂ����́A����ɂ���ĈقȂ�B
	MIT ���C�Z���X�Ȃǂ̓v���O�����\�[�X��ǂݏ�������l�ɂƂ��Đ^��
	���R�ȃ��C�Z���X�ł��邪�AGPL�n�̓\�[�X��ǂݏ����o���Ȃ��l�ɂƂ�
	�Ď��R�ȃ��C�Z���X�ł���A�\�[�X��ǂݏ�������l�ɂƂ��Ă͕s���R
	�ɂ܂�Ȃ��B
	�\�[�X�R�[�h�̓\�[�X��ǂݏ����o����l�X�̐��ʂł���A�\�[�X��ǂ�
	�����o���Ȃ��l�̂��̂ł͂Ȃ��B
	�Ȃ̂Ƀ\�[�X��ǂݏ����o���Ȃ��l�̎��R��D�悷����ăo�J����Ȃ��́H

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
