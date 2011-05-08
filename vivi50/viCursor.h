#pragma once

//----------------------------------------------------------------------
//
//			File:			"viCursor.h"
//			Created:		24-Feb-2011
//			Author:			Nobuhide Tsuda
//			Description:	vi�J�[�\���ړ��֐����錾
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

#include	<QTextCursor>
#include	"textCursor.h"

namespace ViMoveOperation {
	enum {
		Left = 100,				//	h
		Down,					//	j
		Up,						//	k
		Right,					//	l
		RightForA,				//	a
		FirstNonBlankChar,		//	^
		LastChar,				//	$
		NextWord,				//	w
		NextWordEnd,			//	e
		PrevWord,				//	b
		NextSSWord,				//	W
		NextSSWordEnd,			//	E
		PrevSSWord,				//	B
		NextLine,				//	+ Enter
		PrevLine,				//	-
		TopOfView,				//	H
		MiddleOfView,			//	M
		BottomOfView,			//	L
		MatchParen,				//	%
		NextBlankLine,			//	}
		PrevBlankLine,			//	{
		NextBeginBlock,			//	]]
		PrevBeginBlock,			//	[[
		JumpLine,				//	nG
		LastLine,				//	G
	};
}
enum {
	CT_EOF = 0,
	CT_NEWLINE,
	CT_SPACE,
	CT_ALNUM,		//	���p�p����
	CT_HANKANA,		//	���p�J�i
	CT_SYM,			//	���p�V���{��
	//CT_DBSPACE,		//	�S�p��
	CT_HIRA,		//	�S�p�Ђ炪��
	CT_KANA,		//	�S�p�J�^�J�i
	CT_DBALNUM,		//	�S�p�p����
	CT_KANJI,		//	����
	//CT_SYM,			//	�S�p�L��
	CT_CONT,		//	�J��Ԃ��L���i�X�Ȃǁj
	CT_GREEK,		//	�M���V������
	CT_ALPHA_EX,	//	���̑��̍��̕���
	CT_OTHER,		//	���̑�����
};
uchar getCharType(QChar);
bool moveCursor(ViewCursor &cur, /*int &x,*/ int mv, int n=1, bool=false, const ViewBlock *block=0);
bool moveCursorFindInLine(ViewCursor &cur, ushort, const QChar &, int n=1);
bool moveCursorFind(ViewCursor &cur, const QRegExp &, bool forward, int n=1);

typedef uchar MoveMode;

#if 0
class ViCursor : public ViewCursor
{
public:
	ViCursor();
	ViCursor(const ViCursor &);
	ViCursor(const ViewCursor &, int = 0);

public:
	int		nthLine() const { return m_nthLine; }
	int		calcNthLine();

public:
	//bool	movePosition(MoveOperation operation, MoveMode mode, int = 1);
	bool	movePosition(int mv, MoveMode mode = MoveAnchor, int n=1, bool=false, const QTextBlock *block=0);
	void	moveLeftIfEndOfLine();
	void	moveXAtNthLine(const QTextBlock &);

private:
	int		m_nthLine;		//	Block ���̉��s�ڂ��H for j k 
};
#endif
