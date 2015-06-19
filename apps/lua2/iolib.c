#include "ilib/ilib.h"

#include "lua.h"
#include "luadebug.h"
#include "lualib.h"

static void test(void)
{
	printf("%s\n", lua_check_string(1, "test"));
	lua_pushstring("FUNCTION test!!!");
}

static void lua_printstack (FILE *f)
{
  int level = 0;
  lua_Object func;
  fprintf(f, "Active Stack:\n");
  while ((func = lua_stackedfunction(level++)) != LUA_NOOBJECT) {
    char *name;
    int currentline;
    fprintf(f, "\t");
    switch (*lua_getobjname(func, &name)) {
      case 'g':
        fprintf(f, "function %s", name);
        break;
      case 'f':
        fprintf(f, "`%s' fallback", name);
        break;
      default: {
        char *filename;
        int linedefined;
        lua_funcinfo(func, &filename, &linedefined);
        if (linedefined == 0)
          fprintf(f, "main of %s", filename);
        else if (linedefined < 0)
          fprintf(f, "%s", filename);
        else
          fprintf(f, "function (%s:%d)", filename, linedefined);
      }
    }
    if ((currentline = lua_currentline(func)) > 0)
      fprintf(f, " at line %d", currentline);
    fprintf(f, "\n");
  }
}

static void errorfb (void)
{
  char *s = lua_opt_string(1, "(no messsage)", NULL);
  fprintf(stderr, "lua: %s\n", s);
  lua_printstack(stderr);
}

static struct lua_reg iolib[] = {
{"test", test},
};

void iolib_open (void)
{
  luaI_openlib(iolib, (sizeof(iolib)/sizeof(iolib[0])));
  lua_setfallback("error", errorfb);
}