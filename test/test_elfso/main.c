#include <ilib/ilib.h>

int (* add)(int, int);
int (* add1)(int, int);
int (* get_value0)(void);

int32 main(int32 argc, ASCCHAR * argv[])
{
	uint32 ctx_idx = ILLoadELFSO("DA:/isystem/slib/test.so");
	uint32 * value0 = ILGetELFSOSymbol(ctx_idx, "value0");
	printf("Value0: 0x%x, 0x%x\n", value0, *value0);
	uint32 * value1 = ILGetELFSOSymbol(ctx_idx, "value1");
	printf("Value1: 0x%x, 0x%x\n", value1, *value1);
	add = ILGetELFSOSymbol(ctx_idx, "add");
	printf("%d\n", add(100, 23));
	add1 = ILGetELFSOSymbol(ctx_idx, "add1");
	printf("%d\n", add1(12340, 5));
	get_value0 = ILGetELFSOSymbol(ctx_idx, "get_value0");
	printf("0x%x\n", get_value0());
	return 0;
}
