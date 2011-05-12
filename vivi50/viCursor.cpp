//----------------------------------------------------------------------
//
//			File:			"viCursor.cpp"
//			Created:		24-Feb-2011
//			Author:			Nobuhide Tsuda
//			Description:	viカーソル移動関数実装
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

#include <QtGui>
#include "viCursor.h"
#include "TextDocument.h"
#include "TextView.h"
#include "textBlock.h"

int getEOLOffset(const QString text);

#if 0
//	末尾改行（CR/LF/CRLF）位置を返す
int EOLOffset(const QString &text)
{
	int ix = text.length();
	if( ix != 0 ) {
		if( text[ix-1] == 0x0d )
			--ix;
		if( text[ix-1] == 0x0a ) {
			if( ix > 1 && text[ix-2] == 0x0d )
				ix -= 2;
			else
				--ix;
		}
	}
	return ix;
}
#endif
inline bool isTabOrSpace(const QChar ch)
{
	return ch == '\t' || ch == ' ';
}
int firstNonBlankCharPos(const QString &text)
{
	int ix = 0;
	while( ix < text.length() && isTabOrSpace(text[ix]) )
		++ix;
	return ix;
}
void moveToFirstNonBlankChar(ViewCursor &cur)
{
	ViewBlock block = cur.block();
	const int blockPos = block.position();
	const QString blockText = block.text();
	if( !blockText.isEmpty() )
		cur.setPosition(blockPos + firstNonBlankCharPos(blockText));
}
bool gotoNextWord(ViewCursor &cur, int n, bool cdy)
{
	ViewBlock block = cur.block();
	int blockPos = block.position();
	QString text = block.text();
	int ix = cur.position() - blockPos;
	while( --n >= 0 ) {
		//	同タイプ文字を読み飛ばす
		if( ix < text.length() && !isTabOrSpace(text[ix]) ) {
			uchar cat = getCharType(text[ix++]);
			//QChar::Category cat = text[ix++].category();
			while( ix < text.length() && !isTabOrSpace(text[ix]) && getCharType(text[ix]) == cat )
				++ix;
		}
		//	空白類を読み飛ばす
		while( ix == text.length() || isTabOrSpace(text[ix]) ) {
			if( ix == text.length() ) {
				if( cdy && !n )	//	cdy が前置されている場合は、最後の改行はスキップしない
					break;
				block = block.next();
				if( !block.isValid() ) {
					cur.setPosition(blockPos + ix);
					return true;
				}
				blockPos = block.position();
				text = block.text();
				ix = 0;
			} else
				++ix;
		}
	}
	cur.setPosition(blockPos + ix);
	return true;
}
bool gotoPrevWord(ViewCursor &cur, int n)
{
	ViewBlock block = cur.block();
	int blockPos = block.position();
	QString text = block.text();
	int ix = cur.position() - blockPos;
	while( --n >= 0 ) {
		//	ひとつ前の文字が空白類 or 行頭なら文書先頭方向に移動
		while( !ix || isTabOrSpace(text[ix-1]) ) {
			if( !ix ) {
				block = block.prev();
				if( !block.isValid() ) {
					cur.setPosition(blockPos);
					return true;
				}
				blockPos = block.position();
				text = block.text();
				ix = text.length();
			} else
				--ix;
		}
		//	ひとつ前の文字が同じタイプ and 空白類になるまで or 行頭まで読み飛ばす
		if( ix > 0 && !isTabOrSpace(text[ix-1]) ) {
			uchar cat = getCharType(text[--ix]);
			while( ix > 0 && !isTabOrSpace(text[ix-1]) && getCharType(text[ix-1]) == cat )
				--ix;
		}
	}
	cur.setPosition(blockPos + ix);
	return true;
}
#if 0
bool gotoNextWordEnd(DocCursor &cur, int n, bool cdy)
{
	DocBlock block = cur.block();
	int blockPos = block.position();
	QString text = block.text();
	int ix = cur.position() - blockPos;
	int EOLIndex = getEOLOffset(text);
	while( --n >= 0 ) {
		//	次の文字が空白類 or 行末なら文書末尾方向に移動
		while( ix + 1 >= EOLIndex || isTabOrSpace(text[ix+1]) ) {
			if( ix + 1 >= EOLIndex ) {
				block = block.next();
				if( !block.isValid() ) {
					cur.setPosition(blockPos);
					return true;
				}
				blockPos = block.position();
				text = block.text();
				EOLIndex = getEOLOffset(text);
				ix = 0;
				if( ix < EOLIndex && !isTabOrSpace(text[ix]) )
					break;
			} else
				++ix;
		}
		//	次の文字が空白類になるまで or 行末まで読み飛ばす
		if( ix + 1 < EOLIndex && !isTabOrSpace(text[ix+1]) ) {
			uchar cat = getCharType(text[++ix]);
			while( ix + 1 < EOLIndex && !isTabOrSpace(text[ix+1]) && getCharType(text[ix+1]) == cat )
				++ix;
		}
	}
	if( cdy ) ++ix;
	cur.setPosition(blockPos + ix);
	return true;
}
bool gotoNextSSWord(ViewCursor &cur, int n, bool cdy)
{
	ViewBlock block = cur.block();
	int blockPos = block.position();
	QString text = block.text();
	int ix = cur.position() - blockPos;
	while( --n >= 0 ) {
		//	空白類まで読み飛ばす
		while( ix < text.length() && !isTabOrSpace(text[ix]) )
			++ix;
		//	空白類を読み飛ばす
		while( ix == text.length() || isTabOrSpace(text[ix]) ) {
			if( ix == text.length() ) {
				if( cdy && !n )	//	cdy が前置されている場合は、最後の改行はスキップしない
					break;
				block = block.next();
				if( !block.isValid() ) {
					cur.setPosition(blockPos + ix);
					return true;
				}
				blockPos = block.position();
				text = block.text();
				ix = 0;
			} else
				++ix;
		}
	}
	cur.setPosition(blockPos + ix);
	return true;
}
#endif
bool gotoPrevSSWord(ViewCursor &cur, int n)
{
	ViewBlock block = cur.block();
	int blockPos = block.position();
	QString text = block.text();
	int ix = cur.position() - blockPos;
	while( --n >= 0 ) {
		//	ひとつ前の文字が空白類 or 行頭なら文書先頭方向に移動
		while( !ix || isTabOrSpace(text[ix-1]) ) {
			if( !ix ) {
				block = block.prev();
				if( !block.isValid() ) {
					cur.setPosition(blockPos);
					return true;
				}
				blockPos = block.position();
				text = block.text();
				ix = text.length();
			} else
				--ix;
		}
		//	ひとつ前の文字が空白類になるまで or 行頭まで読み飛ばす
		while( ix > 0 && !isTabOrSpace(text[ix-1]) )
			--ix;
	}
	cur.setPosition(blockPos + ix);
	return true;
}
/**
	NextSSWord と PrevSSWord が対称的ではなく、
	実は PrevSSWord と NextSSWordEnd が対称的
*/
bool gotoNextSSWordEnd(ViewCursor &cur, int n, bool cdy)
{
	ViewBlock block = cur.block();
	int blockPos = block.position();
	QString text = block.text();
	int ix = cur.position() - blockPos;
	while( --n >= 0 ) {
		//	次の文字が空白類 or 行末なら文書末尾方向に移動
		while( ix + 1 >= text.length() || isTabOrSpace(text[ix+1]) ) {
			if( ix + 1 >= text.length() ) {
				block = block.next();
				if( !block.isValid() ) {
					cur.setPosition(blockPos);
					return true;
				}
				blockPos = block.position();
				text = block.text();
				ix = 0;
				if( ix < text.length() && !isTabOrSpace(text[ix]) )
					break;
			} else
				++ix;
		}
		//	次の文字が空白類になるまで or 行末まで読み飛ばす
		while( ix + 1 < text.length() && !isTabOrSpace(text[ix+1]) )
			++ix;
	}
	if( cdy ) ++ix;
	cur.setPosition(blockPos + ix);
	return true;
}

