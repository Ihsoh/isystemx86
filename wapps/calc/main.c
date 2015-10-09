#include <ilib/ilib.h>

#define	_TITLE		"Calc"

#define	_WIDTH		215
#define	_HEIGHT		180

#define	_OP_NONE	0
#define	_OP_ADD		1
#define	_OP_SUB		2
#define	_OP_MUL		3
#define	_OP_DIV		4

static int32 _wid = -1;

static int32 _lbl_result	= -1;
static int32 _btn_1			= -1;
static int32 _btn_2			= -1;
static int32 _btn_3			= -1;
static int32 _btn_4			= -1;
static int32 _btn_5			= -1;
static int32 _btn_6			= -1;
static int32 _btn_7			= -1;
static int32 _btn_8			= -1;
static int32 _btn_9			= -1;
static int32 _btn_0			= -1;
static int32 _btn_add		= -1;
static int32 _btn_sub		= -1;
static int32 _btn_mul		= -1;
static int32 _btn_div		= -1;
static int32 _btn_eq		= -1;
static int32 _btn_clear		= -1;
static int32 _btn_dot		= -1;

static ASCCHAR _show[32];
static int32 _op			= _OP_NONE;
static double _oprd			= 0.0;
static double _result		= 0.0;
static BOOL _next_oprd		= FALSE;

static
void
_die(IN CASCTEXT message);

static
void
_init(void);

static
void
_deinit(void);

static
void
_show_result(void);

static
void
_reset_result(void);

static
void
_input(IN ASCCHAR chr);

static
void
_loop(void);

int32
main(	IN int32 argc,
		IN ASCTEXT argv[])
{
	_init();
	_loop();
	return 0;
}

static
void
_die(IN CASCTEXT message)
{
	printf("Error: %s\n", message);
	exit(-1);
}

static
void
_init(void)
{
	if(!ILGIsValid())
		_die(_TITLE" must be run in gui mode.");
	_wid = ILGCreateWindow(	_WIDTH,
							_HEIGHT,
							0xffffffff,
							WINDOW_STYLE_CLOSE | WINDOW_STYLE_MINIMIZE,
							_TITLE);
	if(_wid == -1)
		_die(_TITLE" cannot create a new window.");

	_reset_result();
	
	ILGDrawRect(_wid, 0, 0, _WIDTH, _HEIGHT, 0xffffffff);

	ILGNewLabel(_wid, 10, 10, "0", &_lbl_result);
	ILGNewButton(_wid, 10, 50, " 1 ", &_btn_1);
	ILGNewButton(_wid, 50, 50, " 2 ", &_btn_2);
	ILGNewButton(_wid, 90, 50, " 3 ", &_btn_3);
	ILGNewButton(_wid, 10, 80, " 4 ", &_btn_4);
	ILGNewButton(_wid, 50, 80, " 5 ", &_btn_5);
	ILGNewButton(_wid, 90, 80, " 6 ", &_btn_6);
	ILGNewButton(_wid, 10, 110, " 7 ", &_btn_7);
	ILGNewButton(_wid, 50, 110, " 8 ", &_btn_8);
	ILGNewButton(_wid, 90, 110, " 9 ", &_btn_9);
	ILGNewButton(_wid, 10, 140, " 0 ", &_btn_0);
	ILGNewButton(_wid, 50, 140, " . ", &_btn_dot);
	ILGNewButton(_wid, 90, 140, " = ", &_btn_eq);

	ILGNewButton(_wid, 130, 50, " + ", &_btn_add);
	ILGNewButton(_wid, 130, 80, " - ", &_btn_sub);
	ILGNewButton(_wid, 130, 110, " * ", &_btn_mul);
	ILGNewButton(_wid, 130, 140, " / ", &_btn_div);

	ILGNewButton(_wid, 170, 50, " C ", &_btn_clear);

	ILGSetWindowState(_wid, WINDOW_STATE_SHOW);
	ILGUpdate(_wid);
	ILGFocus(_wid);
	ILRunInBackground();
}

