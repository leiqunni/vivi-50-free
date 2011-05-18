#pragma once

//----------------------------------------------------------------------
//
//			File:			"ViEngine.h"
//			Created:		24-Feb-2011
//			Author:			Nobuhide Tsuda
//			Description:	vi�G���W���N���X�錾
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
	void	doExCommand(const QString &);			//	�����ɂ́A�擪�� : ���܂܂Ȃ������񂪓n��
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

	void	getLineNumbers(const QString &, int &);		//	ex command �̍s�ԍ��͈͕��������
	bool	getLineNumber(const QString &, int &, int &);	//	�l����
	bool	getAddSubTerm(const QString &, int &, int &);	//	�l [{+|-} �l]...
	QString	getExCommand(const QString &, int &, bool &exclamation);

private:
	bool	m_noRepeatCount;		//	�J��Ԃ��񐔎w��s�ifor fFtT���j
	bool	m_redoRecording;		//	.(redo) �̂��߂ɑ}��������L�^��
	bool	m_redoing;				//	.(redo) ���s��
	bool	m_noInsModeAtImeOpenStatus;		//	IME ON�EOFF���ς���Ă��}�����[�h�ɑJ�ڂ��Ȃ�
	bool	m_joinPrevEditBlock;		//	
	bool	m_findForward;			//	����������
	bool	m_moveByLine;			//	�s�P�ʈړ� } { H M L G
	Mode	m_mode;
	int		m_repeatCount;
	int		m_repeatCount2;			//	c2w �̂悤�ɃR�}���h�̊ԂɎw�肳�ꂽ�J��Ԃ���
	int		m_redoRepeatCount;
	int		m_insCount;				//	�}����
	int		m_cdyPos;				//	c|d|y �������ꂽ���̃J�[�\���ʒu
	//int		m_cursorX;				//	�u���b�N�擪�����x�ψ�
	ushort	m_cmdPrefix;			//	]] [[ �̗l�ȂQ�X�g���[�N�R�}���h�̍ŏ��̕���
	ushort	m_cdyCmd;				//	c | d | y �R�}���h
	ushort	m_lastFTCmd;			//	'f' or 'F' or't' or 'T'
	QChar	m_lastFTChar;			//	fFtT ��������
	QString	m_findString;			//	�������K�\��������
	QString	m_cmdString;			//	���͒��R�}���h�e�L�X�g
	QString	m_redoCmd;				//	.(redo) �ōĎ��s����R�}���h
	QString	m_insertedText;			//	�}�����ꂽ�e�L�X�g
	QString	m_message;
	bool	m_yankByLine;			//	�����N�o�b�t�@�F�s�P��
	QString	m_yankBuffer;			//	���������N�o�b�t�@
	QStringList	m_exCommands;		//	���͂��ꂽ ex-command ������
	TextView	*m_view;

	//	undone C �s�ԍ��֌W�͓Ɨ������\���̂ɂ��������悳��
	int		m_nLineNum;				//	�w�肳�ꂽ�s�ԍ���
	int		m_lineNum1;				//	ex command range from
	int		m_lineNum2;				//	ex command range to
	int		m_curLineNum;			//	. lineNumber
};

#endif // VIENGINE_H
