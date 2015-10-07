#include <ilib/ilib.h>

#define	_TITLE		"Calc"

#define	_WIDTH		215
#define	_HEIGHT		180

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

static
void
_die(CASCTEXT message);

static
void
_init(void);

static
void
_deinit(void);

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
_die(CASCTEXT message)
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
		}
	}
}
