//----------------------------------------------------------------------
//
//			File:			"TestViCommands.cpp"
//			Created:		24-Feb-2011
//			Author:			Nobuhide Tsuda
//			Description:	自動テスト関数実装
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
#include "TestViCommands.h"
#include "mainwindow.h"
#include "ViEngine.h"
#include "TextDocument.h"
#include "TextView.h"
#include "charEncoding.h"

#if 0
void readToBuffer(QTextStream &in, ViEngine *viEngine, QString &buffer);
void checkBuffer(QTextStream &in, MainWindow *mainWindow, ViEngine *viEngine, QString &buffer);
#endif

struct TestFailedItem {
	QString	m_path;
	QString	m_message;
	int		m_lineNum;
public:
	TestFailedItem() : m_lineNum(0) {}
	TestFailedItem(const QString &path, int lineNum, const QString &messsage = QString())
		: m_path(path), m_lineNum(lineNum), m_message(messsage)
		{}
};

class TestViCommands
{
public:
	TestViCommands(MainWindow *mainWindow, ViEngine *viEngine, const QString &testFileName);

public:
	void	doTest();

protected:
	void	readToBuffer(QTextStream &in);
	void	checkBuffer(QTextStream &in);
	void	checkCursor();

	void	skipBlank();
	int		getNumber(/*int = 0*/);
	QString	getDQString();

private:
	MainWindow	*m_mainWindow;
	ViEngine	*m_viEngine;
	const QString &m_testFileName;
	int			m_lineNum;
	QString		m_buffer;
	int			m_ix;
	int			m_nTested;
	int			m_nFailed;
	QVector<TestFailedItem>	m_failedPos;
};

