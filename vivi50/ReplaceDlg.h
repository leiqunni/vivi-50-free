//----------------------------------------------------------------------
//
//			File:			"ReplaceDlg.h"
//			Created:		17-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	ReplaceDlg クラス宣言
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

#ifndef REPLACEDLG_H
#define REPLACEDLG_H

#include <QDialog>

class ReplaceDlg : public QDialog
{
	Q_OBJECT

public:
	ReplaceDlg(QWidget *parent);
	~ReplaceDlg();

private:
	
};

#endif // REPLACEDLG_H
