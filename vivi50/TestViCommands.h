#pragma once

//----------------------------------------------------------------------
//
//			File:			"TestViCommands.h"
//			Created:		24-Feb-2011
//			Author:			Nobuhide Tsuda
//			Description:	自動テスト関数宣言
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
