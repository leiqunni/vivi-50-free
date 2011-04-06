//----------------------------------------------------------------------
//
//			File:			"charEncoding.h"
//			Created:		03-Mar-2011
//			Author:			�Óc�L�G
//			Description:	�����R�[�h����֐��錾
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
