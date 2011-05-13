#pragma once

//----------------------------------------------------------------------
//
//			File:			"WinApplication.h"
//			Created:		24-Feb-2011
//			Author:			Nobuhide Tsuda
//			Description:	WinApplication �N���X�錾
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

#ifndef WINAPPLICATION_H
#define WINAPPLICATION_H

#include <QApplication>

class WinApplication : public QApplication
{
	Q_OBJECT

public:
	WinApplication(int & argc, char ** argv);
	~WinApplication();

#ifdef	WIN32
protected:
	bool winEventFilter ( MSG * msg, long * result );
#endif	//	WIN32

signals:
	void imeOpenStatusChanged();
};

#endif // WINAPPLICATION_H
