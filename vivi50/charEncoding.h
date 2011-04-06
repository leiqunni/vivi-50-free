//----------------------------------------------------------------------
//
//			File:			"charEncoding.h"
//			Created:		03-Mar-2011
//			Author:			津田伸秀
//			Description:	文字コード判定関数宣言
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

#ifndef		_HEADER_CHARENCODING_H
#define		_HEADER_CHARENCODING_H

#include <QString>
typedef unsigned char uchar;
typedef const char cchar;
typedef const unsigned char cuchar;

#define		UTF8_BOM_LENGTH		3
#define		UTF16_BOM_LENGTH	2

extern uchar	UTF8_BOM[];
extern uchar	UTF16LE_BOM[];
extern uchar	UTF16BE_BOM[];

namespace CharEncoding {
	enum {
		UNKNOWN = 0,
		SJIS,
		EUC,
		UTF8,
		UTF16_LE,
		UTF16_BE,
	};
	enum {
		CRLF = 0,
		LF,
		CR,
	};
}

inline bool isHankakuKana(uchar uch)
{
	return uch >= 0xa1 && uch < 0xe0;
}
inline int isEUCcode(uchar uch)
{
	return uch >= 0xa1 && uch <= 0xfe;
}
bool isDBCSLeadByte(uchar ch);
bool isDBCSSecondByte(uchar ch);
uchar	checkCharEncoding(cuchar *, cuchar *, int &BOMLength);
bool	loadFile(const QString &, QString &, QString &, uchar * = 0, bool * = 0);

#endif		//_HEADER_CHARENCODING_H
