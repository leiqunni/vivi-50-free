//----------------------------------------------------------------------
//
//			File:			"mainwindowTest.cpp"
//			Created:		05-Apr-2011
//			Author:			津田伸秀
//			Description:
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

extern MainWindow *pMainWindow;

void q_output(const QString &mess)
{
	pMainWindow->doOutput(mess);
}
void test_TextDocument();
void MainWindow::doUnitTest()
{
	g_ut_output = q_output;
	g_total_test_count = 0;
	g_total_fail_count = 0;
	if( 0 ) {
		CUnitTest ut("test");
		ut.test_equal(1, 1);
		ut.test_equal("abc", "abc");
		ut.test_equal(QString("あいうえお"), QString("あいうえお"));
		ut.test_equal(1, 2);
		ut.test_equal("abc", "xyzzz");
		ut.test_equal(QString("あいうえお"), QString("かきくけこ"));
	}
	//test_getWordEditInfo();
	test_TextDocument();
	QString temp;
	if( !g_total_fail_count ) {
		temp = QString("おめでとぉ、テスト失敗がひとつもありませんでした。(%1 tested)\n")
						.arg(g_total_test_count);
		//soundFile = theApp.getGlobSettings()->getTextValue(GLOBSTG_UT_SUCCESS_SOUND);
	} else {
		temp = QString("%1件のテスト失敗がありました。(%2 tested)\n")
						.arg(g_total_fail_count).arg(g_total_test_count);
		//soundFile = theApp.getGlobSettings()->getTextValue(GLOBSTG_UT_FAILURE_SOUND);
	}
	g_ut_output(temp);
}


