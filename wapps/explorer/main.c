#include <ilib/ilib.h>

#define	_TITLE		"ISystem Explorer"

#define	_WIDTH		640
#define	_HEIGHT		480

#define	_VSCROLL_WIDTH	20

static int32 _wid = -1;

static int32 _file_list = -1;

static
void
_die(IN CASCTEXT message)
{
	printf("Error: %s\n", message);
	exit(-1);
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

	BOOL r = ILGNewFileList(
		_wid,
		0, 0,
		640 - _VSCROLL_WIDTH, 480,
		_VSCROLL_WIDTH,
		&_file_list
	);
	printf("\nNEW FILE LIST = %d, %d\n", r, _file_list);

	ILGSetWindowState(_wid, WINDOW_STATE_SHOW);
	ILGUpdate(_wid);
	ILGFocus(_wid);
	ILRunInBackground();
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

int32
main(	IN int32 argc,
		IN ASCTEXT argv[])
{
	_init();
	_loop();
	return 0;
}
