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
