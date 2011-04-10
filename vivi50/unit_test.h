//----------------------------------------------------------------------
//
//			File:			"unit_test.h"
//			Created:		04-Jun-2009
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

#pragma once

//#include	<iostream>
#include	<string>
#include	<vector>
//#include	<boost/lexical_cast.hpp>
#include	<QDebug>

typedef const char cchar;
typedef unsigned int uint;

//typedef void (*OutputFunc)(cchar*);
typedef void (*OutputFunc)(const QString &);
extern OutputFunc g_ut_output;

void ut_output(const QString &);
void ut_set_output(OutputFunc f);

extern uint g_total_test_count;
extern uint g_total_fail_count;

typedef const char cchar;

#define		ut_test(b)			_test(b, __FILE__, __LINE__)
#define		ut_test_equal(e, v)	_test_equal(e, v, __FILE__, __LINE__)

namespace unit_test {
	extern size_t n_test;			//	�g�[�^���e�X�g��
	extern size_t n_failed;			//	�g�[�^�����s��
}

struct SFailedInfo
{
	QString		m_fileName;
	int				m_lineNum;
	QString		m_message;
public:
	SFailedInfo(const QString &fileName, int lineNum, QString mess="")
		: m_fileName(fileName), m_lineNum(lineNum), m_message(mess) {};
	SFailedInfo(const SFailedInfo &src)
		: m_fileName(src.m_fileName), m_lineNum(src.m_lineNum), m_message(src.m_message) {};
	~SFailedInfo() {};

	const SFailedInfo &operator=(const SFailedInfo &src)
	{
		m_fileName = src.m_fileName;
		m_lineNum = src.m_lineNum;
		m_message = src.m_message;
		return *this;
	};
};


class CUnitTest
{
	QString	m_unitName;
	int		m_testCount;		//	�e�X�g��
	int		m_failCount;		//	���s��
	//CVOStream *m_out;		//	�o�̓X�g���[��

	std::vector<SFailedInfo>	m_logs;
public:
	CUnitTest(cchar *unitName /*, CVOStream* = &g_vostream*/);
	~CUnitTest(void);

public:
	//void test(bool b) { _test(b, __FILE__, __LINE__); }
	void _test(bool b, cchar *fileName, int lineNum);

#if 0
	template<typename T>
	void test_equal(const T &exp, const T &v)
	{
		_test_equal(exp, v, __FILE__, __LINE__);
	}
#endif
	template<typename T>
	void _test_equal(const T &exp, const T &v, cchar *fileName, int lineNum)
	{
		m_testCount += 1;
		unit_test::n_test += 1;
		if( exp == v ) {
			g_ut_output(".");
		} else {
			m_failCount += 1;
			unit_test::n_failed += 1;
			g_ut_output("F");

			m_logs.push_back(SFailedInfo(fileName, lineNum));
		}
	}
	void _test_equal(int exp, int v, cchar *fileName = __FILE__, int lineNum = __LINE__)
	{
		m_testCount += 1;
		unit_test::n_test += 1;
		if( exp == v ) {
			g_ut_output(".");
		} else {
			m_failCount += 1;
			unit_test::n_failed += 1;
			g_ut_output("F");

			QString temp = QString("%1 �����҂��܂����� %2 �ł����B").arg(exp).arg(v);
			qDebug() << temp;
#if 0
			std::string temp;
			temp += boost::lexical_cast<std::string>(exp);
			temp += " �����҂��܂����� ";
			temp += boost::lexical_cast<std::string>(v);
			temp += " �ł����B";
#endif
			m_logs.push_back(SFailedInfo(fileName, lineNum, temp));
		}
	}
	void _test_equal(const QString &exp, const QString &v, cchar *fileName, int lineNum)
	{
		m_testCount += 1;
		unit_test::n_test += 1;
		if( exp == v ) {
			g_ut_output(".");
		} else {
			m_failCount += 1;
			unit_test::n_failed += 1;
			g_ut_output("F");

			QString temp = QString("'%1' �����҂��܂����� '%2' �ł����B").arg(exp).arg(v);
			m_logs.push_back(SFailedInfo(fileName, lineNum, temp));
		}
	}
	void _test_equal(cchar *exp, cchar *v, cchar *fileName, int lineNum)
	{
		m_testCount += 1;
		unit_test::n_test += 1;
		if( !strcmp(exp, v) ) {
			g_ut_output(".");
		} else {
			m_failCount += 1;
			unit_test::n_failed += 1;
			g_ut_output("F");

			QString temp = QString("%1 �����҂��܂����� %2 �ł����B").arg(exp).arg(v);
			m_logs.push_back(SFailedInfo(fileName, lineNum, temp));
		}
	}

};
