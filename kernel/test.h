/**
	@File:			test.h
	@Author:		Ihsoh
	@Date:			2015-11-14
	@Description:
		提供一些用于测试的宏。
*/

#ifndef	_TEST_H_
#define	_TEST_H_

#define	TEST_CONTEXT(_name)	\
	static CASCTEXT _failed_func	= NULL;	\
	static int32 _failed_no			= -1;	\
	static CASCTEXT _test_file		= __FILE__;	\
	static int32 _failed_ln			= -1;	\
	CASCTEXT _name##_get_failed_func(void) { return _failed_func; }	\
	int32 _name##_get_failed_no(void) { return _failed_no; }	\
	CASCTEXT _name##_get_test_file(void) { return _test_file; }	\
	int32 _name##_get_failed_ln(void) { return _failed_ln; }

#define	TEST_FAILED()	\
	{ _failed_ln = __LINE__; goto _test_failed; }

#define	TEST_FUNC_BEGIN(_func)	\
	static BOOL _test_##_func(void) {

#define	TEST_FUNC_END	\
		return TRUE;	\
	_test_failed:	\
		_failed_func = __FUNCTION__;	\
		return FALSE;	\
	}

#define	TEST_BEGIN(_no)	\
	{ _failed_no = _no;

#define	TEST_END	\
	_failed_no = -1; }

#define	TEST(_func)	(_test_##_func())

#define	RUN_UNIT_TEST(_name)	\
	{	\
		if(_name##Test())	\
		{	\
			ScrPrintString(_name##_get_test_file());	\
			ScrPrintStringWithProperty(" [OK]\n", CC_GREEN);	\
		}	\
		else	\
		{	\
			ScrPrintString(_name##_get_test_file());	\
			ScrPrintStringWithProperty(" [FAILED]\n", CC_RED);	\
			ScrPrintString(_name##_get_failed_func());	\
			ScrPrintString(":");	\
			printn(_name##_get_failed_ln());	\
			ScrPrintString(":");	\
			printn(_name##_get_failed_no());	\
			ScrPrintString("\n");	\
		}	\
	}

#endif