/**
*/
bool gotoBeginBlock(ViewCursor &cur, bool forward, int n)
{
	ViewBlock block = cur.block();
	if( !block.isValid() ) return false;
	int lastPos = block.position();
	while( --n >= 0 ) {
		for(;;) {
			block = forward ? block.next() : block.prev();
			if( !block.isValid() ) {
				cur.setPosition(lastPos);
				return true;
			}
			lastPos = block.position();
			const QString text = block.text();
			if( !text.isEmpty() && text[0] == '{' )
				break;
		}
	}
	cur.setPosition(lastPos);
	return true;
}
#if 1
bool gotoBlankLine(ViewCursor &cur, bool forward, int n)
{
	ViewBlock block = cur.block();
	if( !block.isValid() ) return false;
	int lastPos = block.position();
	while( --n >= 0 ) {
		//	空行を読み飛ばす
		while( block.EOLOffset() == 0 ) {
			block = forward ? block.next() : block.prev();
			if( !block.isValid() ) {
				cur.setPosition(lastPos);
				return true;
			}
			lastPos = block.position();
		}
		//	空行まで読み飛ばす
		while( block.EOLOffset() != 0 ) {
			block = forward ? block.next() : block.prev();
			if( !block.isValid() ) {
				cur.setPosition(lastPos);
				return true;
			}
			lastPos = block.position();
		}
	}
	cur.setPosition(lastPos);
	return true;
}
#else
bool gotoNextBlankLine(ViewCursor &cur, int n)
{
	QTextBlock block = cur.block();
	if( !block.isValid() ) return false;
	int lastPos = block.position();
	while( --n >= 0 ) {
		//	空行を読み飛ばす
		while( EOLOffset(block.text()) == 0 ) {
			block = block.next();
			if( !block.isValid() ) {
				cur.setPosition(lastPos);
				return true;
			}
			lastPos = block.position();
		}
		//	空行まで読み飛ばす
		while( EOLOffset(block.text()) != 0 ) {
			block = block.next();
			if( !block.isValid() ) {
				cur.setPosition(lastPos);
				return true;
			}
			lastPos = block.position();
		}
	}
	cur.setPosition(lastPos);
	return true;
}
bool gotoPrevBlankLine(ViewCursor &cur, int n)
{
	QTextBlock block = cur.block();
	if( !block.isValid() ) return false;
	int lastPos = block.position();
	while( --n >= 0 ) {
		//	空行を読み飛ばす
		while( EOLOffset(block.text()) == 0 ) {
			block = block.prev();
			if( !block.isValid() ) {
				cur.setPosition(lastPos);
				return true;
			}
			lastPos = block.position();
		}
		//	空行まで読み飛ばす
		while( EOLOffset(block.text()) != 0 ) {
			block = block.prev();
			if( !block.isValid() ) {
				cur.setPosition(lastPos);
				return true;
			}
			lastPos = block.position();
		}
	}
	cur.setPosition(lastPos);
	return true;
}
#endif
bool gotoMatchParen(ViewCursor &cur)
{
	ViewBlock block = cur.block();
	int blockPos = block.position();
	QString blockText = block.text();
	int ix = cur.position() - blockPos;
	bool forward = true;
	QChar paren, dst;
	while( ix < blockText.length() ) {
		if( blockText[ix] == '{' ) { paren = '{'; dst = '}'; break; }
		if( blockText[ix] == '(' ) { paren = '('; dst = ')'; break; }
		if( blockText[ix] == '[' ) { paren = '['; dst = ']'; break; }
		if( blockText[ix] == '}' ) { paren = '}'; dst = '{'; forward = false; break; }
		if( blockText[ix] == ')' ) { paren = ')'; dst = '('; forward = false; break; }
		if( blockText[ix] == ']' ) { paren = ']'; dst = '['; forward = false; break; }
		++ix;
	}
	if( paren == QChar() ) return false;
	int count = 1;
	if( forward ) {
		for(;;) {
			while( ++ix < blockText.length() ) {
				if( blockText[ix] == paren )
					++count;
				else if( blockText[ix] == dst && !--count ) {
					cur.setPosition(blockPos + ix);
					return true;
				}
			}
			block = block.next();
			if( !block.isValid() )
				break;
			blockPos = block.position();
			blockText = block.text();
			ix = -1;
		}
	} else {
		for(;;) {
			while( --ix >= 0 ) {
				if( blockText[ix] == paren )
					++count;
				else if( blockText[ix] == dst && !--count ) {
					cur.setPosition(blockPos + ix);
					return true;
				}
			}
			block = block.prev();
			if( !block.isValid() )
				break;
			blockPos = block.position();
			blockText = block.text();
			ix = blockText.length();
		}
	}
	return false;
}
/**
	@return		カーソル移動した場合は true

	上下移動時はブロック先頭からのｘ変位が保存される。
		cur.block().layout().lineAt() で QTextLine を取得し、
		QTextLine::cursorToX(int) : qreal でx変位取得、
		QTextLine::xToCursor ( qreal x ) : int でｘ変位→カーソル位置取得
	
*/
bool moveCursor(ViewCursor &cur, //int &x,
				int mv, int n,
				bool cdy,		//	{c|d|y} が前置されている
				const ViewBlock *visibleBlock)
{
	const int pos = cur.position();
	ViewBlock block = cur.block();
	const int blockPos = block.position();
	const QString blockText = block.text();
	switch( mv ) {
	case ViMoveOperation::Up:
		mv = DocCursor::Up;
		break;
	case ViMoveOperation::Down:
		mv = DocCursor::Down;
		break;
	case ViMoveOperation::Left:
		n = qMin(n, pos - blockPos);
		mv = DocCursor::Left;
		break;
	case ViMoveOperation::Right: {
		if( blockText.isEmpty() ) return false;		//	改行 or EOF オンリー行の場合
		const int endpos = blockPos + blockText.length() - 1;
		//const int endpos = blockPos + EOLOffset(blockText) - 1;
		if( pos >= endpos ) return false;
		n = qMin(n, endpos - pos);
		mv = DocCursor::Right;
		break;
	}
	case ViMoveOperation::RightForA: {			//	右移動 for a コマンド
		if( blockText.isEmpty() ) return false;		//	改行 or EOF オンリー行の場合
		const int endpos = blockPos + blockText.length();
		//const int endpos = blockPos + EOLOffset(blockText);
		if( pos >= endpos ) return false;
		n = qMin(n, endpos - pos);
		mv = DocCursor::Right;
		break;
#if 0
		if( pos >= blockPos + EOLOffset(blockText)) return false;
		cur.setPosition(pos + 1);
		return true;
#endif
	}
#if 0
	case ViMoveOperation::FirstNonBlankChar:
		cur.setPosition(blockPos + firstNonBlankCharPos(blockText));
		return true;
#endif
	case ViMoveOperation::LastChar: {
		int ix = block.EOLOffset();
		//int ix = blockText.length();
		if( !cdy && ix != 0 ) --ix;
		cur.setPosition(blockPos + ix);
		///cur.setVerticalMovementX(-1);
		cur.setX(INT_MAX);
		return true;
	}
#if 0
	case ViMoveOperation::NextWord:
		return gotoNextWord(cur, n, cdy);
	case ViMoveOperation::PrevWord:
		return gotoPrevWord(cur, n);
	case ViMoveOperation::NextWordEnd:
		return gotoNextWordEnd(cur, n, cdy);
	case ViMoveOperation::NextSSWord:
		return gotoNextSSWord(cur, n, cdy);
#endif
	case ViMoveOperation::PrevSSWord:
		return gotoPrevSSWord(cur, n);
	case ViMoveOperation::NextSSWordEnd:
		return gotoNextSSWordEnd(cur, n, cdy);
	case ViMoveOperation::NextLine:
		cur.movePosition(DocCursor::NextBlock, DocCursor::MoveAnchor, n);
		moveToFirstNonBlankChar(cur);
		return true;
	case ViMoveOperation::PrevLine:
		cur.movePosition(DocCursor::PrevBlock, DocCursor::MoveAnchor, n);
		moveToFirstNonBlankChar(cur);
		return true;
	case ViMoveOperation::MatchParen:
		return gotoMatchParen(cur);
	case ViMoveOperation::NextBlankLine:
		return gotoBlankLine(cur, /*forward=*/true, n);
	case ViMoveOperation::PrevBlankLine:
		return gotoBlankLine(cur, /*forward=*/false, n);
	case ViMoveOperation::NextBeginBlock:
		return gotoBeginBlock(cur, /*forward=*/true, n);
	case ViMoveOperation::PrevBeginBlock:
		return gotoBeginBlock(cur, /*forward=*/false, n);
	case ViMoveOperation::TopOfView:
		block = *visibleBlock;
		while( --n > 0 ) {
			ViewBlock next = block.next();
			if( !next.isValid() ) break;
			block = next;
		}
		cur.setPosition(block.position() + firstNonBlankCharPos(block.text()));
		return true;
	case ViMoveOperation::BottomOfView:
		block = *visibleBlock;
		while( --n > 0 ) {
			ViewBlock next = block.prev();
			if( !next.isValid() ) break;
			block = next;
		}
		cur.setPosition(block.position() + firstNonBlankCharPos(block.text()));
		return true;
	case ViMoveOperation::JumpLine:
		block = block.document()->findBlockByNumber(n - 1);
		//block.setLineCount(n);
		if( block.isValid() ) {
			cur.setPosition(block.position() + firstNonBlankCharPos(block.text()));
			return true;
		}
		//	下にスルー
	case ViMoveOperation::LastLine:
		cur.movePosition(DocCursor::EndOfDocument);
		block = cur.block();
		if( block.text().isEmpty() 			//	空のEOF行の場合
			&& block.prev().isValid() )		//	前行が存在する場合
		{
			block = block.prev();
		}
		cur.setPosition(block.position() + firstNonBlankCharPos(block.text()));
		return true;
	}
	cur.movePosition(static_cast<uchar>(mv), DocCursor::MoveAnchor, n);
	return true;
}

