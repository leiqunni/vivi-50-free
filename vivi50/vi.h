#pragma once

//----------------------------------------------------------------------
//
//			File:			"vi.h"
//			Created:		24-Feb-2011
//			Author:			Nobuhide Tsuda
//			Description:	vi �Ɋ֌W����O���[�o���錾
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
