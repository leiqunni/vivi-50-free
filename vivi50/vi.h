#pragma once

//----------------------------------------------------------------------
//
//			File:			"vi.h"
//			Created:		24-Feb-2011
//			Author:			Nobuhide Tsuda
//			Description:	vi に関係するグローバル宣言
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


#pragma once

#define		USE_EVENT_FILTER		1

enum Mode {
	CMD = 0,
	INSERT,
	REPLACE,
	CMDLINE,
};

namespace ViScrollOperation {
	enum {
		NextPage = 1,			//	^F
		PrevPage,				//	^B
		NextHalfPage,			//	^D
		PrevHalfPage,			//	^U
		ExposeTop,				//	^Y
		ExposeBottom,			//	^E
	};
}
#if 0
namespace ViMoveOperation {
	enum {
		Left = 100,				//	h
		Down,					//	j
		Up,						//	k
		Right,					//	l
		RightForA,				//	a
		FirstNonBlankChar,		//	^
		LastChar,				//	$
		NextLine,				//	+ Enter
		PrevLine,				//	-
	};
}
#endif