bool moveCursorFindInLine(ViewCursor &cur, ushort cmd, const QChar &qch, int n)
{
	ViewBlock block = cur.block();
	QString text = block.text();
	int ix = cur.position() - block.position();
	while( --n >= 0 ) {
		for(;;) {
			if( cmd == 'f' || cmd == 't' ) {
				if( ix == text.length() )
					return false;	//	n番目の文字が発見出来なかった場合はカーソル移動しない
				++ix;
			} else {
				if( !ix )
					return false;	//	n番目の文字が発見出来なかった場合はカーソル移動しない
				--ix;
			}
			if( text[ix] == qch )
				break;
		}
	}
	if( cmd == 't' )
		--ix;
	else if( cmd == 'T' )
		++ix;
	cur.setPosition(block.position() + ix);
	return true;
}

int moveCursorFindForward(const QRegExp &rex, ViewBlock &block, int ix, int &nth, int limit)
{
	for(;;) {
		int i = rex.indexIn(block.text(), ix);
		if( i >= 0 ) {
			if( !--nth ) {
				int pos = i + block.position();
				if( limit < 0 || pos <= limit ) {		//	初期カーソル位置に一周して戻ってきた場合もOK
					return pos;
				} else
					return -1;
			}
			ix = i + 1;
		} else {
			ix = 0;
			block = block.next();
			if( !block.isValid() || limit >= 0 && block.position() > limit )
				return -1;
		}
	}
}
int moveCursorFindBackward(const QRegExp &rex, ViewBlock &block, int ix, int &nth, int limit)
{
	int i;
	for(;;) {
		if( ix >= 0 && (i = rex.lastIndexIn(block.text(), ix)) >= 0 ) {	//	ix = -1 を指定すると最後から検索してしまう
			if( !--nth ) {
				int pos = i + block.position();
				if( limit < 0 || pos >= limit ) {		//	初期カーソル位置に一周して戻ってきた場合もOK
					return pos;
				} else
					return -1;
			}
			ix = i - 1;
		} else {
			block = block.prev();
			if( !block.isValid() || limit >= 0 && block.position() < limit )
				return -1;
			ix = block.text().length();
		}
	}
}
bool moveCursorFind(ViewCursor &cur, const QRegExp &rex, bool forward, int nth)
{
	ViewBlock block = cur.block();
	int curPos = cur.position();
	int ix = curPos - block.position();
	int pos;
	if( forward ) {
		++ix;
		if( (pos = moveCursorFindForward(rex, block, ix, nth, -1)) >= 0 ) {
			cur.setPosition(pos);
			return true;
		}
		//	文章末尾までに見つからなかったら、文書先頭から検索
		block = cur.view()->firstBlock();
		ix = 0;
		if( (pos = moveCursorFindForward(rex, block, ix, nth, curPos)) >= 0 ) {
			cur.setPosition(pos);
			return true;
		}
	} else {
		--ix;
		if( (pos = moveCursorFindBackward(rex, block, ix, nth, -1)) >= 0 ) {
			cur.setPosition(pos);
			return true;
		}
		//	文章先頭までに見つからなかったら、文書末尾から検索
		block = block.view()->lastBlock();
		ix = block.text().length();
		if( (pos = moveCursorFindBackward(rex, block, ix, nth, curPos)) >= 0 ) {
			cur.setPosition(pos);
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------
#if 0
static uchar sbCharTypeTbl[] = {
/* 0 */	CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER,
		CT_OTHER, CT_SPACE, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER,
/* 1 */	CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER,
		CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER,
/* 2 */	CT_SPACE, CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,		//	  ! " # $ % & '
		CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,		//	( ) * + , - . /
/* 3 */	CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM,		//	0 CT_ALNUM 2 3 4 5 6 7
		CT_ALNUM, CT_ALNUM, CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,		//	8 9 : ; < = > ?
/* 4 */	CT_SYM,   CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM,		//	@ A B C D E F G
		CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM,		//	H I J K L M N O
/* 5 */	CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM,		//	P Q R S T U V W
		CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_ALNUM,		//	X Y Z [ \ ] ^ _
/* 6 */	CT_SYM,   CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM,		//	` a b c d e f g
		CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM,		//	h i j k l m n o
/* 7 */	CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_ALNUM,		//	p q r s t u v w
		CT_ALNUM, CT_ALNUM, CT_ALNUM, CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,   CT_SYM,		//	x y z { | } ~
};

uchar getCharType(QChar ch)
{
	const ushort code = ch.unicode();
	if( code < 0x80 )
		return sbCharTypeTbl[code];
	if( code <= 0xa0 ) return CT_OTHER;
	if( code <= 0xbf ) return CT_SYM;
	if( code <= 0x2af ) return CT_ALNUM;
	if( code <= 0x36f ) return CT_GREEK;
	if( code >= 0x3040 && code < 0x30a0 ) return CT_HIRA;
	if( code >= 0x30a0 && code < 0x3100 ) return CT_KANA;
	if( code >= 0x4e00 && code < 0xa000 ) return CT_KANJI;
	return CT_SYM;
}
#endif

//----------------------------------------------------------------------

#if 0
ViCursor::ViCursor()
	: m_nthLine(0)
{
}
ViCursor::ViCursor(const ViCursor &cur)
	: ViewCursor(cur), m_nthLine(cur.m_nthLine)
{
}
ViCursor::ViCursor(const ViewCursor &cur, int nthLine)
	: ViewCursor(cur), m_nthLine(nthLine)
{
}
#if 0
bool ViCursor::movePosition(MoveOperation operation, MoveMode mode, int n)
{
	return QTextCursor::movePosition(operation, mode, n);
}
#endif

bool ViCursor::movePosition(
				int mv,			//	移動種別
				MoveMode mode,	//	アンカー移動 or キープ
				int n,			//	繰り返し回数
				bool cdy,		//	{c|d|y} が前置されている
				const QTextBlock *visibleBlock)		//	画面最上行 or 最下行
{
#if 0
	if( mv < Left ) {
		QTextCursor::movePosition(mv
	}
#endif
	const int pos = DocCursor::position();
	QTextBlock block = DocCursor::block();
	const int blockPos = block.position();
	const QString blockText = block.text();
	switch( mv ) {
	case ViMoveOperation::Up:
		for(int i = 0; i < n; ++i) {
			if( !block.previous().isValid() ) break;
			block = block.previous();
		}
		//if( !(block = block.previous()).isValid() ) return false;
		moveXAtNthLine(block);
		//QTextCursor::movePosition(QTextCursor::Up, QTextCursor::MoveAnchor, n);
		if( !cdy ) moveLeftIfEndOfLine();
		return true;
	case ViMoveOperation::Down:
		for(int i = 0; i < n; ++i) {
			if( !block.next().isValid() ) break;
			block = block.next();
		}
		//if( !(block = block.next()).isValid() ) return false;
		moveXAtNthLine(block);
		//QTextCursor::movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, n);
		if( !cdy ) moveLeftIfEndOfLine();
		return true;
	case ViMoveOperation::Left:
		n = qMin(n, pos - blockPos);
		mv = DocCursor::Left;
		break;
	case ViMoveOperation::Right: {
		if( blockText.isEmpty() ) return false;		//	改行 or EOF オンリー行の場合
		const int endpos = blockPos + blockText.length() - 1;
		if( pos >= endpos ) return false;
		n = qMin(n, endpos - pos);
		mv = DocCursor::Right;
		break;
	}
	case ViMoveOperation::JumpLine:
		block = block.document()->findBlockByNumber(n - 1);
		if( block.isValid() ) {
			setPosition(block.position() + firstNonBlankCharPos(block.text()));
			return true;
		}
		//	下にスルー
	case ViMoveOperation::LastLine:
		DocCursor::movePosition(DocCursor::End);
		block = DocCursor::block();
		if( block.text().isEmpty() 			//	空のEOF行の場合
			&& block.previous().isValid() )		//	前行が存在する場合
		{
			block = block.previous();
		}
		setPosition(block.position() + firstNonBlankCharPos(block.text()));
		return true;
	case ViMoveOperation::PrevWord:
		return gotoPrevWord(*this, n);
	case ViMoveOperation::FirstNonBlankChar:
		setPosition(blockPos + firstNonBlankCharPos(blockText), mode);
		return true;
	}
	DocCursor::movePosition(static_cast<DocCursor::MoveOperation>(mv), mode, n);
	//QTextLayout layout = block().layout();
	//QTextLine textLine = QTextCursor::block().layout()->lineForTextPosition(position());
	//m_nthLine = textLine.lineNumber();
	calcNthLine();
	qDebug() << "lineNumber = " << m_nthLine;

	return true;
}

void ViCursor::moveXAtNthLine(const QTextBlock &block)
{
	int x = verticalMovementX();
	const QTextLayout *layout = block.layout();
	if( x < 0 ||		//	$ の場合は -1 がセットされている
		m_nthLine >= layout->lineCount() )
	{
		setPosition(block.position() + block.text().length());
	} else {
		QTextLine textLine = layout->lineAt(m_nthLine);
		setPosition(block.position() + textLine.xToCursor(x));
	}
	setVerticalMovementX(x);	//	VerticalMovementX の値が変化しているので再設定
}

int ViCursor::calcNthLine()
{
#if 1
	const QTextLayout *layout = DocCursor::block().layout();
	const QTextLine textLine = layout->lineForTextPosition(positionInBlock());
	return m_nthLine = textLine.lineNumber();
#else
	const int pos = position();
	const QTextLayout *layout = QTextCursor::block().layout();
	for(int i = 0; i < layout->lineCount(); ++i) {
		const QTextLine textLine = layout->lineAt(i);
		if( pos < textLine.textStart() + textLine.textLength() )
			return m_nthLine = i;
	}
	return m_nthLine = 0;
#endif
}

void ViCursor::moveLeftIfEndOfLine()
{
	QTextBlock block = DocCursor::block();
	const int length = block.text().length();
	if( length != 0 && position() == block.position() + length ) {
		const int x = verticalMovementX();	//	x 変異を保存
		DocCursor::movePosition(DocCursor::Left);
		setVerticalMovementX(x);
	}
}
#endif
