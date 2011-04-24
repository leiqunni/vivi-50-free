//----------------------------------------------------------------------
//
//			File:			"mainwindowTest.cpp"
//			Created:		05-Apr-2011
//			Author:			�Óc�L�G
//			Description:
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

#include <QtGui>
#include "mainwindow.h"
#include "TextView.h"
#include "TextDocument.h"
#include "TextCursor.h"
#include "FindDlg.h"

extern MainWindow *pMainWindow;

void q_output(const QString &mess)
{
	pMainWindow->doOutput(mess);
}
void MainWindow::printBuffer()
{
	const TextDocument *doc = m_view->document();
	doOutput(QString("document size = %1 blockCount = %2\n")
				.arg(doc->size()).arg(doc->blockCount()));
	for(size_t ix = 0; ix < doc->blockCount(); ++ix) {
		doOutput(QString("block[%1].m_size = %2\n")
				.arg(ix).arg(doc->blockSize(ix)) );
	}
}

void test_TextDocument();
void test_TextView();
void MainWindow::doUnitTest()
{
	g_ut_output = q_output;
	g_total_test_count = 0;
	g_total_fail_count = 0;
	if( 0 ) {
		CUnitTest ut("ut_test");
		ut.ut_test_equal(1, 1);
		ut.ut_test_equal("abc", "abc");
		ut.ut_test_equal(QString("����������"), QString("����������"));
		ut.ut_test_equal(1, 2);
		ut.ut_test_equal(QString("abc"), QString("xyzzz"));
		ut.ut_test_equal(QString("����������"), QString("����������"));
	}
	if( m_unitTestDoc )
		test_TextDocument();
	if( m_unitTestView )
		test_TextView();
	QString temp;
	if( !g_total_fail_count ) {
		temp = QString("���߂łƂ��A�e�X�g���s���ЂƂ�����܂���ł����B(%1 tested)\n")
						.arg(g_total_test_count);
		//soundFile = theApp.getGlobSettings()->getTextValue(GLOBSTG_UT_SUCCESS_SOUND);
	} else {
		temp = QString("%1���̃e�X�g���s������܂����B(%2 tested)\n")
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
		ut.ut_test(doc.isEmpty());
		ut.ut_test_equal(0, doc.size());
		ut.ut_test_equal(1, doc.blockCount());		//	EOF�s�̂�

		doc.setPlainText(QString("123\nabcde\r\nxyzzz\r"));
		ut.ut_test(!doc.isEmpty());
		ut.ut_test_equal(17, doc.size());
		ut.ut_test_equal(4, doc.blockCount());

		DocBlock block = doc.firstBlock();
		ut.ut_test_equal(4, block.size());			//	���s���܂߂��T�C�Y
		ut.ut_test_equal("123\n", block.text());	//	���s���܂߂��e�L�X�g
		block = block.next();
		ut.ut_test(block.isValid());
		ut.ut_test_equal(7, block.size());
		ut.ut_test_equal("abcde\r\n", block.text());

		ut.ut_test_equal(0, doc.findBlock(0).blockNumber());
		ut.ut_test_equal(0, doc.findBlock(1).blockNumber());
		ut.ut_test_equal(0, doc.findBlock(2).blockNumber());
		ut.ut_test_equal(0, doc.findBlock(3).blockNumber());
		ut.ut_test_equal(1, doc.findBlock(4).blockNumber());
		ut.ut_test_equal(1, doc.findBlock(5).blockNumber());
		ut.ut_test_equal(1, doc.findBlock(6).blockNumber());
		ut.ut_test_equal(1, doc.findBlock(7).blockNumber());
		ut.ut_test_equal(1, doc.findBlock(8).blockNumber());
		ut.ut_test_equal(1, doc.findBlock(9).blockNumber());
		ut.ut_test_equal(1, doc.findBlock(10).blockNumber());
		ut.ut_test_equal(2, doc.findBlock(11).blockNumber());
		ut.ut_test_equal(2, doc.findBlock(12).blockNumber());
		ut.ut_test_equal(2, doc.findBlock(13).blockNumber());
		ut.ut_test_equal(2, doc.findBlock(14).blockNumber());
		ut.ut_test_equal(2, doc.findBlock(15).blockNumber());
		ut.ut_test_equal(2, doc.findBlock(16).blockNumber());
		ut.ut_test_equal(3, doc.findBlock(17).blockNumber());
		ut.ut_test_equal(INVALID_INDEX, doc.findBlock(18).blockNumber());

		doc.setPlainText(QString("123\nabc"));			//	EOF�s����łȂ��ꍇ
		ut.ut_test(!doc.isEmpty());
		ut.ut_test_equal(7, doc.size());
		ut.ut_test_equal(2, doc.blockCount());
	}
	if( 1 ) {
		TextDocument doc;
		doc.setPlainText(QString("123\n������\n����������\r\n����\r"));
		DocCursor cur(&doc);		//	�擪�ʒu
		ut.ut_test_equal(0, cur.position());
		cur.movePosition(DocCursor::Right);
		ut.ut_test_equal(1, cur.position());
		cur.movePosition(DocCursor::Right);
		ut.ut_test_equal(2, cur.position());
		cur.movePosition(DocCursor::Right);
		ut.ut_test_equal(3, cur.position());		//	\n
		cur.movePosition(DocCursor::Right);
		ut.ut_test_equal(4, cur.position());		//	��
		cur.movePosition(DocCursor::Right);
		ut.ut_test_equal(7, cur.position());		//	��

	}
	if( 1 ) {
		TextDocument doc;
		doc.setPlainText(QString("1x() File: hoge\nxyz����������\r"));
		DocCursor cur(&doc);		//	�擪�ʒu
		ut.ut_test_equal(0, cur.position());
		cur.movePosition(DocCursor::NextWord);
		ut.ut_test_equal(2, cur.position());		//	(
		cur.movePosition(DocCursor::NextWord);
		ut.ut_test_equal(5, cur.position());		//	File
		cur.movePosition(DocCursor::NextWord);
		ut.ut_test_equal(9, cur.position());		//	:
		cur.movePosition(DocCursor::NextWord);
		ut.ut_test_equal(11, cur.position());		//	h
		cur.movePosition(DocCursor::NextWord);
		ut.ut_test_equal(15, cur.position());		//	\n
		cur.movePosition(DocCursor::NextWord);
		ut.ut_test_equal(16, cur.position());		//	x
		cur.movePosition(DocCursor::NextWord);
		ut.ut_test_equal(19, cur.position());		//	��
		cur.movePosition(DocCursor::NextWord);
		ut.ut_test_equal(28, cur.position());		//	��
		cur.movePosition(DocCursor::NextWord);
		ut.ut_test_equal(34, cur.position());		//	\r
		cur.movePosition(DocCursor::NextWord);
		ut.ut_test_equal(35, cur.position());		//	EOF
		cur.movePosition(DocCursor::PrevWord);
		ut.ut_test_equal(34, cur.position());		//	\r
		cur.movePosition(DocCursor::PrevWord);
		ut.ut_test_equal(28, cur.position());		//	��
		cur.movePosition(DocCursor::PrevWord);
		ut.ut_test_equal(19, cur.position());		//	��
		cur.movePosition(DocCursor::PrevWord);
		ut.ut_test_equal(16, cur.position());		//	x
		cur.movePosition(DocCursor::PrevWord);
		ut.ut_test_equal(15, cur.position());		//	\n
		cur.movePosition(DocCursor::PrevWord);
		ut.ut_test_equal(11, cur.position());		//	h
		cur.movePosition(DocCursor::PrevWord);
		ut.ut_test_equal(9, cur.position());		//	:
		cur.movePosition(DocCursor::PrevWord);
		ut.ut_test_equal(5, cur.position());		//	File
		cur.movePosition(DocCursor::PrevWord);
		ut.ut_test_equal(2, cur.position());		//	(
		cur.movePosition(DocCursor::PrevWord);
		ut.ut_test_equal(0, cur.position());		//	1
	}
	if( 1 ) {
		TextDocument doc;
		doc.setPlainText(QString("line-1\nline-2\nline-3\nline-4\n"));
		index_t bp;
		ut.ut_test_equal(0, doc.findBlockIndex(0, &bp));
		ut.ut_test_equal(0, bp);
		ut.ut_test_equal(0, doc.findBlockIndex(5, &bp));		//	1
		ut.ut_test_equal(0, bp);
		ut.ut_test_equal(0, doc.findBlockIndex(6, &bp));		//	\n
		ut.ut_test_equal(0, bp);
		ut.ut_test_equal(1, doc.findBlockIndex(7, &bp));		//	line
		ut.ut_test_equal(7, bp);
	}
	if( 1 ) {
		TextDocument doc;
		doc.setPlainText(QString("������\n����������\r\n����\r"));
	}
	if( 1 ) {
		TextDocument doc;
		doc.insert(0, "123\n");
		ut.ut_test_equal(2, doc.blockCount());
		ut.ut_test_equal(0, doc.blockPosition(0));		//	123\n
		ut.ut_test_equal(4, doc.blockPosition(1));		//	EOF
		doc.insert(0, "xyz");		//	�s�̐擪�ɑ}��
		ut.ut_test_equal(2, doc.blockCount());
		ut.ut_test_equal(0, doc.blockPosition(0));		//	xyz123\n
		ut.ut_test_equal(7, doc.blockPosition(1));		//	EOF
		doc.insert(2, "12\n12");		//	�s�̓r���ɉ��s���e�L�X�g�}��
		ut.ut_test_equal(3, doc.blockCount());
		ut.ut_test_equal(0, doc.blockPosition(0));		//	xy12\n
		ut.ut_test_equal(5, doc.blockPosition(1));		//	12z123\n
		ut.ut_test_equal(12, doc.blockPosition(2));	//	EOF
		doc.insert(4, "\r");		//	\n �̒��O�� \r ��}��
		ut.ut_test_equal(3, doc.blockCount());
		ut.ut_test_equal(0, doc.blockPosition(0));		//	xy12\r\n
		ut.ut_test_equal(6, doc.blockPosition(1));		//	12z123\n
		ut.ut_test_equal(13, doc.blockPosition(2));	//	EOF
		doc.insert(1, "\r");		//	�P�� \r ��}��
		ut.ut_test_equal(4, doc.blockCount());
		ut.ut_test_equal(0, doc.blockPosition(0));		//	x\r
		ut.ut_test_equal(2, doc.blockPosition(1));		//	y12\r\n
		ut.ut_test_equal(7, doc.blockPosition(2));		//	12z123\n
		ut.ut_test_equal(14, doc.blockPosition(3));	//	EOF
		doc.clear();
		doc.insert(0, "123");
		ut.ut_test_equal(1, doc.blockCount());
		ut.ut_test_equal(0, doc.blockPosition(0));		//	123
		doc.insert(2, "x\ny\nz");					//	���s���Q�܂ރe�L�X�g�}��
		ut.ut_test_equal(3, doc.blockCount());
		ut.ut_test_equal(0, doc.blockPosition(0));		//	12x\n
		ut.ut_test_equal(4, doc.blockPosition(1));		//	y\n
		ut.ut_test_equal(6, doc.blockPosition(2));		//	z3
	}
	if( 1 ) {
		TextDocument doc;
		doc.insert(0, "123\n");
		doc.erase(1, 2);
		ut.ut_test_equal(2, doc.blockCount());
		ut.ut_test_equal(0, doc.blockPosition(0));		//	13\n
		ut.ut_test_equal(3, doc.blockPosition(1));		//	EOF
		doc.clear();
		doc.insert(0, "123\nxyz\n");
		doc.erase(2, 5);		//	���s���܂��3�����폜
		ut.ut_test_equal(2, doc.blockCount());
		ut.ut_test_equal(0, doc.blockPosition(0));		//	12yz\n
		ut.ut_test_equal(5, doc.blockPosition(1));		//	EOF
		doc.clear();
		doc.insert(0, "123\r\nxyz\n");
		doc.erase(2, 4);		//	\n �̑O�� \r �܂ō폜
		ut.ut_test_equal(3, doc.blockCount());
		ut.ut_test_equal(0, doc.blockPosition(0));		//	12\n
		ut.ut_test_equal(3, doc.blockPosition(1));		//	xyz\n
		ut.ut_test_equal(7, doc.blockPosition(2));		//	EOF
		doc.clear();
		doc.insert(0, "123\rxyz\n");
		ut.ut_test_equal(3, doc.blockCount());
		doc.erase(2, 4);		//	\r �폜
		ut.ut_test_equal(2, doc.blockCount());
		ut.ut_test_equal(0, doc.blockPosition(0));		//	12xyz\n
		ut.ut_test_equal(6, doc.blockPosition(1));		//	EOF

		doc.setPlainText(QString("123\nXYZ\n"));
		ut.ut_test_equal(3, doc.blockCount());
		ut.ut_test_equal(4, doc.firstBlock().size());
		ut.ut_test_equal(QString("123\n"), doc.firstBlock().text());
		doc.erase(2, 5);		//	3�����폜
		ut.ut_test_equal(2, doc.blockCount());
		ut.ut_test_equal(5, doc.firstBlock().size());
		ut.ut_test_equal(QString("12YZ\n"), doc.firstBlock().text());

		doc.setPlainText(QString("123\nXYZ\n"));
		ut.ut_test_equal(3, doc.blockCount());
		ut.ut_test_equal(4, doc.firstBlock().size());
		ut.ut_test_equal(QString("123\n"), doc.firstBlock().text());
		doc.erase(2, 4);		//	2�����폜
		ut.ut_test_equal(2, doc.blockCount());
		ut.ut_test_equal(6, doc.firstBlock().size());
		ut.ut_test_equal(QString("12XYZ\n"), doc.firstBlock().text());

		doc.setPlainText(QString("123\nXYZ\n"));
		ut.ut_test_equal(3, doc.blockCount());
		ut.ut_test_equal(4, doc.firstBlock().size());
		ut.ut_test_equal(QString("123\n"), doc.firstBlock().text());
		doc.erase(3, 5);		//	2�����폜
		ut.ut_test_equal(2, doc.blockCount());
		ut.ut_test_equal(6, doc.firstBlock().size());
		ut.ut_test_equal(QString("123YZ\n"), doc.firstBlock().text());
	}
	if( 1 ) {		//	undo/redo �Ή� do_insert() �e�X�g
		TextDocument doc;
		doc.do_insert(0, "123\n");
		ut.ut_test_equal(QString("123\n"), doc.toPlainText());
		index_t position, anchor;
		doc.doUndo(position, anchor);
		ut.ut_test_equal(QString(""), doc.toPlainText());
		doc.doRedo(position, anchor);
		ut.ut_test_equal(QString("123\n"), doc.toPlainText());
	}
	if( 1 ) {		//	undo/redo �Ή� do_replace() �e�X�g
		TextDocument doc;
		doc.setPlainText(QString("123\nxyzzz\n12345\n"));
		ut.ut_test_equal(4, doc.firstBlock().size());
		doc.do_replace(0, 1, "XYZ");
		ut.ut_test_equal(6, doc.firstBlock().size());
		ut.ut_test_equal(QString("XYZ23\n"), doc.firstBlock().text());
		index_t position, anchor;
		doc.doUndo(position, anchor);
		ut.ut_test_equal(4, doc.firstBlock().size());
		ut.ut_test_equal(QString("123\n"), doc.firstBlock().text());
		doc.doRedo(position, anchor);
		ut.ut_test_equal(6, doc.firstBlock().size());
		ut.ut_test_equal(QString("XYZ23\n"), doc.firstBlock().text());

		doc.setPlainText(QString("123\n456\n"));
		ut.ut_test_equal(3, doc.blockCount());
		ut.ut_test_equal(4, doc.firstBlock().size());
		ut.ut_test_equal(QString("123\n"), doc.firstBlock().text());
		doc.do_replace(2, 5, "XYZ");
		ut.ut_test_equal(2, doc.blockCount());
		ut.ut_test_equal(8, doc.firstBlock().size());
		ut.ut_test_equal(QString("12XYZ56\n"), doc.firstBlock().text());
	}
	if( 1 ) {
		TextDocument doc;
		doc.do_insert(0, "123\nxyzzz\nxyZZZ\n");
		ut.ut_test( doc.find(QString("abc")).isNull() );
		ut.ut_test( !doc.find(QString("xyZ")).isNull() );
		DocCursor c = doc.find(QString("xyZ"), 0, MatchCase);
		ut.ut_test_equal(10, c.anchor() );
		ut.ut_test_equal(13, c.position() );
	}
	if( 1 ) {		//	DocCursor::insert() �e�X�g
		TextDocument doc;
		doc.setPlainText(QString("0123\n"));
		ut.ut_test_equal(5, doc.firstBlock().size());
		DocCursor cur(&doc);
		cur.setPosition(1);
		cur.insertText(QString("XYZ"));
		ut.ut_test_equal(4, cur.position());
		ut.ut_test_equal(8, doc.firstBlock().size());
		ut.ut_test_equal(QString("0XYZ123\n"), doc.firstBlock().text());
		index_t position, anchor;
		doc.doUndo(position, anchor);
		//ut.ut_test_equal(4, cur.position());
		ut.ut_test_equal(5, doc.firstBlock().size());
		ut.ut_test_equal(QString("0123\n"), doc.firstBlock().text());
		doc.doRedo(position, anchor);
		ut.ut_test_equal(4, cur.position());
		ut.ut_test_equal(8, doc.firstBlock().size());
		ut.ut_test_equal(QString("0XYZ123\n"), doc.firstBlock().text());

		cur.setPosition(1);
		cur.setPosition(4, DocCursor::KeepAnchor);		//	[1, 4) ��I��
		cur.insertText(QString("7"));
		ut.ut_test_equal(2, cur.position());
		ut.ut_test_equal(6, doc.firstBlock().size());
		ut.ut_test_equal(QString("07123\n"), doc.firstBlock().text());
		doc.doUndo(position, anchor);
		//ut.ut_test_equal(4, cur.position());
		ut.ut_test_equal(8, doc.firstBlock().size());
		ut.ut_test_equal(QString("0XYZ123\n"), doc.firstBlock().text());
		doc.doRedo(position, anchor);
		ut.ut_test_equal(2, cur.position());
		ut.ut_test_equal(6, doc.firstBlock().size());
		ut.ut_test_equal(QString("07123\n"), doc.firstBlock().text());
	}
	if( 1 ) {
		TextDocument doc;
		DocCursor cur(&doc);
		cur.insertText("abc\n");
		ut.ut_test_equal(4, cur.position());
		ut.ut_test_equal(1, cur.blockData().index());
		ut.ut_test_equal(4, cur.blockData().position());
		ut.ut_test_equal(2, doc.blockCount());
		ut.ut_test_equal(4, doc.blockSize(0));
		ut.ut_test_equal(0, doc.blockSize(1));
	}
	if( 1 ) {
		TextDocument doc;
		DocCursor cur(&doc);
		cur.insertText("abc");
		cur.insertText("\n");
		ut.ut_test_equal(4, cur.position());
		ut.ut_test_equal(1, cur.blockData().index());
		ut.ut_test_equal(4, cur.blockData().position());
		ut.ut_test_equal(2, doc.blockCount());
		ut.ut_test_equal(4, doc.blockSize(0));
		ut.ut_test_equal(0, doc.blockSize(1));
	}
	if( 1 ) {
		TextDocument doc;
		doc.setPlainText(QString("12\r\nxyzzz\n"));
		DocCursor cur(&doc);
		cur.movePosition(DocCursor::Right);
		ut.ut_test_equal(1, cur.position());		//	2
		cur.movePosition(DocCursor::Right);
		ut.ut_test_equal(2, cur.position());		//	\r\n
		cur.movePosition(DocCursor::Right);
		ut.ut_test_equal(4, cur.position());		//	x
		cur.movePosition(DocCursor::Left);
		ut.ut_test_equal(2, cur.position());		//	\r\n
		cur.movePosition(DocCursor::Left);
		ut.ut_test_equal(1, cur.position());		//	2
	}
	if( 1 ) {		//	�J�[�\���㉺�ړ��e�X�g
		TextDocument doc;
		doc.setPlainText(QString("12\r\nxyzzz\n"));
		DocCursor cur(&doc);
		cur.movePosition(DocCursor::Down);
		ut.ut_test_equal(4, cur.position());		//	x
		cur.movePosition(DocCursor::Down);
		ut.ut_test_equal(10, cur.position());		//	EOF
		cur.movePosition(DocCursor::Up);
		ut.ut_test_equal(4, cur.position());		//	x
		cur.movePosition(DocCursor::Up);
		ut.ut_test_equal(0, cur.position());		//	1
		cur.movePosition(DocCursor::Right);
		ut.ut_test_equal(1, cur.position());		//	2
		cur.movePosition(DocCursor::Down);
		ut.ut_test_equal(5, cur.position());		//	y
		cur.movePosition(DocCursor::Down);
		ut.ut_test_equal(10, cur.position());		//	EOF
		cur.movePosition(DocCursor::Up);
		ut.ut_test_equal(5, cur.position());		//	y
		cur.movePosition(DocCursor::Up);
		ut.ut_test_equal(1, cur.position());		//	2
	}
	if( 1 ) {
		TextDocument *doc = new TextDocument();
		doc->setPlainText(QString("12345\n98765\n"));
		ut.ut_test_equal(QString("12345\n"), doc->firstBlock().text());
		ut.ut_test_equal(6, doc->firstBlock().size());
		DocCursor cur(doc);
		cur.setPosition(5);
		cur.movePosition(DocCursor::Right, DocCursor::KeepAnchor, 3);	//	�R�����I��
		cur.insertText("abcde");
		ut.ut_test_equal(QString("12345abcde765\n"), doc->firstBlock().text());
		ut.ut_test_equal(14, doc->firstBlock().size());
		delete doc;
	}
	if( 1 ) {
		TextDocument *doc = new TextDocument();
		doc->setPlainText(QString("12345\n98765\n"));
		ut.ut_test_equal(QString("12345\n"), doc->firstBlock().text());
		ut.ut_test_equal(6, doc->firstBlock().size());
		DocCursor cur(doc);
		cur.movePosition(DocCursor::Right, DocCursor::KeepAnchor, 3);	//	�R�����I��
		cur.insertText("XYZ");
		cur.movePosition(DocCursor::Right, DocCursor::MoveAnchor, 2);	//	�Q�����i�߂�
		ut.ut_test_equal(5, cur.position());
		cur.movePosition(DocCursor::Right, DocCursor::KeepAnchor, 3);	//	�R�����I��
		cur.insertText("ABCDE");
		ut.ut_test_equal(QString("XYZ45ABCDE765\n"), doc->firstBlock().text());
		ut.ut_test_equal(14, doc->firstBlock().size());
		delete doc;
	}
	if( 1 ) {
		TextDocument *doc = new TextDocument();
		doc->setPlainText(QString("0123456789\n9876543210\n"));
		ut.ut_test_equal(QString("0123456789\n"), doc->firstBlock().text());
		ut.ut_test_equal(11, doc->firstBlock().size());
		DocCursor cur(doc);
		if( !cur.atEnd() ) {
			cur.movePosition(DocCursor::Right, DocCursor::KeepAnchor, 3);	//	�R�����I��
			cur.insertText("abcde");
			ut.ut_test_equal(QString("abcde3456789\n"), doc->firstBlock().text());
			ut.ut_test_equal(13, doc->firstBlock().size());
			ut.ut_test_equal(5, cur.position());
			cur.movePosition(DocCursor::Right, DocCursor::MoveAnchor, 2);	//	�Q�����i�߂�
			ut.ut_test_equal(7, cur.position());
		}
		if( !cur.atEnd() ) {
			cur.movePosition(DocCursor::Right, DocCursor::KeepAnchor, 3);	//	�R�����I��
			cur.insertText("abcde");
			ut.ut_test_equal(QString("abcde34abcde89\n"), doc->firstBlock().text());
			ut.ut_test_equal(15, doc->firstBlock().size());
			ut.ut_test_equal(12, cur.position());
			cur.movePosition(DocCursor::Right, DocCursor::MoveAnchor, 2);	//	�Q�����i�߂�
			ut.ut_test_equal(14, cur.position());
			ut.ut_test_equal(0, cur.blockIndex());
			ut.ut_test_equal(0, cur.blockPosition());
		}
		if( !cur.atEnd() ) {
			cur.movePosition(DocCursor::Right, DocCursor::KeepAnchor, 3);	//	�R�����I��
			ut.ut_test_equal(1, cur.blockIndex());
			ut.ut_test_equal(15, cur.blockPosition());
			ut.ut_test_equal(3, doc->blockCount());
			cur.insertText("ABCDE");
			ut.ut_test_equal(2, doc->blockCount());
			ut.ut_test_equal(28, doc->blockSize(0));
			ut.ut_test_equal(28, doc->firstBlock().size());
			ut.ut_test_equal(QString("abcde34abcde89ABCDE76543210\n"), doc->firstBlock().text());
			ut.ut_test_equal(19, cur.position());
			cur.movePosition(DocCursor::Right, DocCursor::MoveAnchor, 2);	//	�Q�����i�߂�
			ut.ut_test_equal(21, cur.position());
		}
		if( !cur.atEnd() ) {
			cur.movePosition(DocCursor::Right, DocCursor::KeepAnchor, 3);	//	�R�����I��
			cur.insertText("abcde");
			ut.ut_test_equal(26, cur.position());
			cur.movePosition(DocCursor::Right, DocCursor::MoveAnchor, 2);	//	�Q�����i�߂�
			ut.ut_test_equal(28, cur.position());
		}
		if( !cur.atEnd() ) {
			cur.movePosition(DocCursor::Right, DocCursor::KeepAnchor, 3);	//	�R�����I��
			cur.insertText("abcde");
			ut.ut_test_equal(33, cur.position());
			cur.movePosition(DocCursor::Right, DocCursor::MoveAnchor, 2);	//	�Q�����i�߂�
			ut.ut_test( cur.atEnd() );
			ut.ut_test_equal(33, cur.position());
		}
		if( !cur.atEnd() ) {
			cur.movePosition(DocCursor::Right, DocCursor::KeepAnchor, 3);	//	�R�����I��
			cur.insertText("abcde");
			cur.movePosition(DocCursor::Right, DocCursor::MoveAnchor, 2);	//	�Q�����i�߂�
		}
		delete doc;
	}
	if( 1 ) {		//	findBlockByNumber() �e�X�g
		TextDocument doc;
		DocCursor cur(&doc);		//	�擪�ʒu
		const QString text("123456789\n");
		const int nLines = 100;
		for(int i = 0; i < nLines; ++i)
			cur.insertText(text);
		for(int i = 0; i < nLines; ++i) {
			DocBlock block = doc.findBlockByNumber(i);
			ut.ut_test_equal(i*10, block.position());
			ut.ut_test_equal(i, block.blockNumber());
		}
		for(int i = 0; i < nLines; i += 2) {
			DocBlock block = doc.findBlockByNumber(i);
			ut.ut_test_equal(i*10, block.position());
			ut.ut_test_equal(i, block.blockNumber());
		}
		for(int i = nLines; --i >= 0;) {
			DocBlock block = doc.findBlockByNumber(i);
			ut.ut_test_equal(i*10, block.position());
			ut.ut_test_equal(i, block.blockNumber());
		}
	}
	if( 1 ) {		//	findBlockByNumber() �e�X�g�A�ҏW�����
		TextDocument doc;
		DocCursor cur(&doc);		//	�擪�ʒu
		const QString text("123456789\n");
		const int nLines = 100;
		for(int i = 0; i < nLines; ++i)
			cur.insertText(text);
		DocBlock block = doc.findBlockByNumber(50);
		ut.ut_test_equal(50*10, block.position());
		ut.ut_test_equal(50, block.blockNumber());
		cur.setPosition(0);
		cur.insertText(text);		//	�擪��1�s�E10�o�C�g�}��
		block = doc.findBlockByNumber(50);
		ut.ut_test_equal(50*10, block.position());
		ut.ut_test_equal(50, block.blockNumber());
	}
	if( 1 ) {		//	findBlockData() �e�X�g�A�L���b�V���������ɂ���ꍇ
		TextDocument doc;
		DocCursor cur(&doc);		//	�擪�ʒu
		const QString text("123456789\n");		//	10byte �e�L�X�g
		const int nLines = 30;
		for(int i = 0; i < nLines; ++i)
			cur.insertText(text);
		doc.m_blockData.m_index = 0;	//	�L���b�V������
		for(int i = 0; i <= nLines; ++i) {
			BlockData d = doc.findBlockData(i*10);
			ut.ut_test_equal(i, d.m_index);
			ut.ut_test_equal(i*10, d.m_position);
		}
		DocBlock block = doc.findBlockByNumber(15);
		doc.m_blockData.m_index = block.blockNumber();		//	�L���b�V���L��
		doc.m_blockData.m_position = block.position();
		for(int i = 0; i <= nLines; ++i) {
			BlockData d = doc.findBlockData(i*10);
			ut.ut_test_equal(i, d.m_index);
			ut.ut_test_equal(i*10, d.m_position);
		}
	}
	if( 1 ) {		//	findBlockIndex() �e�X�g�A�L���b�V���������ɂ���ꍇ
		TextDocument doc;
		DocCursor cur(&doc);		//	�擪�ʒu
		const QString text("123456789\n");		//	10byte �e�L�X�g
		const int nLines = 100;
		for(int i = 0; i < nLines; ++i)
			cur.insertText(text);
		DocBlock block = doc.findBlockByNumber(50);
		doc.m_blockData.m_index = block.blockNumber();
		doc.m_blockData.m_position = block.position();
		index_t blockPos;
		//index_t ix = doc.findBlockIndex(26*10, &blockPos);
		for(int i = 0; i <= nLines; ++i) {
			index_t blockPos;
			index_t ix = doc.findBlockIndex(i*10, &blockPos);
			ut.ut_test_equal(i, ix);
			ut.ut_test_equal(i*10, blockPos);
		}
	}
	if( 1 ) {
		TextDocument doc;
		doc.setPlainText(QString("xyz\n12345\n"));
		DocCursor cur(&doc);		//	�擪�ʒu
		cur.setPosition(3, DocCursor::KeepAnchor);
		doc.insertText(cur, QString("1234567"), true);
		ut.ut_test_equal(0, cur.anchor());
		ut.ut_test_equal(7, cur.position());
	}
}

