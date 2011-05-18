//----------------------------------------------------------------------
//
//			File:			"ViEngine.cpp"
//			Created:		24-Feb-2011
//			Author:			Nobuhide Tsuda
//			Description:	viエンジンクラス実装
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
#include "vi.h"
#include "ViEngine.h"
#include "TextView.h"
#include "TextDocument.h"
#include "viCursor.h"

bool hasSelection(const std::vector<ViewCursor*> &v);
int getEOLOffset(const QString text);

#if 1
ViEngine::ViEngine(QObject *parent)
	: m_noInsModeAtImeOpenStatus(false), m_mode(CMD), m_editor(0)
	, m_redoRecording(false), m_redoing(false), m_redoRepeatCount(0)
	, m_noRepeatCount(false), m_joinPrevEditBlock(false)
	, m_moveByLine(false)
	, m_cmdPrefix(0), m_cdyCmd(0)	//(0)
	, m_repeatCount(0), m_repeatCount2(0), QObject(parent)
{
    QSettings settings;
    m_exCommands = settings.value("recentExCmdList").toStringList();
}

ViEngine::~ViEngine()
{

}
TextDocument *ViEngine::document()
{
	return editor()->document();
}
void ViEngine::setEditor(TextView *editor)
{
	m_editor = editor;
#if USE_EVENT_FILTER
	m_editor->installEventFilter(this);
#endif
}
void ViEngine::setMode(Mode mode, ushort subMode)
{
	if( mode != m_mode ) {
		const bool fromCMDLINE = m_mode == CMDLINE;
		m_mode = mode;
		m_noInsModeAtImeOpenStatus = true;
		emit modeChanged(mode, subMode);
		m_noInsModeAtImeOpenStatus = false;
		if( fromCMDLINE )
			m_editor->setFocus();
	}
}
#if 1
bool ViEngine::eventFilter(QObject *obj, QEvent *event)
{
	if( obj == m_editor ) {
		if( event->type() == QEvent::KeyPress ) {
			return processKeyPressEvent(static_cast<QKeyEvent *>(event));
#if 0
			switch( mode() ) {
			case CMD:
				return cmdModeKeyPressEvent(static_cast<QKeyEvent *>(event));	//	コマンドモードの場合の処理
			case INSERT:
				return insModeKeyPressEvent(static_cast<QKeyEvent *>(event));	//	挿入モードの場合の処理
			}
#endif
		//		マウスクリックしても条件文が成立しない 11/02/12
		//} else if( event->type() == QEvent::MouseButtonPress ) {
		//	m_redoRecording = false;
		}
	}
	return false;
}
#else
bool ViEngine::processKeyPressEvent( QKeyEvent * event )
{
	switch( mode() ) {
	case CMD:
		return cmdModeKeyPressEvent(event);	//	コマンドモードの場合の処理
	case INSERT:
		return insModeKeyPressEvent(event);	//	挿入モードの場合の処理
	}
	return false;
}
#endif

void ViEngine::printCursorPosition()
{
	QString mess;
	ViewCursor cur = m_editor->textCursor();
	ViewBlock block = cur.block();
	if( block.isValid() ) {
		mess = QString(tr("pos = %1 block.length() = %2 textLen = %3"))
				.arg(cur.position()).arg(block.length()).arg(block.text().length());
	} else
		mess = tr("block is NOT valid.");
	emit showMessage(mess);
}

//	次行先頭の空白類は削除
//	前行末尾に空白類が無い場合のみ、半角空白を挿入
void ViEngine::doJoin(int n)
{
	ViewCursor cur = m_editor->textCursor();
	int pos = cur.position();
	document()->openUndoBlock();
	//cur.beginEditBlock();
	do {
		ViewBlock block = cur.block();
		ViewBlock next = block.next();
		if( !next.isValid() ) break;
		index_t eo = block.EOLOffset();
		cur.movePosition(DocCursor::EndOfBlock);
		cur.movePosition(ViMoveOperation::NextLine, DocCursor::KeepAnchor);
		QChar lastChar;
		if( eo != 0 ) {
			const QString text = block.text();
			lastChar = text.at(getEOLOffset(text) - 1);
		}
		if( lastChar == ' ' || lastChar == '\t' )
			cur.deleteChar();
		else
			cur.insertText(" ");
		//cur.movePosition(DocCursor::Right);	//	空白位置に移動
	} while( --n > 1 );		//	2J までは１回だけjoin
	document()->closeUndoBlock();
	//cur.endEditBlock();
	//cur.setPosition(pos);	//	最後の空白位置に移動
	//m_editor->textCursor().setPosition(cur.position());
}
bool ViEngine::doShiftRight(ViewCursor &cur, int n)
{
	int pos = cur.position();
	document()->openUndoBlock();
	//cur.beginEditBlock();
	ViewBlock block = cur.block();
	do {
		cur.setPosition(block.position());
		cur.insertText("\t");
		if( !--n ) break;
		block = block.next();
	} while( block.isValid() );
	cur.setPosition(pos);
	cur.movePosition(ViMoveOperation::FirstNonBlankChar);
	m_editor->setTextCursor(cur);
	document()->closeUndoBlock();
	//cur.endEditBlock();
	m_editor->viewport()->update();
	return true;
}
bool ViEngine::doShiftLeft(ViewCursor &cur, int n)
{
	int pos = cur.position();
	document()->openUndoBlock();
	//cur.beginEditBlock();
	ViewBlock block = cur.block();
	do {
		const QString text = block.text();
		if( text.length() != 0 ) {
			int ix = 0;
			if( text[0] == '\t' ) ix = 1;
			else
				while( ix < 4 && ix < text.length() && text[ix] == ' ' )
					++ix;
			if( ix != 0 ) {
				cur.setPosition(block.position());
				cur.setPosition(block.position() + ix, DocCursor::KeepAnchor);
				cur.deleteChar();
			}
		}
		if( !--n ) break;
		block = block.next();
	} while( block.isValid() );
	cur.setPosition(pos);
	cur.movePosition(ViMoveOperation::FirstNonBlankChar);
	m_editor->setTextCursor(cur);
	document()->closeUndoBlock();
	//cur.endEditBlock();
	m_editor->viewport()->update();
	return true;
}

