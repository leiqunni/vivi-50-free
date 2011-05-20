//----------------------------------------------------------------------
//
//			File:			"findOpt.h"
//			Created:		17-Apr-2011
//			Author:			津田伸秀
//			Description:
//
//----------------------------------------------------------------------

#pragma once

#ifndef		_HEADER_FINDOPT_H
#define		_HEADER_FINDOPT_H

enum {
	IgnoreCase = 0,
	MatchCase = 0x01,
	IgnoreLowerCase = 0x02,		//	小文字のみ検索時：大文字小文字同一視
	FindBackWard = 0x04,
	RegExp = 0x08,				//	正規表現検索
};


#endif		//_HEADER_FINDOPT_H
