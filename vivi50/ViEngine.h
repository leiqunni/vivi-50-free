#pragma once

//----------------------------------------------------------------------
//
//			File:			"ViEngine.h"
//			Created:		24-Feb-2011
//			Author:			Nobuhide Tsuda
//			Description:	viエンジンクラス宣言
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

#ifndef VIENGINE_H
#define VIENGINE_H

#include <QObject>
#include <QStringList>
#include "vi.h"

class QKeyEvent;
class TextView;
class TextDocument;
class ViewCursor;

class ViEngine : public QObject
{
	Q_OBJECT

#if 0
public:
	enum Mode {
		CMD = 0,
		INSERT,
	};
#endif

public:
	ViEngine(QObject *parent = 0);
	~ViEngine();

public:
	Mode	mode() const { return m_mode; }
	const QString &message() const { return m_message; }
	const QStringList &exCommands() const { return m_exCommands; }

public:
	void	doViCommand(const QString &);
	bool	doViCommand(const QChar &);
	void	onImeOpenStatusChanged();
	void	setMode(Mode mode, ushort=0);
	void	setEditor(TextView *editor);	//{ m_view = editor; }
	TextView *editor() { return m_view; }
	TextDocument	*document();
	void	doExCommand(const QString &);			//	引数には、先頭の : を含まない文字列が渡る
	void	doSet(const QString &);
	void	doSet(const QString &, const QString &);
	void	doFind(const QString &, bool);			//	
	bool	processKeyPressEvent( QKeyEvent * event );

signals:
	void	modeChanged(Mode, ushort=0);
	void	reOpen();
	void	open(const QString &);
	void	save(const QString &);
	void	regexpSearched(const QString &);
	void	testViCommands(QString fileName);
	void	showMessage(const QString &);
	void	doOutput(const QString &);
	void	clearOutput();
	void	closeView(TextView*, bool = false);
	void	closeAllViews(bool = false);

protected:
	bool	eventFilter(QObject *obj, QEvent *event);

	bool	cmdModeKeyPressEvent(QKeyEvent *);
	bool	insModeKeyPressEvent(QKeyEvent *);
	void	doJoin(int n);
	bool	doShiftRight(ViewCursor&, int n);
	bool	doShiftLeft(ViewCursor&, int n);
	bool	doFindNext(ViewCursor &cur, int nth, bool revers=false);
	void	doSubst(const QString &, int, int);
	bool	parseSubstCmd(const QString &, QString &, QString &, bool &);
	void	doDelete();

	void	printSettings();
	void	printCursorPosition();

	int		repeatCount() const;
	//{ return !m_repeatCount ? 1 : m_repeatCount; }
	QString	yankText() const;

	void	getLineNumbers(const QString &, int &);		//	ex command の行番号範囲部分を解析
	bool	getLineNumber(const QString &, int &, int &);	//	値部分
	bool	getAddSubTerm(const QString &, int &, int &);	//	値 [{+|-} 値]...
	QString	getExCommand(const QString &, int &, bool &exclamation);

private:
	bool	m_noRepeatCount;		//	繰り返し回数指定不可（for fFtT等）
	bool	m_redoRecording;		//	.(redo) のために挿入文字列記録中
	bool	m_redoing;				//	.(redo) 実行中
	bool	m_noInsModeAtImeOpenStatus;		//	IME ON・OFFが変わっても挿入モードに遷移しない
	bool	m_joinPrevEditBlock;		//	
	bool	m_findForward;			//	順方向検索
	bool	m_moveByLine;			//	行単位移動 } { H M L G
	Mode	m_mode;
	int		m_repeatCount;
	int		m_repeatCount2;			//	c2w のようにコマンドの間に指定された繰り返し回数
	int		m_redoRepeatCount;
	int		m_insCount;				//	挿入回数
	int		m_cdyPos;				//	c|d|y が押された時のカーソル位置
	//int		m_cursorX;				//	ブロック先頭からのx変位
	ushort	m_cmdPrefix;			//	]] [[ の様な２ストロークコマンドの最初の文字
	ushort	m_cdyCmd;				//	c | d | y コマンド
	ushort	m_lastFTCmd;			//	'f' or 'F' or't' or 'T'
	QChar	m_lastFTChar;			//	fFtT 検索文字
	QString	m_findString;			//	検索正規表現文字列
	QString	m_cmdString;			//	入力中コマンドテキスト
	QString	m_redoCmd;				//	.(redo) で再実行するコマンド
	QString	m_insertedText;			//	挿入されたテキスト
	QString	m_message;
	bool	m_yankByLine;			//	ヤンクバッファ：行単位
	QString	m_yankBuffer;			//	無名ヤンクバッファ
	QStringList	m_exCommands;		//	入力された ex-command 文字列
	TextView	*m_view;

	//	undone C 行番号関係は独立した構造体にした方がよさげ
	int		m_nLineNum;				//	指定された行番号数
	int		m_lineNum1;				//	ex command range from
	int		m_lineNum2;				//	ex command range to
	int		m_curLineNum;			//	. lineNumber
};

#endif // VIENGINE_H
