#pragma once

//----------------------------------------------------------------------
//
//			File:			"WinApplication.h"
//			Created:		24-Feb-2011
//			Author:			Nobuhide Tsuda
//			Description:	WinApplication クラス宣言
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