static
void
_deinit(void)
{
	ILGCloseWindow(_wid);
	exit(0);
}

static
void
_show_result(void)
{
	sprintf_s(_show, sizeof(_show), "%f", _result);
	ILGSetText(_wid, _lbl_result, _show);
}

static
void
_reset_result(void)
{
	_result = 0;
	_show_result();
}

static
void
_input(IN ASCCHAR chr)
{
	if(_next_oprd)
	{
		if(chr == '.')
			return;
		_show[0] = chr;
		_show[1] = '\0';
		_next_oprd = FALSE;
	}
	else
	{
		uint32 len = strlen(_show);
		if(len + 1 == sizeof(_show))
			return;
		if(len == 1 && _show[0] == '0' && chr != '.')
			_show[0] = chr;
		else
		{
			if(chr == '.')
			{
				uint32 ui;
				for(ui = 0; ui < len; ui++)
					if(_show[ui] == '.')
						return;
			}
			ASCCHAR buf[2];
			buf[0] = chr;
			buf[1] = '\0';
			strcat(_show, buf);
		}
	}
	ILGSetText(_wid, _lbl_result, _show);
	_result = stod(_show);
}

static
void
_loop(void)
{
	int32 cid = -1;
	uint32 type = 0;
	void * data = NULL;
	while(TRUE)
	{
		if(ILGGetMessage(_wid, &cid, &type, &data))
		{
			if(cid == -1 && type == WINDOW_WILL_CLOSE)
				_deinit();
			if(cid == _btn_0 && type == BUTTON_CLICK)
				_input('0');
			if(cid == _btn_1 && type == BUTTON_CLICK)
				_input('1');
			if(cid == _btn_2 && type == BUTTON_CLICK)
				_input('2');
			if(cid == _btn_3 && type == BUTTON_CLICK)
				_input('3');
			if(cid == _btn_4 && type == BUTTON_CLICK)
				_input('4');
			if(cid == _btn_5 && type == BUTTON_CLICK)
				_input('5');
			if(cid == _btn_6 && type == BUTTON_CLICK)
				_input('6');
			if(cid == _btn_7 && type == BUTTON_CLICK)
				_input('7');
			if(cid == _btn_8 && type == BUTTON_CLICK)
				_input('8');
			if(cid == _btn_9 && type == BUTTON_CLICK)
				_input('9');
			if(cid == _btn_dot && type == BUTTON_CLICK)
				_input('.');
			if(cid == _btn_add && type == BUTTON_CLICK)
			{
				_oprd = _result;
				_op = _OP_ADD;
				_next_oprd = TRUE;
			}
			if(cid == _btn_sub && type == BUTTON_CLICK)
			{
				_oprd = _result;
				_op = _OP_SUB;
				_next_oprd = TRUE;
			}
			if(cid == _btn_mul && type == BUTTON_CLICK)
			{
				_oprd = _result;
				_op = _OP_MUL;
				_next_oprd = TRUE;
			}
			if(cid == _btn_div && type == BUTTON_CLICK)
			{
				_oprd = _result;
				_op = _OP_DIV;
				_next_oprd = TRUE;
			}
			if(cid == _btn_eq && type == BUTTON_CLICK)
			{
				switch(_op)
				{
					case _OP_ADD:
						_result = _oprd + _result;
						break;
					case _OP_SUB:
						_result = _oprd - _result;
						break;
					case _OP_MUL:
						_result = _oprd * _result;
						break;
					case _OP_DIV:
						_result = _oprd / _result;
						break;
				}
				_op = _OP_NONE;
				_next_oprd = TRUE;
				_show_result();
			}
			if(cid == _btn_clear && type == BUTTON_CLICK)
			{
				_op = _OP_NONE;
				_next_oprd = FALSE;
				_reset_result();
			}
		}
	}
}
