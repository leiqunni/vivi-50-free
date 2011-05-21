//----------------------------------------------------------------------
//
//			File:			"unit_test.cpp"
//			Created:		04-Jun-2009
//			Author:			�Óc�L�G
//			Description:	�P�̃e�X�g�p���W���[��
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

#include	<QString>
#include	<QDebug>
#include	"unit_test.h"
//#include	<iostream>

uint g_total_test_count = 0;
uint g_total_fail_count = 0;

void ut_output(const QString &mess)
{
	qDebug() << mess;
	//std::cout << mess;
}

OutputFunc g_ut_output = &ut_output;

void ut_set_output(OutputFunc f)
{
	g_ut_output = f;
}

namespace unit_test {
	size_t n_test = 0;			//	�g�[�^���e�X�g��
	size_t n_failed = 0;		//	�g�[�^�����s��
}

//----------------------------------------------------------------------

CUnitTest::CUnitTest(cchar *unitName /*, CVOStream *out*/)
	: m_unitName(unitName), m_testCount(0), m_failCount(0) /*, m_out(out)*/
{
	QString temp(m_unitName);
	temp += ":\n";
	g_ut_output(temp);
}

CUnitTest::~CUnitTest(void)
{
	QString temp;
	if( m_failCount == 0 ) {
		temp.sprintf("\nOK! (%d tested)\n", m_testCount);
		g_ut_output(temp);
	} else {
		temp.sprintf("\n������ NG! (%d failed / %d tested)\n", m_failCount, m_testCount);
		g_ut_output(temp);
		for(std::vector<SFailedInfo>::const_iterator itr = m_logs.begin(); itr != m_logs.end(); ++itr) {
			temp = QString("\"%1\"(%2); %3\n").arg(itr->m_fileName).arg(itr->m_lineNum).arg(itr->m_message);
			g_ut_output(temp);
		}
		g_ut_output("\n");
	}
	g_total_test_count += m_testCount;
	g_total_fail_count += m_failCount;
}

void CUnitTest::_test(bool b, cchar *fileName, int lineNum)
{
#if 1
	m_testCount += 1;
	unit_test::n_test += 1;
	if( b ) {
		g_ut_output(".");
	} else {
		m_failCount += 1;
		unit_test::n_failed += 1;
		g_ut_output("F");

		m_logs.push_back(SFailedInfo(fileName, lineNum, "true expected, but false."));
	}
#endif
}


