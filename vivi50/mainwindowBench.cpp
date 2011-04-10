//----------------------------------------------------------------------
//
//			File:			"mainwindowBench.cpp"
//			Created:		05-Apr-2011
//			Author:			津田伸秀
//			Description:	ベンチマーク
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
#include "mainwindow.h"
//#include "PlainTextEdit.h"
#include "TextDocument.h"
#include	<boost/timer.hpp>

extern MainWindow *pMainWindow;

//	findBlockByNumber ベンチマーク
void q_findBlockByNumber(uint n)
{
	QTextDocument doc;
	QTextCursor c(&doc);
	QString text = "XYZ入ってる。XYZ入ってる。XYZ入ってる。XYZ入ってる。XYZ入ってる。XYZ入ってる。XYZ入ってる。\n";
	for(uint i = 0; i < n; ++i)
		c.insertText(text);
	boost::timer tm;
	for(int i = 0; i < doc.blockCount(); ++i)
		QTextBlock block = doc.findBlockByNumber(i);
	const double dur = tm.elapsed();
	pMainWindow->doOutput(QString("\t%1: dur = %2\n").arg(n).arg(dur));
}
void v_findBlockByNumber(uint n)
{
	TextDocument doc;
	TextCursor c(&doc);
	QString text = "XYZ入ってる。XYZ入ってる。XYZ入ってる。XYZ入ってる。XYZ入ってる。XYZ入ってる。XYZ入ってる。\n";
	for(uint i = 0; i < n; ++i)
		c.insertText(text);
	boost::timer tm;
	for(int i = 0; i < doc.blockCount(); ++i)
		TextBlock block = doc.findBlockByNumber(i);
	const double dur = tm.elapsed();
	pMainWindow->doOutput(QString("\t%1: dur = %2\n").arg(n).arg(dur));
}
void q_replace35_mv7(uint n)
{
	QTextDocument doc;
	QTextCursor c(&doc);
	QString text = "XYZ入ってる。XYZ入ってる。XYZ入ってる。XYZ入ってる。XYZ入ってる。XYZ入ってる。XYZ入ってる。\n";
	for(uint i = 0; i < n; ++i)
		c.insertText(text);
	boost::timer tm;
	QTextCursor cur(&doc);
	while( !cur.atEnd() ) {
		cur.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 3);	//	３文字選択
		cur.insertText("abcde");
		cur.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 7);
	}
	const double dur = tm.elapsed();
	pMainWindow->doOutput(QString("\t%1: dur = %2\n").arg(n).arg(dur));
}
void do_replace35_mv7(uint n)
{
	TextDocument doc;
	TextCursor c(&doc);
	QString text = "XYZ入ってる。XYZ入ってる。XYZ入ってる。XYZ入ってる。XYZ入ってる。XYZ入ってる。XYZ入ってる。\n";
	for(uint i = 0; i < n; ++i)
		c.insertText(text);
	boost::timer tm;
	TextCursor cur(&doc);
	while( !cur.atEnd() ) {
		cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor, 3);	//	３文字選択
		cur.insertText("abcde");
		cur.movePosition(TextCursor::Right, TextCursor::MoveAnchor, 7);
	}
	const double dur = tm.elapsed();
	pMainWindow->doOutput(QString("\t%1: dur = %2\n").arg(n).arg(dur));
}
void MainWindow::doBenchmark()
{
#if 0
	doOutput("append:\n");
	doOutput("  QTextDocument:\n");
	q_append100(1000);
	q_append100(10000);

	doOutput("  TextDocument:\n");
	append100(1000);
	append100(10000);
	append100(100000);

	doOutput("  TextDocument (undo/redo):\n");
	do_append100(1000);
	do_append100(10000);
	do_append100(100000);

	doOutput("insert:\n");
	doOutput("  QTextDocument:\n");
	q_insert100(1000);
	q_insert100(10000);
	doOutput("  TextDocument (undo/redo):\n");
	do_insert100(1000);
	do_insert100(10000);
	//do_insert100(100000);
#endif

#if 0
	doOutput("seq-access by TextCursor:\n");
	doOutput("  QTextDocument:\n");
	q_seqAccessByCursor(1000);
	q_seqAccessByCursor(10000);
	q_seqAccessByCursor(100000);
	doOutput("  TextDocument:\n");
	do_seqAccessByCursor(1000);
	do_seqAccessByCursor(10000);
	do_seqAccessByCursor(100000);
#endif

#if 0
	doOutput("seq-access by TextBlock:\n");
	doOutput("  QTextDocument:\n");
	q_seqAccessByBlock(100000);
	doOutput("  TextDocument:\n");
	do_seqAccessByBlock(100000);
#endif

#if 0
	doOutput("find:\n");
	doOutput("  QTextDocument:\n");
	q_find(10000);
	doOutput("  TextDocument:\n");
	do_find(10000);
#endif

#if 1
	doOutput("replace '...' to 'abcde':\n");
	doOutput("  QTextDocument:\n");
	q_replace35_mv7(1000);
	q_replace35_mv7(5000);
	q_replace35_mv7(10000);
	doOutput("  TextDocument:\n");
	do_replace35_mv7(1000);
	do_replace35_mv7(5000);
	do_replace35_mv7(10000);
#endif

	doOutput("findBlockByNumber (sequential):\n");
	doOutput("  QTextDocument:\n");
	q_findBlockByNumber(1000);
	q_findBlockByNumber(5000);
	q_findBlockByNumber(10000);
	doOutput("  TextDocument:\n");
	v_findBlockByNumber(1000);
	v_findBlockByNumber(5000);
	v_findBlockByNumber(10000);

	doOutput("\n=== Benchmark Test finished ===\n");
}
