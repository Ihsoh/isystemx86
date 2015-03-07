#include "ilib.h"

void * operator new(uint size)
{
	return malloc(size);
}

void operator delete(void * ptr)
{
	free(ptr);
}

class Test
{
public:
	Test()
	{
		printf("%s\n", "Test Constructor");
	}

	void foo()
	{
		printf("%s\n", "Hello, World!");
	}
};

int main(int argc, char * argv[])
{
	Test * obj = new Test;
	obj->foo();
	delete obj;
	return 0;
}
