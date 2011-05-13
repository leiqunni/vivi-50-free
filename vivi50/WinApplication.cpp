//----------------------------------------------------------------------
//
//			File:			"WinApplication.cpp"
//			Created:		24-Feb-2011
//			Author:			Nobuhide Tsuda
//			Description:	WinApplication �N���X����
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

#ifdef	WIN32
#include <windows.h>
#endif	//WIN32
#include "WinApplication.h"

WinApplication::WinApplication(int & argc, char ** argv)
	: QApplication(argc, argv)
{

}

WinApplication::~WinApplication()
{

}
#ifdef	WIN32
bool WinApplication::winEventFilter ( MSG * msg, long * result )
{
	if( msg->message == WM_IME_NOTIFY ) {
		//qDebug() << "WM_IME_NOTIFY";
		DWORD dwCommand = (DWORD) msg->wParam;
		//DWORD dwData = (DWORD) msg->lParam;
		if( dwCommand == IMN_SETOPENSTATUS )
			emit imeOpenStatusChanged();
	}
	return false;
}
#endif	//WIN32