TestViCommands::TestViCommands(MainWindow *mainWindow, ViEngine *viEngine, const QString &testFileName)
	: m_mainWindow(mainWindow), m_viEngine(viEngine), m_testFileName(testFileName)
{
}
void TestViCommands::doTest()
{
	if( m_testFileName.isEmpty() ) {
		m_mainWindow->doOutput("need fileName.\n");
		m_mainWindow->doOutput("usage: :test <testFileName>\n");
		return;
	}
#if 1
	QString buffer;
	QString errorString;
	if( !::loadFile(m_testFileName, buffer, errorString) ) {
		m_mainWindow->doOutput(QObject::tr("can't open '%1'.\n").arg(m_testFileName));
		return;
	}
	m_mainWindow->doOutput(QObject::tr("testing '%1' (Qt verdion %2)\n")
							.arg(m_testFileName).arg(QT_VERSION_STR));
	QTextStream in(&buffer);
#else
	QFile f(m_testFileName);
	if( !f.open(QIODevice::ReadOnly) ) {
		m_mainWindow->doOutput(QObject::tr("can't open '%1'.\n").arg(m_testFileName));
		return;
	}
	QTextStream in(&f);
#endif
	m_nTested = 0;
	m_nFailed = 0;
	m_lineNum = 0;
	bool skipReadLine = false;
	while( !in.atEnd() ) {
		if( !skipReadLine ) {
			m_buffer = in.readLine();
			++m_lineNum;
		} else
			skipReadLine = false;
		if( m_buffer.isEmpty() ) continue;
		if( m_buffer[0] == '<' ) {
			readToBuffer(in);
			skipReadLine = true;
		} else if( m_buffer[0] == '>' ) {
			checkBuffer(in);
			skipReadLine = true;
		} else if( m_buffer[0] == '/' || m_buffer[0] == '/' ) {
			m_viEngine->doFind(m_buffer.mid(1), m_buffer[0] == '/');
		//} else if( m_buffer[0] == ':' ) {
		//	m_viEngine->doExCommand(m_buffer.mid(1));
		} else if( m_buffer == ".end" ) {
			break;
		} else {
			m_ix = 0;
			while( m_ix < m_buffer.length() ) {
				QChar ch = m_buffer[m_ix++];
				if( ch == ';' || ch == '#' || ch == '/' )	//	行コメント
					break;
				if( ch == ' ' || ch == '\t' )
					continue;
				if( ch == '=' )
					checkCursor();
				else if( ch == '"' ) {
					QString cmd = getDQString();
					m_viEngine->doViCommand(cmd);
				} else if( ch.isDigit() ) {
					--m_ix;
					QChar cmd(getNumber(/*ch.unicode() - '0'*/));
					m_viEngine->doViCommand(cmd.unicode());
				} else if( ch == ':' ) {
					m_viEngine->doExCommand(m_buffer.mid(m_ix));
					break;
				}
			}
		}
	}
	qDebug() << "test finished.";
	m_mainWindow->doOutput("\n");
	m_mainWindow->doOutput(QObject::tr("%1 failed / %2 tested.\n").arg(m_nFailed).arg(m_nTested));

	for(QVector<TestFailedItem>::const_iterator itr = m_failedPos.begin(), iend = m_failedPos.end();
		itr != iend; ++itr)
	{
		m_mainWindow->doOutput(QObject::tr("\"%1\"(%2):%3\n")
								.arg(itr->m_path).arg(itr->m_lineNum).arg(itr->m_message));
	}
}
void TestViCommands::skipBlank()
{
	while( m_ix < m_buffer.length() && (m_buffer[m_ix] == ' ' ||  m_buffer[m_ix] == '\t') )
		++m_ix;
}
int TestViCommands::getNumber(/*int value*/)
{
	int value = 0;
	skipBlank();
	if( m_ix + 1 < m_buffer.length() && m_buffer[m_ix] == '0' &&
		m_buffer[m_ix+1].toLower() == 'x' )
	{
		m_ix += 2;
		int t;
		while( m_ix < m_buffer.length() ) {
			QChar ch = m_buffer[m_ix];
			if( ch.isDigit() )
				t = ch.unicode() - '0';
			else {
				ch = ch.toLower();
				if( ch < 'a' || ch > 'f' )
					break;
				t = ch.unicode() - 'a' + 10;
			}
			value = value * 16 + t;
			++m_ix;
		}
	} else {
		while( m_ix < m_buffer.length() && m_buffer[m_ix].isDigit() )
			value = value * 10 + m_buffer[m_ix++].unicode() - '0';
	}
	skipBlank();
	return value;
}
QString TestViCommands::getDQString()
{
	QString text;
	while( m_ix < m_buffer.length() ) {
		QChar ch = m_buffer[m_ix++];
		if( ch == '"' ) {
			if( m_ix < m_buffer.length() && m_buffer[m_ix] == '"' )
				++m_ix;
			else
				break;
		}
		text += ch;
	}
	return text;
}
void TestViCommands::checkCursor()
{
	int line = getNumber();
	int offset = getNumber();
	TextView *editor = m_viEngine->editor();
	DocCursor cur = editor->textCursor();
	if( line == cur.blockNumber() + 1 && offset == cur.prevCharsCount() )
		m_mainWindow->doOutput(".");
	else {
		qDebug() << "failed at checkCursor()";
		m_mainWindow->doOutput("F");
		const QString mess = QObject::tr("cursor pos %1 %2 expected, but %3 %4.")
						.arg(line).arg(offset)
						.arg(cur.blockNumber() + 1).arg(cur.positionInBlock());
		m_failedPos.push_back(TestFailedItem(m_testFileName, m_lineNum, mess));
				
		++m_nFailed;
	}
	++m_nTested;
}
void TestViCommands::readToBuffer(QTextStream &in)
{
	TextView *editor = m_viEngine->editor();
	editor->document()->clear();
	editor->clear();
	ViewCursor cur = editor->textCursor();
	for(;;) {
		cur.insertText(m_buffer.mid(1) + "\n");
		//m_mainWindow->printBuffer();
		if( in.atEnd() ) {
			m_buffer.clear();
			break;
		}
		m_buffer = in.readLine();
		++m_lineNum;
		if( m_buffer.isEmpty() || m_buffer[0] != '<' ) {
			cur.movePosition(DocCursor::StartOfDocument);
			editor->setTextCursor(cur);
			return;
		}
	}
}
void TestViCommands::checkBuffer(QTextStream &in)
{
	TextView *editor = m_viEngine->editor();
	DocBlock block = editor->document()->firstBlock();
	while( !m_buffer.isEmpty() && m_buffer[0] == '>' ) {
		const QString text = block.text();
		const size_t len = text.length() - block.newlineLength();
		if( block.isValid() && text.left(len) == m_buffer.mid(1, len) )
			m_mainWindow->doOutput(".");
		else {
			m_mainWindow->doOutput("F");
			QString mess = QObject::tr("'%1' expected, but '%2'.").arg(m_buffer.mid(1)).arg(block.text());
			m_failedPos.push_back(TestFailedItem(m_testFileName, m_lineNum, mess));
			++m_nFailed;
		}
		++m_nTested;
		if( block.isValid() )
			block = block.next();
		if( in.atEnd() ) {
			m_buffer.clear();
			break;
		}
		m_buffer = in.readLine();
		++m_lineNum;
	}
	while( block.isValid() ) {
		m_mainWindow->doOutput("F");
		QString mess = QObject::tr("'%1' is NOT expected.").arg(block.text());
		m_failedPos.push_back(TestFailedItem(m_testFileName, m_lineNum, mess));
		++m_nFailed;
		++m_nTested;
		block = block.next();
	}
}