void test_TextView()
{
	CUnitTest ut("TextView");
#if 0
	if( 1 ) {
		TextView view;
		TextDocument *doc = view.document();
		ut.ut_test( doc->isEmpty() );
		ut.ut_test_equal(0, doc->size());
		ut.ut_test_equal(1, view.blockCount());
		ViewCursor cur(&view);
		cur.insertText(QString("123\r\n������\n"));
		view.buildBlocks();
		ut.ut_test_equal(3, view.blockCount());
		ut.ut_test_equal(5, view.blockSize(0));
		ut.ut_test_equal(10, view.blockSize(1));
		ut.ut_test_equal(0, view.blockSize(2));

		ut.ut_test( !doc->isEmpty() );
		ut.ut_test_equal(15, doc->size());
		cur.setPosition(0);
		ut.ut_test_equal(0, cur.position());
		ut.ut_test_equal(0, cur.anchor());
		cur.movePosition(DocCursor::Right);
		ut.ut_test_equal(1, cur.position());
	}
	if( 1 ) {
		TextView view;
		TextDocument *doc = view.document();
		doc->setPlainText(QString("123\r\n����������������\n"));
		QFontMetrics fm = view.fontMetrics();
		view.m_viewportWidth = fm.width(QString("����������"));
		view.buildBlocks();
		ut.ut_test_equal(4, view.blockCount());
		ut.ut_test_equal(5, view.blockSize(0));
		ut.ut_test_equal(15, view.blockSize(1));	//	����������
		ut.ut_test_equal(10, view.blockSize(2));	//	������
		ut.ut_test_equal(0, view.blockSize(3));
	}
#endif
	if( 1 ) {		//	�}���`�J�[�\���F�����}��
		std::vector<ViewCursor*> v;
		TextView view;
		TextDocument *doc = view.document();
		doc->setPlainText(QString("\n\n\n\n\n"));	//	���s���T�s
		ViewCursor cur(&view);
		view.addToMultiCursor(cur);			//	1�s��
		cur.movePosition(DocCursor::Down);
		view.addToMultiCursor(cur);			//	2�s��
		cur.movePosition(DocCursor::Down);
		view.addToMultiCursor(cur);			//	3�s��
		cur.movePosition(DocCursor::Down);
		view.addToMultiCursor(cur);			//	4�s��
		cur.movePosition(DocCursor::Down);
		view.setTextCursor(cur);			//	���C���J�[�\���F5�s��
		view.insertText("=");
		view.getAllCursor(v);
		ut.ut_test_equal(5, v.size());
		ut.ut_test_equal(1, v[0]->position());
		ut.ut_test_equal(0, v[0]->blockData().index());
		ut.ut_test_equal(0, v[0]->blockData().position());
		ut.ut_test_equal(3, v[1]->position());
		ut.ut_test_equal(1, v[1]->blockData().index());
		ut.ut_test_equal(2, v[1]->blockData().position());
		ut.ut_test_equal(5, v[2]->position());
		ut.ut_test_equal(2, v[2]->blockData().index());
		ut.ut_test_equal(4, v[2]->blockData().position());
		ut.ut_test_equal(7, v[3]->position());
		ut.ut_test_equal(3, v[3]->blockData().index());
		ut.ut_test_equal(6, v[3]->blockData().position());
		ut.ut_test_equal(9, v[4]->position());
		ut.ut_test_equal(4, v[4]->blockData().index());
		ut.ut_test_equal(8, v[4]->blockData().position());
		ut.ut_test_equal(QString("=\n=\n=\n=\n=\n"), doc->toPlainText());
		view.undo();
		ut.ut_test_equal(QString("\n\n\n\n\n"), doc->toPlainText());
		view.redo();
		ut.ut_test_equal(QString("=\n=\n=\n=\n=\n"), doc->toPlainText());
	}
	if( 1 ) {		//	�}���`�J�[�\���F�I����ԕ����}��
		std::vector<ViewCursor*> v;
		TextView view;
		TextDocument *doc = view.document();
		doc->setPlainText(QString("abc\nxyz\n123\nABC\n"));
		ViewCursor cur(&view);
		cur.setPosition(3, DocCursor::KeepAnchor);		//	1�s�� abc �I��
		view.addToMultiCursor(cur);
		cur.setPosition(8);
		cur.setPosition(11, DocCursor::KeepAnchor);	//	3�s�� 123 �I��
		view.setTextCursor(cur);			//	���C���J�[�\��
		view.insertText("=");
		ut.ut_test_equal(QString("=\nxyz\n=\nABC\n"), doc->toPlainText());
		view.undo();
		ut.ut_test_equal(QString("abc\nxyz\n123\nABC\n"), doc->toPlainText());
		view.redo();
		ut.ut_test_equal(QString("=\nxyz\n=\nABC\n"), doc->toPlainText());
	}
	if( 1 ) {		//	�}���`�J�[�\���FDelete �폜
		std::vector<ViewCursor*> v;
		TextView view;
		TextDocument *doc = view.document();
		doc->setPlainText(QString("abc\nabc\nabc\nabc\nabc\n"));	//	�T�s
		ViewCursor cur(&view);
		view.addToMultiCursor(cur);			//	1�s��
		cur.movePosition(DocCursor::Down);
		view.addToMultiCursor(cur);			//	2�s��
		cur.movePosition(DocCursor::Down);
		view.addToMultiCursor(cur);			//	3�s��
		cur.movePosition(DocCursor::Down);
		view.addToMultiCursor(cur);			//	4�s��
		cur.movePosition(DocCursor::Down);
		view.setTextCursor(cur);			//	���C���J�[�\���F5�s��
		view.deleteChar();
		ut.ut_test_equal(QString("bc\nbc\nbc\nbc\nbc\n"), doc->toPlainText());
		view.undo();
		ut.ut_test_equal(QString("abc\nabc\nabc\nabc\nabc\n"), doc->toPlainText());
		view.redo();
		ut.ut_test_equal(QString("bc\nbc\nbc\nbc\nbc\n"), doc->toPlainText());
	}
	if( 1 ) {		//	�}���`�J�[�\���FDelete �I��͈͍폜
		std::vector<ViewCursor*> v;
		TextView view;
		TextDocument *doc = view.document();
		doc->setPlainText(QString("abc\nabc\nabc\nabc\nabc\n"));	//	�T�s
		ViewCursor cur(&view);
		cur.setPosition(3, DocCursor::KeepAnchor);		//	1�s�� abc �I��
		view.addToMultiCursor(cur);
		cur.setPosition(8);
		cur.setPosition(11, DocCursor::KeepAnchor);	//	3�s�� abc �I��
		view.setTextCursor(cur);			//	���C���J�[�\��
		view.deleteChar();
		ut.ut_test_equal(QString("\nabc\n\nabc\nabc\n"), doc->toPlainText());
		view.undo();
		ut.ut_test_equal(QString("abc\nabc\nabc\nabc\nabc\n"), doc->toPlainText());
		view.redo();
		ut.ut_test_equal(QString("\nabc\n\nabc\nabc\n"), doc->toPlainText());
	}
	if( 1 ) {		//	�}���`�J�[�\���F���[�e�C�g
		std::vector<ViewCursor*> v;
		TextView view;
		TextDocument *doc = view.document();
		doc->setPlainText(QString("abc\n1234567\n"));
		ViewCursor cur(&view);
		cur.setPosition(3, DocCursor::KeepAnchor);
		view.addToMultiCursor(cur);
		cur.setPosition(4);
		cur.setPosition(11, DocCursor::KeepAnchor);
		view.setTextCursor(cur);			//	���C���J�[�\��
		view.insertText(QString("\t"), true);	//	���[�e�C�g
		ut.ut_test_equal(8, doc->blockSize(0));
		ut.ut_test_equal(4, doc->blockSize(1));
		view.getAllCursor(v);
		ut.ut_test_equal(2, v.size());
		ut.ut_test_equal(7, v[0]->position());
		ut.ut_test_equal(0, v[0]->blockData().index());
		ut.ut_test_equal(0, v[0]->blockData().position());
		ut.ut_test_equal(11, v[1]->position());
		ut.ut_test_equal(1, v[1]->blockData().index());
		ut.ut_test_equal(8, v[1]->blockData().position());
		ut.ut_test_equal(QString("1234567\nabc\n"), doc->toPlainText());
		view.undo();
		ut.ut_test_equal(QString("abc\n1234567\n"), doc->toPlainText());
		view.redo();
		ut.ut_test_equal(QString("1234567\nabc\n"), doc->toPlainText());
	}
	if( 1 ) {		//	buildBlocks �e�X�g
		TextView view;
		TextDocument *doc = view.document();
		doc->setPlainText(QString("123\r\n����������������\n��������������������������\n"));
		ut.ut_test_equal(4, view.blockCount());
		ViewBlock block = view.firstBlock();
		ut.ut_test( !block.isLayouted() );
		ut.ut_test_equal(5, block.size());
		ut.ut_test_equal(QString("123\r\n"), block.text());
		block = view.lastBlock();
		ut.ut_test( !block.isLayouted() );
		ut.ut_test_equal(3, block.index());
		ut.ut_test_equal(5+25+40, block.position());
		ut.ut_test_equal(0, block.size());
		ut.ut_test_equal(QString(""), block.text());

		QFontMetrics fm = view.fontMetrics();
		const int wd = fm.width(QString("����������"));
		const int ht = fm.lineSpacing() * 10;
		DocCursor cur(doc);
		view.buildBlocks(cur.block(), wd, ht);		//	�ŏ�����Ō�܂Ń��C�A�E�g
		ut.ut_test_equal(7, view.blockCount());

		block = view.lastBlock();
		ut.ut_test( block.isLayouted() );
		ut.ut_test_equal(6, block.index());
		ut.ut_test_equal(5+25+40, block.position());
		ut.ut_test_equal(0, block.size());
		ut.ut_test_equal(QString(""), block.text());
		block = view.firstBlock();
		ut.ut_test( block.isLayouted() );
		ut.ut_test( block.isValid() );
		ut.ut_test_equal(0, block.docIndex());
		ut.ut_test_equal(5, block.size());
		ut.ut_test_equal(QString("123\r\n"), block.text());
		block = block.next();
		ut.ut_test( block.isValid() );
		ut.ut_test( block.isLayouted() );
		ut.ut_test_equal(1, block.docIndex());
		ut.ut_test_equal(15, block.size());
		ut.ut_test_equal(QString("����������"), block.text());
		block = block.next();
		ut.ut_test( block.isValid() );
		ut.ut_test( block.isLayouted() );
		ut.ut_test_equal(1, block.docIndex());
		ut.ut_test_equal(10, block.size());
		ut.ut_test_equal(QString("������\n"), block.text());
		block = block.next();
		ut.ut_test( block.isValid() );
		ut.ut_test( block.isLayouted() );
		ut.ut_test_equal(2, block.docIndex());
		ut.ut_test_equal(15, block.size());
		ut.ut_test_equal(QString("����������"), block.text());
		block = block.next();
		ut.ut_test( block.isValid() );
		ut.ut_test( block.isLayouted() );
		ut.ut_test_equal(2, block.docIndex());
		ut.ut_test_equal(15, block.size());
		ut.ut_test_equal(QString("����������"), block.text());
		block = block.next();
		ut.ut_test( block.isValid() );
		ut.ut_test( block.isLayouted() );
		ut.ut_test_equal(2, block.docIndex());
		ut.ut_test_equal(10, block.size());
		ut.ut_test_equal(QString("������\n"), block.text());
		block = block.next();
		ut.ut_test( block.isValid() );
		ut.ut_test( block.isLayouted() );
		ut.ut_test_equal(3, block.docIndex());
		ut.ut_test_equal(0, block.size());
		ut.ut_test_equal(QString(""), block.text());
		block = block.next();
		ut.ut_test_equal(4, block.docIndex());
		ut.ut_test( !block.isValid() );
	}
	if( 1 ) {		//	buildBlocks �e�X�g
		TextView view;
		TextDocument *doc = view.document();
		doc->setPlainText(QString("123\n����������������\n��������������������������\n"));
		ut.ut_test_equal(4, view.blockCount());
		QFontMetrics fm = view.fontMetrics();
		const int wd = fm.width(QString("����������"));
		const int ht = fm.lineSpacing() * 2;		//	2�s��
		DocCursor cur(doc);
		cur.setPosition(4);							//	2�s�ڐ擪
		view.buildBlocks(cur.block(), wd, ht);		//	�ŏ����烌�C�A�E�g
		ut.ut_test_equal(5, view.blockCount());

	}
}
