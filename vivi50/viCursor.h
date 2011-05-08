#pragma once

//----------------------------------------------------------------------
//
//			File:			"viCursor.h"
//			Created:		24-Feb-2011
//			Author:			Nobuhide Tsuda
//			Description:	viカーソル移動関数等宣言
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
	CT_ALNUM,		//	半角英数字
	CT_HANKANA,		//	半角カナ
	CT_SYM,			//	半角シンボル
	//CT_DBSPACE,		//	全角空白
	CT_HIRA,		//	全角ひらがな
	CT_KANA,		//	全角カタカナ
	CT_DBALNUM,		//	全角英数字
	CT_KANJI,		//	漢字
	//CT_SYM,			//	全角記号
	CT_CONT,		//	繰り返し記号（々など）
	CT_GREEK,		//	ギリシャ文字
	CT_ALPHA_EX,	//	その他の国の文字
	CT_OTHER,		//	その他文字
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
	int		m_nthLine;		//	Block 内の何行目か？ for j k 
};
#endif
