/*
void * __alloca(void)
{
	unsigned int size = 0;
	asm volatile(
		"movl	%%eax, %0"
		:"=m"(size));
	return malloc(size);
}
*/