int ViEngine::repeatCount() const
{
	int rc2 = !m_repeatCount2 ? 1 : m_repeatCount2;
	if( !m_repeatCount ) {
		if( m_redoing && m_redoRepeatCount != 0 )
			return m_redoRepeatCount;
		return rc2;
	}
	return m_repeatCount * rc2;
}
//	block から n 行下の先頭位置を返す
int nLinesPosition(DocCursor cur, int n)
{
	DocBlock block = cur.block();
	while( --n >= 0 ) {
		block = block.next();
		if( !block.isValid() ) {
			cur.movePosition(DocCursor::EndOfDocument);
			return cur.position();
		}
	}
	return block.position();
}
void ViEngine::doViCommand(const QString &text)
{
	for(int ix = 0; ix < text.length(); ++ix)
		doViCommand(text[ix]);
}
/**
	@return		コマンドとして処理した場合は true
*/
bool ViEngine::doViCommand(const QChar &qch)
{
	ushort ch = qch.unicode();
	int op = 0;
	switch( ch ) {
	case 'F' - '@':
		op = ViScrollOperation::NextPage;
		break;
	case 'B' - '@':
		op = ViScrollOperation::PrevPage;
		break;
	case 'D' - '@':
		op = ViScrollOperation::NextHalfPage;
		break;
	case 'U' - '@':
		op = ViScrollOperation::PrevHalfPage;
		break;
	case 'E' - '@':
		op = ViScrollOperation::ExposeBottom;
		break;
	case 'Y' - '@':
		op = ViScrollOperation::ExposeTop;
		break;
	}
	if( op != 0 ) {
		m_editor->doVertScroll(op);
		return true;
	}
	std::vector<ViewCursor*> v;			//	メインカーソルも含めたカーソル一覧（昇順ソート済み）
	m_editor->getAllCursor(v);
	if( qch == '\t' && m_editor->hasMultiCursor() && hasSelection(v) ) {
		//	選択領域がある場合はローテイト
		document()->openUndoBlock();
		m_editor->rotateSelectedText(v);
		document()->closeUndoBlock();
		return true;
	}
	if( qch.unicode() == 0x1b ) {	//	Esc
		m_editor->clearMultiCursor();
		m_editor->clearSelection();
		document()->closeUndoBlock();
#if	0	//def	_DEBUG
		const ViewCursor cur = m_editor->textCursor();
		const LaidoutBlock *lb = m_editor->lbMgr()->cacheBlock();
		QString mess = QString("%1 cur=(p=%2 d.i=%3 d.p=%4 v.i=%5 v.p=%6 x=%7) blockData=(%8 %9) cache=(d.i=%10 d.p=%11 v.i=%12 v.p=%13)\n")
							.arg(QDir::currentPath())
							.arg(cur.position())
							.arg(cur.blockData().index())
							.arg(cur.blockData().position())
							.arg(cur.viewBlockData().index())
							.arg(cur.viewBlockData().position())
							.arg(cur.x())
							.arg(document()->cacheBlockData().index())
							.arg(document()->cacheBlockData().position())
							.arg(lb->docIndex())
							.arg(lb->docPosition())
							.arg(lb->index())
							.arg(lb->position());
		doOutput(mess);
#endif
	}
	if( mode() == INSERT || mode() == REPLACE ) {
		//	done A insModeKeyPressEvent() の部分と処理を共通化する
		if( qch.unicode() == 0x1b ) {	//	Esc
			ViewCursor cur = m_editor->textCursor();
			if( m_redoRecording ) {
				//	挿入中にカーソル移動していない＆繰り返し回数指定がある場合は、残り回数分の文字挿入
#if 1
				//	回数R の場合でも、２回目以降は挿入処理する（nvi と同じ仕様）
				for(int i = 1; i < m_insCount; ++i) {
					if( m_redoCmd == "o" || m_redoCmd == "O" )
						cur.insertText("\n");
					cur.insertText(m_insertedText);
				}
#else
				if( mode() == INSERT ) {
					for(int i = 1; i < m_insCount; ++i) {
						if( m_redoCmd == "o" || m_redoCmd == "O" )
							cur.insertText("\n");
						cur.insertText(m_insertedText);
					}
				} else {
					for(int i = 1; i < m_insCount; ++i) {
						cur.movePosition(DocCursor::Right, DocCursor::KeepAnchor,
											m_insertedText.length());
						cur.insertText(m_insertedText);
					}
				}
#endif
			}
			moveCursor(cur, ViMoveOperation::Left);
			m_editor->setTextCursor(cur);
			m_editor->setOverwriteMode(false);
			m_joinPrevEditBlock = false;
			m_redoRecording = false;
			m_insCount = 0;
			document()->setDoNotMergeUndoItem();
			setMode(CMD);
		} else {
			if( ch == 0x08 ) {		//	BackSpace
				if( !m_insertedText.isEmpty() )
					m_insertedText = m_insertedText.left(m_insertedText.length() - 1);
				return false;
			} else if( ch < ' ' && ch != '\r' && ch != '\n' && ch != '\t' ) {
				m_redoRecording = false;
				m_joinPrevEditBlock = false;
			} else
				m_insertedText += qch;
			ViewCursor cur = m_editor->textCursor();
			if( mode() == REPLACE && !cur.atBlockEnd() )
				cur.movePosition(DocCursor::Right, DocCursor::KeepAnchor);
#if 0
			if( m_joinPrevEditBlock ) {
				///cur.joinPreviousEditBlock();
				cur.insertText(qch);
				document()->closeUndoBlock();
				//cur.endEditBlock();
			} else {
#endif
				//cur.insertText(qch);
				if( qch == '\n' || qch == '\r' ) {
					//	undone B 文書の改行コードを挿入
					cur.insertText(document()->EOLText());
				} else
					cur.insertText(qch);
			//}
			m_editor->setTextCursor(cur);
		}
		return true;
	}
	m_cmdString += qch;
	//showMessage(m_cmdString);
	if( !m_noRepeatCount ) {
		if( !m_cdyCmd ) {
			if( ch == '0' && m_repeatCount != 0 || ch >= '1' && ch <= '9' ) {
				m_repeatCount = m_repeatCount * 10 + (ch - '0');
				emit showMessage(m_cmdString);
				return true;
			}
		} else {
			if( ch >= '0' && ch <= '9' ) {
				m_repeatCount2 = m_repeatCount2 * 10 + (ch - '0');
				emit showMessage(m_cmdString);
				return true;
			}
		}
	}
	//bool rc = true;
	ViewCursor cur = m_editor->textCursor();
	///ViewCursor cur = m_editor->viCursor();
	ViewBlock block = cur.block();
	bool toUpdateRedoCmd = false;
	bool cursorMoved = false;
	bool toInsertMode = false;
	bool toReplaceMode = false;
	bool toYankByLine = false;
	bool findInLineForward = false;		//	f t or (; ,) で前方検索
	int toMovePos = -1;
	int delFrom = -1;
	int delTo = -1;
	int yankFrom = -1;
	int yankTo = -1;

	if( m_cmdPrefix != 0 ) {
		switch( m_cmdPrefix ) {
		case 'g':
			if( ch == 'g' ) {
				cursorMoved = cur.movePosition(ViMoveOperation::JumpLine);
				m_moveByLine = true;
			}
			break;
		case 'r': {
			QString t;
			for(int i = repeatCount(); --i >= 0; ) {
				if( !cur.atBlockEnd() )
					cur.movePosition(ViewCursor::Right, ViewCursor::KeepAnchor, 1);
				t += ch;
			}
			if( t.isEmpty() )	//	改行位置にいた場合
				cur.insertText(QString(ch));
			else
				cur.insertText(t);
			m_redoCmd = QString("r") + qch;
			m_redoRepeatCount = repeatCount();
			break;
		}
		case '>':
			if( ch == '>' ) {
				doShiftRight(cur, repeatCount());
				m_redoCmd = ">>";
				m_redoRepeatCount = repeatCount();
			}
			break;
		case '<':
			if( ch == '<' ) {
				cursorMoved = doShiftLeft(cur, repeatCount());
				m_redoCmd = "<<";
				m_redoRepeatCount = repeatCount();
			}
			break;
		case ']':
			if( ch == ']' )
				cursorMoved = moveCursor(cur, ViMoveOperation::NextBeginBlock, repeatCount());
			break;
		case '[':
			if( ch == '[' )
				cursorMoved = moveCursor(cur, ViMoveOperation::PrevBeginBlock, repeatCount());
			break;
		case 'f':
		case 'F':
		case 't':
		case 'T':
			m_lastFTCmd = m_cmdPrefix;		//	マッチするかどうかに関係なく記録
			m_lastFTChar = qch;
			cursorMoved = moveCursorFindInLine(cur, m_cmdPrefix, qch, repeatCount());
			break;
		case 'z':
			if( ch == '\r' || ch == '\n' ) {
				cur = m_editor->textCursor();
				m_editor->doVertScroll(ViScrollOperation::Set, cur.blockNumber());
			}
			break;
		case 'Z':
			if( ch == 'Z' ) {
				emit save(QString());
				emit closeView(m_editor);
			}
			break;
		}
	} else if( m_cdyCmd != 0 && m_cdyCmd == ch ) {
		switch( ch ) {
		case 'c':		//	cc
			m_editor->doDelete(cur.block().position(), nLinesPosition(cur, repeatCount()));
			m_editor->doOpenLine(/*next = */false);
			cur = m_editor->textCursor();
			toInsertMode = true;
			toUpdateRedoCmd = true;
			m_repeatCount = 1;		//	繰り返し回数は削除行数
			//delFrom = cur.position();
			//delTo = block.position() + block.text().length();
			break;
		case 'd':		//	dd
			delFrom = cur.block().position();
			delTo = nLinesPosition(cur, repeatCount());
			toYankByLine = true;		//	for m_yankByLine;
			break;
		case 'y':		//	yy
			yankFrom = cur.block().position();
			yankTo = nLinesPosition(cur, repeatCount());
			toYankByLine = true;		//	for m_yankByLine;
			break;
		}
	} else {
		const bool cdy = m_cdyCmd != 0;
		switch( ch ) {
		case 'f':
		case 'F':
		case 't':
		case 'T':
		case ']':
		case '[':
		case 'g':
		case 'r':
		case 'z':
		case 'Z':
			m_noRepeatCount = true;
			//	下にスルー
		case '>':
		case '<':
			m_cmdPrefix = ch;
			emit showMessage(m_cmdString);
			return true;
		case 'c':
		case 'd':
		case 'y':
			if( cur.hasSelection() ) {
				if( ch == 'y' ) {
					yankFrom = qMin(cur.anchor(), cur.position());
					yankTo = qMax(cur.anchor(), cur.position());
					break;
				}
				if( ch == 'c' ) {
					document()->openUndoBlock();
					toInsertMode = true;
				}
				delFrom = qMin(cur.anchor(), cur.position());
				delTo = qMax(cur.anchor(), cur.position());
				break;
			}
			m_cdyCmd = ch;
			m_cdyPos = cur.position();
			emit showMessage(m_cmdString);
			return true;
		case ';':
			if( m_lastFTChar != QChar() ) {
				findInLineForward = m_lastFTCmd == 'f' || m_lastFTCmd == 't';
				cursorMoved = moveCursorFindInLine(cur, m_lastFTCmd, m_lastFTChar, repeatCount());
			}
			break;
		case ',':
			if( m_lastFTChar != QChar() ) {
				findInLineForward = m_lastFTCmd == 'F' || m_lastFTCmd == 'T';
				const ushort cmd = m_lastFTCmd ^ 'F' ^ 'f';		//	f,F t,T 反転
				cursorMoved = moveCursorFindInLine(cur, cmd, m_lastFTChar, repeatCount());
			}
			break;
		case '0':
			cursorMoved = moveCursor(cur, DocCursor::StartOfBlock);
			break;
		case '^':
			cursorMoved = cur.movePosition(ViMoveOperation::FirstNonBlankChar);
			break;
		case '$':
			if( cdy )
				cursorMoved = cur.movePosition(DocCursor::EndOfBlock);
			else
				cursorMoved = cur.movePosition(ViMoveOperation::LastChar);
			break;
		case '%':
			cursorMoved = moveCursor(cur, ViMoveOperation::MatchParen);
			break;
		case '}':
			cursorMoved = moveCursor(cur, ViMoveOperation::NextBlankLine, repeatCount());
			m_moveByLine = true;
			break;
		case '{':
			cursorMoved = moveCursor(cur, ViMoveOperation::PrevBlankLine, repeatCount());
			m_moveByLine = true;
			break;
		case 'H': 
			cursorMoved = moveCursor(cur, ViMoveOperation::TopOfView, repeatCount(),
										cdy, m_editor->firstVisibleBlockPtr());
			m_moveByLine = true;
			break;
		case 'L': 
			cursorMoved = moveCursor(cur, ViMoveOperation::BottomOfView, repeatCount(),
										cdy, m_editor->lastVisibleBlockPtr());
			m_moveByLine = true;
			break;
		case 'M': {
			const int n = (m_editor->firstVisibleBlockPtr()->blockNumber() + 
							m_editor->lastVisibleBlockPtr()->blockNumber()) / 2 + 1;
			cursorMoved = cur.movePosition(ViMoveOperation::JumpLine, DocCursor::MoveAnchor, n);
			m_moveByLine = true;
			break;
		}
		case 'G':
			if( !m_repeatCount )
				cursorMoved = moveCursor(cur, ViMoveOperation::LastLine);
			else
				cursorMoved = cur.movePosition(ViMoveOperation::JumpLine, DocCursor::MoveAnchor, repeatCount());
			m_moveByLine = true;
			break;
		case '/':
		case '?':
		case ':':
			//m_cmdString = qch;
			m_message.clear();
			setMode(CMDLINE, ch);	//	: が押されたらexモードへ
			break;
		case 'a':
			cursorMoved = moveCursor(cur, ViMoveOperation::RightForA);
			toInsertMode = true;
			break;
		case 'i':
			toInsertMode = true;
			break;
		case 'A':
			cursorMoved = cur.movePosition(DocCursor::EndOfBlock);
			toInsertMode = true;
			break;
		case 'I':
			cursorMoved = cur.movePosition(ViMoveOperation::FirstNonBlankChar);
			toInsertMode = true;
			break;
#if 1
		case 'o':
		case 'O':
			//cur.beginEditBlock();
			m_joinPrevEditBlock = true;
			m_editor->doOpenLine(/*next=*/ch == 'o');
			cur = m_editor->textCursor();
			//cur.endEditBlock();
			toInsertMode = true;
			break;
#else
		case 'o':
			//	done C 同時にundo可能にする
			cur.beginEditBlock();
			//m_editor->setTextCursor(cur);
			m_joinPrevEditBlock = true;
			m_editor->doOpenLine();
			//m_editor->viewport()->update();
			cur = m_editor->textCursor();
			cur.endEditBlock();
			toInsertMode = true;
			break;
		case 'O':
			//	done C 同時にundo可能にする
			cur.beginEditBlock();
			m_joinPrevEditBlock = true;
			m_editor->doOpenLine(/*next = */false);
			cur = m_editor->textCursor();
			cur.endEditBlock();
			toInsertMode = true;
			break;
#endif
		case 'S':
			//	done C 同時にundo可能にする
			document()->openUndoBlock();
			m_editor->doDelete(cur.block().position(), nLinesPosition(cur, repeatCount()));
			m_editor->doOpenLine(/*next = */false);
			cur = m_editor->textCursor();
			toInsertMode = true;
			toUpdateRedoCmd = true;
			m_repeatCount = 1;		//	繰り返し回数は削除行数
			break;
		case 'R':
			m_redoCmd = "R";
			toReplaceMode = true;
			break;
		case 0x08:		//	BackSpace
		case 'h':
			cursorMoved = cur.movePosition(ViMoveOperation::Left, DocCursor::MoveAnchor, repeatCount());
			break;
		case ' ':
		case 'l':
			cursorMoved = cur.movePosition(ViMoveOperation::Right, DocCursor::MoveAnchor, repeatCount());
			break;
		case 'k':
			cursorMoved = cur.movePosition(ViMoveOperation::Up, DocCursor::MoveAnchor, repeatCount());
			break;
		case 'j':
			cursorMoved = cur.movePosition(ViMoveOperation::Down, DocCursor::MoveAnchor, repeatCount());
			break;
		case 'b':
			cursorMoved = cur.movePosition(ViMoveOperation::PrevWord, DocCursor::MoveAnchor, repeatCount());
			break;
		case 'e':
			cursorMoved = cur.movePosition(ViMoveOperation::NextWordEnd, DocCursor::MoveAnchor, repeatCount(), cdy);
			break;
		case 'w':
			cursorMoved = cur.movePosition(ViMoveOperation::NextWord, DocCursor::MoveAnchor, repeatCount(), cdy);
			break;
		case 'B':
			cursorMoved = cur.movePosition(ViMoveOperation::PrevSSWord, DocCursor::MoveAnchor, repeatCount());
			break;
		case 'E':
			cursorMoved = cur.movePosition(ViMoveOperation::NextSSWordEnd, DocCursor::MoveAnchor, repeatCount(), cdy);
			break;
		case 'W':
			cursorMoved = cur.movePosition(ViMoveOperation::NextSSWord, DocCursor::MoveAnchor, repeatCount(), cdy);
			break;
		case '\r':		//	Enter
		case '+':
			cursorMoved = cur.movePosition(ViMoveOperation::NextLine, DocCursor::MoveAnchor, repeatCount());
			break;
		case '-':
			cursorMoved = cur.movePosition(ViMoveOperation::PrevLine, DocCursor::MoveAnchor, repeatCount());
			break;
		case 'n':
			cursorMoved = doFindNext(cur, repeatCount());
			break;
		case 'N':
			cursorMoved = doFindNext(cur, repeatCount(), /*reverse=*/true);
			break;
		case 'J':
			doJoin(repeatCount());
			toUpdateRedoCmd = true;
			break;
		case 'C':
			toInsertMode = true;
			//	下にスルー
		case 'D': {
			delFrom = cur.position();
			//const ViewBlock block = cur.block();
			delTo = block.position() + block.EOLOffset();
			toUpdateRedoCmd = true;
			break;
		}
		case 's':
			if( cur.hasSelection() ) {
				delFrom = qMin(cur.anchor(), cur.position());
				delTo = qMax(cur.anchor(), cur.position());
			} else {
				delFrom = cur.position();
				moveCursor(cur, ViMoveOperation::RightForA, repeatCount());
				delTo = cur.position();
			}
			document()->openUndoBlock();
			toInsertMode = true;
			m_repeatCount = 1;		//	<数値>s text Esc で text を数値回数挿入しませんように
			break;
		case 'x':
			if( cur.hasSelection() ) {
				delFrom = qMin(cur.anchor(), cur.position());
				delTo = qMax(cur.anchor(), cur.position());
			} else {
				delFrom = cur.position();
				cur.movePosition(ViMoveOperation::RightForA, DocCursor::KeepAnchor, repeatCount());
				//moveCursor(cur, ViMoveOperation::RightForA, repeatCount());
				delTo = cur.position();
			}
			break;
		case 'X':
			if( cur.hasSelection() ) {
				delFrom = cur.anchor();
				delTo = cur.position();
				//delFrom = qMin(cur.anchor(), cur.position());
				//delTo = qMax(cur.anchor(), cur.position());
			} else {
				delFrom = cur.position();
				moveCursor(cur, ViMoveOperation::Left, repeatCount());
				delTo = cur.position();
			}
			break;
		case 'Y':		//	行ヤンク
			yankFrom = cur.block().position();
			yankTo = nLinesPosition(cur, repeatCount());
			toYankByLine = true;
			break;
		case 'p':		//	カーソル直後・次行にペースト
			if( m_yankBuffer.isEmpty() ) break;
			if( m_yankByLine ) {
				if( block.next().isValid() )
					cur.setPosition(block.next().position());
				else if( !block.text().isEmpty() ) {
					//	空でないEOF行の場合
					cur.movePosition(DocCursor::EndOfDocument);
					cur.insertText("\n");
				}
			} else {
				if( !cur.atBlockEnd() )
					cur.movePosition(DocCursor::Right);
			}
			toMovePos = cur.position();
			cur.insertText(yankText());
			cur.setPosition(toMovePos);
			m_editor->setTextCursor(cur);
			break;
		case 'P':		//	カーソル位置・行直前にペースト
			if( m_yankBuffer.isEmpty() ) break;
			if( m_yankByLine )
				cur.setPosition(block.position());
			toMovePos = cur.position();
			cur.insertText(yankText());
			cur.setPosition(toMovePos);
			m_editor->setTextCursor(cur);
			break;
		case '~':		//	英大文字小文字置換
			cur.movePosition(DocCursor::Right, DocCursor::KeepAnchor, repeatCount());
			if( cur.hasSelection() ) {
				QString text = cur.selectedText();
				for(int i = 0; i < text.length(); ++i) {
					if( text[i].isUpper() )
						text[i] = text[i].toLower();
					else if( text[i].isLower() )
						text[i] = text[i].toUpper();
				}
				cur.insertText(text);
			}
			break;
		case 'u':
			m_editor->doUndo(repeatCount(), true);
			break;
		case 'U':
			m_editor->doRedo(repeatCount());
			break;
		case '.':
			if( !m_redoCmd.isEmpty() ) {
				m_redoing = true;
				doViCommand(m_redoCmd);
				m_redoing = false;
			}
			break;
		case ('G' - '@'):
			printCursorPosition();
			break;
		case 0x1b:		//	Esc
			emit showMessage("CMD");
			break;
		default:
			//rc = false;
			break;
		}
	}
	if( cursorMoved ) {
		//if( m_cdyCmd && (ch == 'e' || ch == 'E') )
		//	cur.movePosition(DocCursor::Right);
		switch( m_cdyCmd ) {
		case 'c':
			document()->openUndoBlock();
			m_joinPrevEditBlock = true;
			toInsertMode = true;
			//	下にスルー
		case 'd':
			delFrom = m_cdyPos;
			delTo = cur.position();
			if( m_cmdPrefix == 'f' || m_cmdPrefix == 't' || findInLineForward )
				++delTo;
			if( delFrom > delTo ) {		//	文書先頭方向に移動した場合
				int t = delFrom;
				delFrom = delTo;
				delTo = t;
			}
			break;
		case 'y':
			yankFrom = m_cdyPos;
			yankTo = cur.position();
			if( m_cmdPrefix == 'f' || m_cmdPrefix == 't' || findInLineForward )
				++yankTo;
			if( yankFrom > yankTo ) {		//	文書先頭方向に移動した場合
				int t = yankFrom;
				yankFrom = yankTo;
				yankTo = t;
			}
			break;
		default:
			if( !toInsertMode )
				cur.moveLeftIfEndOfLine();
			m_editor->setTextCursor(cur);
			//qDebug() << "cur.position() = " << cur.position();
		}
	}
	if( (delFrom >= 0 && delFrom != delTo || toUpdateRedoCmd || toInsertMode)
		&& !m_redoing )
	{
		//	.(repeat command) の為に編集コマンド文字列を記録
		if( m_cdyCmd )
			m_redoCmd = QChar(m_cdyCmd);
		else
			m_redoCmd = QString();
		if( m_cmdPrefix != 0 )
			m_redoCmd += QChar(m_cmdPrefix);
		m_redoCmd += qch;
		m_redoRepeatCount = repeatCount();
	}
	if( m_cdyCmd )
		m_repeatCount = m_repeatCount2 = 1;
	if( yankFrom >= 0 && yankFrom < yankTo ) {
		//	undone R delete の場合と処理を共通化する
		cur.setPosition(yankFrom);
		m_editor->setTextCursor(cur);
		cur.setPosition(yankTo, DocCursor::KeepAnchor);
		m_yankByLine = m_moveByLine || toYankByLine;
		m_yankBuffer = cur.selectedText();
		//m_editor->textCursor().setPosition(yankFrom);
	}
	if( delFrom >= 0 && delFrom != delTo ) {
		m_editor->clearSelection();
		if( m_moveByLine ) {
			//ViewCursor cur = m_editor->textCursor();
			cur.setPosition(delFrom);
			delFrom = cur.block().position();
			cur.setPosition(delTo);
			ViewBlock block = cur.block();
			//if( cur.position() != block.position() ) { //	delTo が行頭ではない場合
				if( block.next().isValid() )
					delTo = block.next().position();
				else
					delTo = block.position() + block.text().length();
			//}
		}
		cur.setPosition(delFrom);
		cur.setPosition(delTo, DocCursor::KeepAnchor);
		m_yankByLine = m_moveByLine || toYankByLine;
		m_yankBuffer = cur.selectedText();
		cur.deleteChar();
#if 0
		if( m_joinPrevEditBlock ) {
			document()->openUndoBlock();
			//cur.beginEditBlock();
			cur.deleteChar();
			document()->closeUndoBlock();
			//cur.endEditBlock();
		} else {
			cur.deleteChar();
		}
#endif
		if( !toInsertMode )
			cur.moveLeftIfEndOfLine();		//	改行位置にいる場合はカーソルを左移動
		m_editor->setTextCursor(cur);
		//	削除後に改行位置にいる場合はカーソルを左移動
#if 0
		ViewCursor cur = m_editor->textCursor();
		const ViewBlock block = cur.block();
		if( !toInsertMode && block.text().length() != 0 &&
			cur.position() == block.position() + block.text().length() )
		{
			cur.setPosition(cur.position() - 1);
			m_editor->setTextCursor(cur);
		}
#endif
	}
	if( toInsertMode ) {
		//if( m_joinPrevEditBlock )
		//	cur.beginEditBlock();
		m_insCount = repeatCount();
		if( !m_redoing ) {
			m_redoRecording = true;
			m_insertedText.clear();
			setMode(INSERT);
			m_editor->viewport()->update();		//	画面乱れを無くすため
		} else {
			if( !m_insertedText.isEmpty() ) {
				for(int c = ch != 's' ? repeatCount() : 1; c > 0; --c)
					cur.insertText(m_insertedText);
			}
			moveCursor(cur, ViMoveOperation::Left);
			m_editor->setTextCursor(cur);
		}
	}
	if( toReplaceMode ) {
		m_insCount = repeatCount();
		if( !m_redoing ) {
			m_redoRecording = true;
			m_insertedText.clear();
			m_editor->setOverwriteMode(true);
			setMode(REPLACE);
		} else {
			if( !m_insertedText.isEmpty() ) {
				//m_editor->setOverwriteMode(true);
				document()->openUndoBlock();
				for(int c = repeatCount(); c > 0; --c) {
					cur.movePosition(DocCursor::Right, DocCursor::KeepAnchor, m_insertedText.length());
					cur.insertText(m_insertedText);
				}
				document()->closeUndoBlock();
				//m_editor->setOverwriteMode(false);
			}
			moveCursor(cur, ViMoveOperation::Left);
			m_editor->setTextCursor(cur);
		}
		m_editor->viewport()->update();		//	画面乱れを無くすため
	}
	m_noRepeatCount = false;
	m_moveByLine = false;
	m_cmdPrefix = 0;
	m_cdyCmd = 0;
	m_repeatCount = 0;
	m_repeatCount2 = 0;
	m_cmdString.clear();
	m_message.clear();
	//showMessage(" ");
	if( !m_cmdString.isEmpty() )
		emit showMessage(m_cmdString);
	return true;
}
QString ViEngine::yankText() const
{
	QString text = m_yankBuffer;
	for(int i = repeatCount(); --i > 0; )
		text += m_yankBuffer;
	return text;
}
bool viCtrlCmd[] = 
{
	false,	//	Ctrl + @
	false,	//	Ctrl + A
	true,	//	Ctrl + B
	false,	//	Ctrl + C
	true,	//	Ctrl + D
	true,	//	Ctrl + E
	true,	//	Ctrl + F
	true,	//	Ctrl + G
	true,	//	Ctrl + H	BackSpace
	true,	//	Ctrl + I	TAB
	false,	//	Ctrl + J
	false,	//	Ctrl + K
	false,	//	Ctrl + L
	true,	//	Ctrl + M	Enter
	false,	//	Ctrl + N
	false,	//	Ctrl + O
	false,	//	Ctrl + P
	false,	//	Ctrl + Q
	false,	//	Ctrl + R
	false,	//	Ctrl + S
	false,	//	Ctrl + T
	true,	//	Ctrl + U
	false,	//	Ctrl + V
	false,	//	Ctrl + W
	false,	//	Ctrl + X
	true,	//	Ctrl + Y
	false,	//	Ctrl + Z
	true,	//	Ctrl + [	Esc
	false,	//	Ctrl + \ 
	false,	//	Ctrl + ]
	false,	//	Ctrl + ^
	false,	//	Ctrl + _
};
bool ViEngine::processKeyPressEvent( QKeyEvent * event )
{
	const int key = event->key();
	const QString text = event->text();
	//	カーソル移動キー、Home, End, PageUP, PageDown には文字コードは割り振られていないので、
	//	text は空になる
	if( !text.isEmpty() ) {
		if( key == Qt::Key_Delete )	//	Delete キーには文字コードが割り振られている
			return false;		
		ushort code = text[0].unicode();
		qDebug() << "code = " << code;
		//	done R テーブル化して判定
#if 0
		if( code < 0x20 && code != 'G'-'@' && code != '\t' && code != '\r' &&
			code != 'F' - '@' && code != 'B' - '@' && 
			code != 'D' - '@' && code != 'U' - '@' && 
			code != 'E' - '@' && code != 'Y' - '@' && 
			code != '\n' && code != 0x1b )
#endif
		if( code < 0x20 && !viCtrlCmd[code] )
		{
			return false;		//	普通のコントロールコードは通常処理
		}
		return doViCommand(text[0]);
		//return true;
	} else if( m_redoRecording &&
				(key == Qt::Key_Up || key == Qt::Key_Down ||
				key == Qt::Key_Left || key == Qt::Key_Right) )
	{
		m_redoRecording = false;
		m_joinPrevEditBlock = false;
	}
	return false;
}

bool isMatch(const QString &cmdText, const QString &pat)
{
	return pat.startsWith(cmdText);
}
void ViEngine::doExCommand(const QString &text)
{
	if( text.isEmpty() ) return;
	m_exCommands.removeOne(text);		//	重複削除
	m_exCommands.push_back(text);
	while( m_exCommands.count() > 100 )
		m_exCommands.pop_front();
    QSettings settings;
    settings.setValue("recentExCmdList", m_exCommands);
	int ix = 0;
	getLineNumbers(text, ix);
	bool exclamation;
	QString cmdText = getExCommand(text, ix, exclamation);
	QString param = text.mid(ix);
	if( cmdText.isEmpty() ) {	//	指定行にジャンプ
		if( m_lineNum2 < 0 ||
			m_lineNum2 > m_editor->document()->lastBlock().blockNumber() + 1)
		{
			return;		//	行番号が範囲外の場合
		}
		if( !m_lineNum2 )		//	:0 は最初の行にジャンプ
			m_lineNum2 = 1;
		ViewCursor cur = m_editor->textCursor();
		cur.movePosition(ViMoveOperation::JumpLine, DocCursor::MoveAnchor, m_lineNum2);
		m_editor->setTextCursor(cur);
		return;
	}
	//	undone B if-elseif チェインがある程度長くなったら、テーブルドリブンに書き換える
	if( cmdText == "set" ) {
		doSet(param);
	} else if( isMatch(cmdText, "delete") ) {
		doDelete();
	} else if( isMatch(cmdText, "substitute") ) {
		if( !m_nLineNum )	//	範囲指定無しの場合
			doSubst(param, m_curLineNum, m_curLineNum);
		else if( m_nLineNum == 1 )
			doSubst(param, m_lineNum2, m_lineNum2);
		else
			doSubst(param, m_lineNum1, m_lineNum2);
	} else if( isMatch(cmdText, "quit") ) {
		//if( exclamation )	//	!付きの場合は強制クローズ
		//	m_editor->document()->setModified(false);
		emit closeView(m_editor, exclamation);
	} else if( isMatch(cmdText, "QUIT") ) {
		emit closeAllViews(exclamation);
	} else if( isMatch(cmdText, "edit") ) {
		if( exclamation && param.isEmpty() )
			emit reOpen();
		else
			emit open(param);
	} else if( isMatch(cmdText, "write") ) {
		emit save(param);
	} else if( isMatch(cmdText, "writequit") || isMatch(cmdText, "wquit") ) {
		emit save(param);
		emit closeView(m_editor);
	} else if( cmdText == "pwd" ) {
		emit showMessage(m_message = QDir::currentPath());
	} else if( cmdText == "cd" ) {
		if( param.isEmpty() )
			QDir::setCurrent(QDir::homePath());
		else {
			QDir dir(param);
			QDir::setCurrent(dir.path());
		}
		emit showMessage(m_message = QDir::currentPath());
	} else if( cmdText == "cls" ) {
		emit clearOutput();
	} else if( cmdText == "test" ) {
		emit testViCommands(param);
	}
	//setMode(CMD);
}
void ViEngine::doSet(const QString &text)
{
	if( text.isEmpty() || text == "all" ) {
		printSettings();
		return;
	}
	int ix = text.indexOf('=');
	if( ix >= 0 ) {
		doSet(text.left(ix), text.mid(ix+1));
	} else {
		const bool no = text.startsWith("no");
		const QString t = text.mid(no ? 2 : 0);
		if( t == "linebreak" )
			m_editor->setLineBreakMode(!no);
	}
}
void ViEngine::doSet(const QString &key, const QString &value)
{
	QString v = value;
	if( value.length() >= 2 && value[0] == '"' && value[value.length() - 1] == '"' )
		v = value.mid(1, value.length() - 2);
	//bool fontChanged = false;
	//QFont font = m_editor->font();
	if( !key.compare("fontSize", Qt::CaseInsensitive) ||
		!key.compare("font", Qt::CaseInsensitive) && v.toInt() != 0 )
	{
		m_editor->setFontPointSize(v.toInt());
		//fontChanged = true;
	} else if( !key.compare("fontFamily", Qt::CaseInsensitive) ||
				!key.compare("fontName", Qt::CaseInsensitive) ||
				!key.compare("font", Qt::CaseInsensitive) )
	{
		m_editor->setFontFamily(v);
		//fontChanged = true;
	}
#if 0
	if( fontChanged ) {
		m_editor->setFont(font);
		//m_editor->
	}
#endif
}

void ViEngine::doDelete()
{
	if( !m_nLineNum )
		m_lineNum1 = m_lineNum2 = m_curLineNum;
	else if( m_nLineNum == 1 )
		m_lineNum1 = m_lineNum2;
	if( !m_lineNum1 ) m_lineNum1 = 1;
	if( m_lineNum1 > m_lineNum2 ) return;
	ViewCursor cur = m_editor->textCursor();
	DocBlock block1 = m_editor->document()->findBlockByNumber(m_lineNum1 - 1);
	cur.setPosition(block1.position());
	int pos;
	DocBlock block2 = m_editor->document()->findBlockByNumber(m_lineNum2 - 1);
	if( block2.next().isValid() )
		pos = block2.next().position();
	else
		pos = block2.position() + block2.text().length();
	cur.setPosition(pos, DocCursor::KeepAnchor);
	//	undone C "[a-z] ヤンクバッファ指定対応
	m_yankBuffer = cur.selectedText();
	m_yankByLine = true;
	cur.deleteChar();
}

//
//		src に含まれるエスケープ文字列を置換文字列に変換する
//		undone B 部分一致
//
QString replaceEscapeChars(const QString &src)
{
	QString dst;
	for(int ix = 0; ix < src.length(); ++ix) {
		const ushort ch = src[ix].unicode();
		if( ch == '\\' && ix + 1 < src.length() ) {
			switch( src[++ix].unicode() ) {
			case 't':
				dst += '\t';
				break;
			case 'r':
				dst += '\r';
				break;
			case 'n':
				dst += '\n';
				break;
			case '\\':
			default:
				dst += '\\';
				break;
			}
		} else
			dst += src[ix];
	}
	return dst;
}
//	置換範囲：[first, last]
void ViEngine::doSubst(const QString &param, int first, int last)
{
	if( param.isEmpty() ) {
		return;
	}
	QString before;
	QString after;
	bool global;
	if( !parseSubstCmd(param, before, after, global) ) {
		showMessage("usage: <range>s[ubst]/before/after/[g]");
		return;
	}
	//qDebug() << QString("before = '%1', after = '%2'").arg(before).arg(after);
	ViewCursor cur = m_editor->textCursor();
	document()->openUndoBlock();
	//cur.beginEditBlock();
	QRegExp rex(before);
	DocBlock block = m_editor->document()->findBlockByNumber(first - 1);
	while( first <= last && block.isValid() ) {
		for(int k = 0;;) {
			int ix = rex.indexIn(block.text(), k);
			if( ix < 0 ) break;
			cur.setPosition(block.position() + ix);
			cur.setPosition(block.position() + ix + rex.matchedLength(), DocCursor::KeepAnchor);
			cur.insertText(after);
			if( !global ) break;
			k = ix + after.length();
		}
		++first;
		block = block.next();
	}
	document()->closeUndoBlock();
	//cur.endEditBlock();
	m_editor->setTextCursor(cur);
}
bool ViEngine::parseSubstCmd(const QString &text,
								QString &before, QString &after, bool &bGlobal)
{
	QChar sep = text[0];
	int ix = text.indexOf(sep, 1);
	if( ix < 0 ) return false;
	before = text.mid(1, ix - 1);
	bGlobal = false;
	int ix2 = text.indexOf(sep, ix + 1);
	if( ix2 < 0 ) {
		after = text.mid(ix + 1);
	} else {
		after = text.mid(ix + 1, ix2 - ix - 1);
		for(int i = ix2 + 1; i < text.length(); ++i) {
			if( text[i] == 'g' )
				bGlobal = true;
			else
				return false;
		}
	}
	after = replaceEscapeChars(after);
	return true;
}
void ViEngine::doFind(const QString &regexpText, bool forward)
{
	setMode(CMD);
	QRegExp rex(regexpText);
	if( !rex.isValid() ) {
		emit showMessage(tr("invalid regexp."));
		return;
	}
	ViewCursor cur = m_editor->textCursor();
	if( moveCursorFind(cur, rex, forward) ) {
		m_findString = regexpText;
		m_findForward = forward;
		m_editor->setTextCursor(cur);
	}
	emit regexpSearched(regexpText);
}
bool ViEngine::doFindNext(ViewCursor &cur, int nth, bool revers)
{
	if( m_findString.isEmpty() )
		return false;
	QRegExp rex(m_findString);
	if( !rex.isValid() ) {
		emit showMessage(tr("invalid regexp."));
		return false;
	}
	const bool forward = !revers ? m_findForward : !m_findForward;
	return moveCursorFind(cur, rex, forward, nth);
}

void ViEngine::onImeOpenStatusChanged()
{
	if( mode() == CMD && !m_noInsModeAtImeOpenStatus )
		setMode(INSERT);
}

void ViEngine::printSettings()
{
	QFont font = m_editor->font();
	doOutput("fontFamily=\"" + font.family() + "\"\n");
	doOutput(QString("fontSize=%1pt\n").arg(font.pointSize()));
	doOutput(QString("%1linebreak\n").arg(m_editor->lineBreakMode() ? "  " : "no"));
}

QString ViEngine::getExCommand(const QString &text, int &ix, bool &exclamation)
{
	QString exCommand;
	while( ix < text.length() && text[ix].isLetterOrNumber() )
		exCommand += text[ix++];
	if( (exclamation = ix < text.length() && text[ix] == '!') )
		++ix;
	while( ix < text.length() && (text[ix] == ' ' || text[ix] == '\t') )
		++ix;
	return exCommand;
}

/*

	<range> ::= <lineNumber> | <range> {','|';'} <lineNumber>
	<lineNumber> ::= <digits> | '.' | '$'

*/
void ViEngine::getLineNumbers(const QString &text, int &ix)
{
	if( m_editor == 0 )
		m_curLineNum = 1;
	else {
		const ViewCursor cur = m_editor->textCursor();
		const ViewBlock block = cur.block();
		m_curLineNum = block.blockNumber() + 1;
	}
	if( ix < text.length() && text[ix] == '%' ) {
		++ix;
		m_nLineNum = 2;
		m_lineNum1 = 1;
		DocBlock block = m_editor->document()->lastBlock();
		m_lineNum2 = block.blockNumber() + 1;
		if( m_lineNum2 > 1 && block.text().isEmpty() )
			--m_lineNum2;
		return;
	}
	m_lineNum1 = m_lineNum2 = m_nLineNum = 0;
	ix = 0;
	int lineNumber;
	while( getAddSubTerm(text, ix, lineNumber) ) {
		++m_nLineNum;
		m_lineNum1 = m_lineNum2;
		m_lineNum2 = lineNumber;
		if( ix >= text.length() ) break;
		QChar ch = text[ix];
		if( ch == ';' )
			m_curLineNum = m_lineNum2;
		else if( ch != ',' )
			break;
		++ix;
	}
}
bool ViEngine::getAddSubTerm(const QString &text, int &ix, int &value)
{
	value = 0;
	if( ix >= text.length() ) return false;
	QChar op;
	if( (op = text[ix]) == '+' || op == '-' )
		value = m_curLineNum;	//	+2 とか -3 とかの様に . が省略された場合
	else
		if( !getLineNumber(text, ix, value) ) return false;
	while( ix < text.length() && ((op = text[ix]) == '+' || op == '-') ) {
		++ix;
		int term = 0;
		if( !getLineNumber(text, ix, term) ) return false;
		if( op == '+' )
			value += term;
		else
			value -= term;
	}
	return true;
}
bool ViEngine::getLineNumber(const QString &text, int &ix, int &lineNumber)
{
	lineNumber = 0;
	if( ix >= text.length() ) return false;
	if( text[ix] == '.' ) {
		++ix;
		lineNumber = m_curLineNum;
		return true;
	}
	if( text[ix] == '$' ) {
		++ix;
		if( m_editor == 0 ) return 1;
		DocBlock block = m_editor->document()->lastBlock();
		lineNumber = block.blockNumber() + 1;
		if( lineNumber > 1 && block.text().isEmpty() )
			--lineNumber;
		return true;
	}
	if( !text[ix].isDigit() ) return false;
	while( ix < text.length() && text[ix].isDigit() )
		lineNumber = lineNumber * 10 + (text[ix++].unicode() - '0');
	return true;
}
#endif
