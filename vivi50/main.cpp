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