void testViCommands(MainWindow *mainWindow, ViEngine *viEngine, const QString &fileName)
{
	TestViCommands tvc(mainWindow, viEngine, fileName);
	tvc.doTest();
}


#if 0
void testViCommands(MainWindow *mainWindow, ViEngine *viEngine, const QString &fileName)
{
	if( fileName.isEmpty() ) {
		mainWindow->doOutput("need fileName.\n");
		mainWindow->doOutput("usage: :test <testFileName>\n");
		return;
	}
	QFile f(fileName);
	if( !f.open(QIODevice::ReadOnly) ) {
		mainWindow->doOutput(QObject::tr("can't open '%1'.\n").arg(fileName));
		return;
	}
	QTextStream in(&f);
	QString buffer;
	bool skipReadLine = false;
	while( !in.atEnd() ) {
		if( !skipReadLine )
			buffer = in.readLine();
		else
			skipReadLine = false;
		//mainWindow->doOutput(buffer);
		//mainWindow->doOutput("\n");
		if( buffer.isEmpty() ) continue;
		if( buffer[0] == '<' ) {
			readToBuffer(in, viEngine, buffer);
			skipReadLine = true;
		} else if( buffer[0] == '>' ) {
			checkBuffer(in, mainWindow, viEngine, buffer);
			skipReadLine = true;
		}
	}
}
void readToBuffer(QTextStream &in, ViEngine *viEngine, QString &buffer)
{
	TextView *editor = viEngine->editor();
	editor->document()->clear();
	ViewCursor cur = editor->textCursor();
	for(;;) {
		cur.insertText(buffer.mid(1));
		cur.insertText("\n");
		if( in.atEnd() ) {
			buffer.clear();
			break;
		}
		buffer = in.readLine();
		if( buffer.isEmpty() || buffer[0] != '<' ) {
			cur.movePosition(ViewCursor::Start);
			editor->setTextCursor(cur);
			return;
		}
	}
}
void checkBuffer(QTextStream &in, MainWindow *mainWindow, ViEngine *viEngine, QString &buffer)
{
	TextView *editor = viEngine->editor();
	QTextBlock block = editor->document()->firstBlock();
	while( !buffer.isEmpty() && buffer[0] == '>' ) {
		if( block.isValid() && block.text() == buffer.mid(1) )
			mainWindow->doOutput(".");
		else
			mainWindow->doOutput("F");
		if( block.isValid() )
			block = block.next();
		if( in.atEnd() ) {
			buffer.clear();
			break;
		}
		buffer = in.readLine();
	}
	while( block.isValid() ) {
		mainWindow->doOutput("F");
		block = block.next();
	}
}
#endif

#if 0
TestViCommands::TestViCommands(QObject *parent)
	: QObject(parent)
{

}

TestViCommands::~TestViCommands()
{

}
#endif
