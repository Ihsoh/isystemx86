/*
** lua.c
** Linguagem para Usuarios de Aplicacao
** TeCGraf - PUC-Rio
** 28 Apr 93
*/

#include <ilib/ilib.h>

#include "lua.h"
#include "lualib.h"

int main (int argc, char *argv[])
{
 int i;
 if (argc < 2)
 {
  puts ("usage: lua filename [functionnames]");
  return 0;
 }
 iolib_open ();
 strlib_open ();
 mathlib_open ();
 char path[1024];
 strcpy(path, argv[1]);
 ILFixPath(path, path);
 lua_dofile (path);
 /*
 lua_dostring(
 "\
  print(123.311 + 3 * 3 - 10)\n\
  print(tonumber('123.345'))\n\
 ");
 */
 for (i=2; i<argc; i++)
 {
  lua_call (argv[i],0);
 }
 return 0;
}


