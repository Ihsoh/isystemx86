#include <ilib/ilib.h>
#include <jsonlib/jsonlib.h>

void die(const char * msg)
{
	printf("test-jsonlib ERROR: %s\n", msg);
	exit(-1);
}

const char * text0 = "	\
{	\
	a:\"test0\",	\
	b:null,	\
	c:true, 	\
	d:false,	\
	e:123,	\
	f:123.123,	\
	g:{	\
		a:\"test0-g-a\",	\
		b:null,	\
		c:true, 	\
		d:false,	\
		e:123,	\
		f:123.123,	\
		h:[[1], {a:2}, [3], {b:4}]	\
	},	\
	h:[\"123\", null, true, false, 123, 123.123, {a:[], b:{}, c:[], d:{}}]	\
}";

int main(int argc, char * argv[])
{
	JSONLEnvironment env;
	env.jsonl_malloc = malloc;
	env.jsonl_calloc = calloc;
	env.jsonl_free = free;
	if(!jsonl_init(&env))
		die("Cannot initialize jsonlib!");
	JSONLRawPtr text0_jsonobj = jsonl_parse_json(text0);
	if(text0_jsonobj == NULL)
		die("Cannot parse text0!");
	JSONLRawPtr text0_a = NULL;
	if(!JSONL_OBJECT_GET(text0_jsonobj, "a", &text0_a))
		die("Bad test0 json object!");
	char buffer[1024];
	if(	!jsonl_string_value(text0_a, buffer, sizeof(buffer))
		|| strcmp(buffer, "test0") != 0)
		die("Bad test0 json object!");
	JSONLRawPtr text0_g = NULL;
	if(!JSONL_OBJECT_GET(text0_jsonobj, "g", &text0_g))
		die("Bad test0 json object!");
	JSONLRawPtr text0_g_a = NULL;
	if(!JSONL_OBJECT_GET(text0_g, "a", &text0_g_a))
	if(	!jsonl_string_value(text0_g_a, buffer, sizeof(buffer))
		|| strcmp(buffer, "test0-g-a") != 0)
		die("Bad test0 json object!");
	if(!jsonl_free_json(text0_jsonobj))
		die("Cannot free text0 json object!");
	printf("test-jsonlib: OK!\n");
	return 0;
}