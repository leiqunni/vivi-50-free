#pragma once

//----------------------------------------------------------------------
//
//			File:			"TestViCommands.h"
//			Created:		24-Feb-2011
//			Author:			Nobuhide Tsuda
//			Description:	�����e�X�g�֐��錾
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


#ifndef TESTVICOMMANDS_H
#define TESTVICOMMANDS_H

class QString;
class MainWindow;
class ViEngine;

void testViCommands(MainWindow *, ViEngine *, const QString &);

#if 0
#include <QObject>

class TestViCommands : public QObject
{
	Q_OBJECT

public:
	TestViCommands(QObject *parent);
	~TestViCommands();

private:
	
};
#endif

#endif // TESTVICOMMANDS_H
