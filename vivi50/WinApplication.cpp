//----------------------------------------------------------------------
//
//			File:			"WinApplication.cpp"
//			Created:		24-Feb-2011
//			Author:			Nobuhide Tsuda
//			Description:	WinApplication クラス実装
//
//----------------------------------------------------------------------

/*

	Copyright (C) 2011 by Nobuhide Tsuda

	本ソースコードは基本的に MIT ライセンスに従う。
	http://www.opensource.org/licenses/mit-license.php
	http://sourceforge.jp/projects/opensource/wiki/licenses%2FMIT_license

	ただし、おいらは不自由で使い勝手の悪い GPL が大嫌いなので、
	GPL ライセンスプロジェクトが本ソースを流用することを禁じる

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
