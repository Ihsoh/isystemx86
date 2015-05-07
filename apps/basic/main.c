#include <ilib/ilib.h>
#include <baslanglib/baslanglib.h>
#include "die.h"

int main(int argc, char * argv[])
{
	BASLANGLEnvironment env;
	env.baslangl_malloc = malloc;
	env.baslangl_calloc = calloc;
	env.baslangl_free = free;
	if(!baslangl_init(&env))
		die("Cannot initialize 'baslanglib'.");
	return 0;
}