void test_TextDocument()
{
	CUnitTest ut("TextDocument");
	if( 1 ) {
		TextDocument doc;
		ut.test(doc.isEmpty());
		ut.test_equal(0, doc.size());
		ut.test_equal(1, doc.blockCount());		//	EOF行のみ

		doc.setPlainText(QString("123\nabcde\r\nxyzzz\r"));
		ut.test(!doc.isEmpty());
		ut.test_equal(17, doc.size());
		ut.test_equal(4, doc.blockCount());

		TextBlock block = doc.firstBlock();
		ut.test_equal(4, block.size());			//	改行を含めたサイズ
		ut.test_equal("123\n", block.text());	//	改行を含めたテキスト
		block = block.next();
		ut.test(block.isValid());
		ut.test_equal(7, block.size());
		ut.test_equal("abcde\r\n", block.text());

		ut.test_equal(0, doc.findBlock(0).blockNumber());
		ut.test_equal(0, doc.findBlock(1).blockNumber());
		ut.test_equal(0, doc.findBlock(2).blockNumber());
		ut.test_equal(0, doc.findBlock(3).blockNumber());
		ut.test_equal(1, doc.findBlock(4).blockNumber());
		ut.test_equal(1, doc.findBlock(5).blockNumber());
		ut.test_equal(1, doc.findBlock(6).blockNumber());
		ut.test_equal(1, doc.findBlock(7).blockNumber());
		ut.test_equal(1, doc.findBlock(8).blockNumber());
		ut.test_equal(1, doc.findBlock(9).blockNumber());
		ut.test_equal(1, doc.findBlock(10).blockNumber());
		ut.test_equal(2, doc.findBlock(11).blockNumber());
		ut.test_equal(2, doc.findBlock(12).blockNumber());
		ut.test_equal(2, doc.findBlock(13).blockNumber());
		ut.test_equal(2, doc.findBlock(14).blockNumber());
		ut.test_equal(2, doc.findBlock(15).blockNumber());
		ut.test_equal(2, doc.findBlock(16).blockNumber());
		ut.test_equal(3, doc.findBlock(17).blockNumber());
		ut.test_equal(INVALID_INDEX, doc.findBlock(18).blockNumber());

		doc.setPlainText(QString("123\nabc"));			//	EOF行が空でない場合
		ut.test(!doc.isEmpty());
		ut.test_equal(7, doc.size());
		ut.test_equal(2, doc.blockCount());
	}
	if( 1 ) {
		TextDocument doc;
		doc.setPlainText(QString("123\nあいう\nかきくけこ\r\n漢字\r"));
		TextCursor cur(&doc);		//	先頭位置
		ut.test_equal(0, cur.position());
		cur.movePosition(TextCursor::Right);
		ut.test_equal(1, cur.position());
		cur.movePosition(TextCursor::Right);
		ut.test_equal(2, cur.position());
		cur.movePosition(TextCursor::Right);
		ut.test_equal(3, cur.position());		//	\n
		cur.movePosition(TextCursor::Right);
		ut.test_equal(4, cur.position());		//	あ
		cur.movePosition(TextCursor::Right);
		ut.test_equal(7, cur.position());		//	い

	}
	if( 1 ) {
		TextDocument doc;
		doc.setPlainText(QString("line-1\nline-2\nline-3\nline-4\n"));
		index_t bp;
		ut.test_equal(0, doc.findBlockIndex(0, &bp));
		ut.test_equal(0, bp);
		ut.test_equal(0, doc.findBlockIndex(5, &bp));		//	1
		ut.test_equal(0, bp);
		ut.test_equal(0, doc.findBlockIndex(6, &bp));		//	\n
		ut.test_equal(0, bp);
		ut.test_equal(1, doc.findBlockIndex(7, &bp));		//	line
		ut.test_equal(7, bp);
	}
	if( 1 ) {
		TextDocument doc;
		doc.setPlainText(QString("あいう\nかきくけこ\r\n漢字\r"));
	}
	if( 1 ) {
		TextDocument doc;
		doc.insert(0, "123\n");
		ut.test_equal(2, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	123\n
		ut.test_equal(4, doc.blockPosition(1));		//	EOF
		doc.insert(0, "xyz");		//	行の先頭に挿入
		ut.test_equal(2, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	xyz123\n
		ut.test_equal(7, doc.blockPosition(1));		//	EOF
		doc.insert(2, "12\n12");		//	行の途中に改行入テキスト挿入
		ut.test_equal(3, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	xy12\n
		ut.test_equal(5, doc.blockPosition(1));		//	12z123\n
		ut.test_equal(12, doc.blockPosition(2));	//	EOF
		doc.insert(4, "\r");		//	\n の直前に \r を挿入
		ut.test_equal(3, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	xy12\r\n
		ut.test_equal(6, doc.blockPosition(1));		//	12z123\n
		ut.test_equal(13, doc.blockPosition(2));	//	EOF
		doc.insert(1, "\r");		//	単に \r を挿入
		ut.test_equal(4, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	x\r
		ut.test_equal(2, doc.blockPosition(1));		//	y12\r\n
		ut.test_equal(7, doc.blockPosition(2));		//	12z123\n
		ut.test_equal(14, doc.blockPosition(3));	//	EOF
		doc.clear();
		doc.insert(0, "123");
		ut.test_equal(1, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	123
		doc.insert(2, "x\ny\nz");					//	改行を２つ含むテキスト挿入
		ut.test_equal(3, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	12x\n
		ut.test_equal(4, doc.blockPosition(1));		//	y\n
		ut.test_equal(6, doc.blockPosition(2));		//	z3
	}
	if( 1 ) {
		TextDocument doc;
		doc.insert(0, "123\n");
		doc.erase(1, 2);
		ut.test_equal(2, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	13\n
		ut.test_equal(3, doc.blockPosition(1));		//	EOF
		doc.clear();
		doc.insert(0, "123\nxyz\n");
		doc.erase(2, 5);		//	改行を含んで3文字削除
		ut.test_equal(2, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	12yz\n
		ut.test_equal(5, doc.blockPosition(1));		//	EOF
		doc.clear();
		doc.insert(0, "123\r\nxyz\n");
		doc.erase(2, 4);		//	\n の前の \r まで削除
		ut.test_equal(3, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	12\n
		ut.test_equal(3, doc.blockPosition(1));		//	xyz\n
		ut.test_equal(7, doc.blockPosition(2));		//	EOF
		doc.clear();
		doc.insert(0, "123\rxyz\n");
		ut.test_equal(3, doc.blockCount());
		doc.erase(2, 4);		//	\r 削除
		ut.test_equal(2, doc.blockCount());
		ut.test_equal(0, doc.blockPosition(0));		//	12xyz\n
		ut.test_equal(6, doc.blockPosition(1));		//	EOF

		doc.setPlainText(QString("123\nXYZ\n"));
		ut.test_equal(3, doc.blockCount());
		ut.test_equal(4, doc.firstBlock().size());
		ut.test_equal(QString("123\n"), doc.firstBlock().text());
		doc.erase(2, 5);		//	3文字削除
		ut.test_equal(2, doc.blockCount());
		ut.test_equal(5, doc.firstBlock().size());
		ut.test_equal(QString("12YZ\n"), doc.firstBlock().text());

		doc.setPlainText(QString("123\nXYZ\n"));
		ut.test_equal(3, doc.blockCount());
		ut.test_equal(4, doc.firstBlock().size());
		ut.test_equal(QString("123\n"), doc.firstBlock().text());
		doc.erase(2, 4);		//	2文字削除
		ut.test_equal(2, doc.blockCount());
		ut.test_equal(6, doc.firstBlock().size());
		ut.test_equal(QString("12XYZ\n"), doc.firstBlock().text());

		doc.setPlainText(QString("123\nXYZ\n"));
		ut.test_equal(3, doc.blockCount());
		ut.test_equal(4, doc.firstBlock().size());
		ut.test_equal(QString("123\n"), doc.firstBlock().text());
		doc.erase(3, 5);		//	2文字削除
		ut.test_equal(2, doc.blockCount());
		ut.test_equal(6, doc.firstBlock().size());
		ut.test_equal(QString("123YZ\n"), doc.firstBlock().text());
	}
	if( 1 ) {		//	undo/redo 対応 do_insert() テスト
		TextDocument doc;
		doc.do_insert(0, "123\n");
		ut.test_equal(QString("123\n"), doc.toPlainText());
		doc.doUndo();
		ut.test_equal(QString(""), doc.toPlainText());
		doc.doRedo();
		ut.test_equal(QString("123\n"), doc.toPlainText());
	}
	if( 1 ) {		//	undo/redo 対応 do_replace() テスト
		TextDocument doc;
		doc.setPlainText(QString("123\nxyzzz\n12345\n"));
		ut.test_equal(4, doc.firstBlock().size());
		doc.do_replace(0, 1, "XYZ");
		ut.test_equal(6, doc.firstBlock().size());
		ut.test_equal(QString("XYZ23\n"), doc.firstBlock().text());
		doc.doUndo();
		ut.test_equal(4, doc.firstBlock().size());
		ut.test_equal(QString("123\n"), doc.firstBlock().text());
		doc.doRedo();
		ut.test_equal(6, doc.firstBlock().size());
		ut.test_equal(QString("XYZ23\n"), doc.firstBlock().text());

		doc.setPlainText(QString("123\n456\n"));
		ut.test_equal(3, doc.blockCount());
		ut.test_equal(4, doc.firstBlock().size());
		ut.test_equal(QString("123\n"), doc.firstBlock().text());
		doc.do_replace(2, 5, "XYZ");
		ut.test_equal(2, doc.blockCount());
		ut.test_equal(8, doc.firstBlock().size());
		ut.test_equal(QString("12XYZ56\n"), doc.firstBlock().text());
	}
	if( 1 ) {
		TextDocument doc;
		doc.do_insert(0, "123\nxyzzz\nxyZZZ\n");
		ut.test( doc.find(QString("abc")).isNull() );
		ut.test( !doc.find(QString("xyZ")).isNull() );
		ut.test_equal(10, doc.find(QString("xyZ")).position() );
	}
	if( 1 ) {		//	TextCursor::insert() テスト
		TextDocument doc;
		doc.setPlainText(QString("0123\n"));
		ut.test_equal(5, doc.firstBlock().size());
		TextCursor cur(&doc);
		cur.setPosition(1);
		cur.insertText(QString("XYZ"));
		ut.test_equal(4, cur.position());
		ut.test_equal(8, doc.firstBlock().size());
		ut.test_equal(QString("0XYZ123\n"), doc.firstBlock().text());
		doc.doUndo();
		//ut.test_equal(4, cur.position());
		ut.test_equal(5, doc.firstBlock().size());
		ut.test_equal(QString("0123\n"), doc.firstBlock().text());
		doc.doRedo();
		ut.test_equal(4, cur.position());
		ut.test_equal(8, doc.firstBlock().size());
		ut.test_equal(QString("0XYZ123\n"), doc.firstBlock().text());

		cur.setPosition(1);
		cur.setPosition(4, TextCursor::KeepAnchor);		//	[1, 4) を選択
		cur.insertText(QString("7"));
		ut.test_equal(2, cur.position());
		ut.test_equal(6, doc.firstBlock().size());
		ut.test_equal(QString("07123\n"), doc.firstBlock().text());
		doc.doUndo();
		//ut.test_equal(4, cur.position());
		ut.test_equal(8, doc.firstBlock().size());
		ut.test_equal(QString("0XYZ123\n"), doc.firstBlock().text());
		doc.doRedo();
		ut.test_equal(2, cur.position());
		ut.test_equal(6, doc.firstBlock().size());
		ut.test_equal(QString("07123\n"), doc.firstBlock().text());
	}
	if( 1 ) {
		TextDocument *doc = new TextDocument();
		doc->setPlainText(QString("12345\n98765\n"));
		ut.test_equal(QString("12345\n"), doc->firstBlock().text());
		ut.test_equal(6, doc->firstBlock().size());
		TextCursor cur(doc);
		cur.setPosition(5);
		cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor, 3);	//	３文字選択
		cur.insertText("abcde");
		ut.test_equal(QString("12345abcde765\n"), doc->firstBlock().text());
		ut.test_equal(14, doc->firstBlock().size());
		delete doc;
	}
	if( 1 ) {
		TextDocument *doc = new TextDocument();
		doc->setPlainText(QString("12345\n98765\n"));
		ut.test_equal(QString("12345\n"), doc->firstBlock().text());
		ut.test_equal(6, doc->firstBlock().size());
		TextCursor cur(doc);
		cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor, 3);	//	３文字選択
		cur.insertText("XYZ");
		cur.movePosition(TextCursor::Right, TextCursor::MoveAnchor, 2);	//	２文字進める
		ut.test_equal(5, cur.position());
		cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor, 3);	//	３文字選択
		cur.insertText("ABCDE");
		ut.test_equal(QString("XYZ45ABCDE765\n"), doc->firstBlock().text());
		ut.test_equal(14, doc->firstBlock().size());
		delete doc;
	}
	if( 1 ) {
		TextDocument *doc = new TextDocument();
		doc->setPlainText(QString("0123456789\n9876543210\n"));
		ut.test_equal(QString("0123456789\n"), doc->firstBlock().text());
		ut.test_equal(11, doc->firstBlock().size());
		TextCursor cur(doc);
		if( !cur.atEnd() ) {
			cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor, 3);	//	３文字選択
			cur.insertText("abcde");
			ut.test_equal(QString("abcde3456789\n"), doc->firstBlock().text());
			ut.test_equal(13, doc->firstBlock().size());
			ut.test_equal(5, cur.position());
			cur.movePosition(TextCursor::Right, TextCursor::MoveAnchor, 2);	//	２文字進める
			ut.test_equal(7, cur.position());
		}
		if( !cur.atEnd() ) {
			cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor, 3);	//	３文字選択
			cur.insertText("abcde");
			ut.test_equal(QString("abcde34abcde89\n"), doc->firstBlock().text());
			ut.test_equal(15, doc->firstBlock().size());
			ut.test_equal(12, cur.position());
			cur.movePosition(TextCursor::Right, TextCursor::MoveAnchor, 2);	//	２文字進める
			ut.test_equal(14, cur.position());
			ut.test_equal(0, cur.blockIndex());
			ut.test_equal(0, cur.blockPosition());
		}
		if( !cur.atEnd() ) {
			cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor, 3);	//	３文字選択
			ut.test_equal(1, cur.blockIndex());
			ut.test_equal(15, cur.blockPosition());
			ut.test_equal(3, doc->blockCount());
			cur.insertText("ABCDE");
			ut.test_equal(2, doc->blockCount());
			ut.test_equal(28, doc->blockSize(0));
			ut.test_equal(28, doc->firstBlock().size());
			ut.test_equal(QString("abcde34abcde89ABCDE76543210\n"), doc->firstBlock().text());
			ut.test_equal(19, cur.position());
			cur.movePosition(TextCursor::Right, TextCursor::MoveAnchor, 2);	//	２文字進める
			ut.test_equal(21, cur.position());
		}
		if( !cur.atEnd() ) {
			cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor, 3);	//	３文字選択
			cur.insertText("abcde");
			ut.test_equal(26, cur.position());
			cur.movePosition(TextCursor::Right, TextCursor::MoveAnchor, 2);	//	２文字進める
			ut.test_equal(28, cur.position());
		}
		if( !cur.atEnd() ) {
			cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor, 3);	//	３文字選択
			cur.insertText("abcde");
			ut.test_equal(33, cur.position());
			cur.movePosition(TextCursor::Right, TextCursor::MoveAnchor, 2);	//	２文字進める
			ut.test( cur.atEnd() );
			ut.test_equal(33, cur.position());
		}
		if( !cur.atEnd() ) {
			cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor, 3);	//	３文字選択
			cur.insertText("abcde");
			cur.movePosition(TextCursor::Right, TextCursor::MoveAnchor, 2);	//	２文字進める
		}
		delete doc;
	}
	if( 1 ) {		//	findBlockByNumber() テスト
		TextDocument doc;
		TextCursor cur(&doc);		//	先頭位置
		const QString text("123456789\n");
		const int nLines = 100;
		for(int i = 0; i < nLines; ++i)
			cur.insertText(text);
		for(int i = 0; i < nLines; ++i) {
			TextBlock block = doc.findBlockByNumber(i);
			ut.test_equal(i*10, block.position());
			ut.test_equal(i, block.blockNumber());
		}
		for(int i = 0; i < nLines; i += 2) {
			TextBlock block = doc.findBlockByNumber(i);
			ut.test_equal(i*10, block.position());
			ut.test_equal(i, block.blockNumber());
		}
		for(int i = nLines; --i >= 0;) {
			TextBlock block = doc.findBlockByNumber(i);
			ut.test_equal(i*10, block.position());
			ut.test_equal(i, block.blockNumber());
		}
	}
	if( 1 ) {		//	findBlockByNumber() テスト、編集操作後
		TextDocument doc;
		TextCursor cur(&doc);		//	先頭位置
		const QString text("123456789\n");
		const int nLines = 100;
		for(int i = 0; i < nLines; ++i)
			cur.insertText(text);
		TextBlock block = doc.findBlockByNumber(50);
		ut.test_equal(50*10, block.position());
		ut.test_equal(50, block.blockNumber());
		cur.setPosition(0);
		cur.insertText(text);		//	先頭に1行・10バイト挿入
		block = doc.findBlockByNumber(50);
		ut.test_equal(50*10, block.position());
		ut.test_equal(50, block.blockNumber());
	}
}
