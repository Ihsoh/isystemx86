#include <ilib.h>

int main(int argc, char * argv[])
{
	*((uint *)0x9fffffff) = 0;
	asm volatile ("int $0xa1;");
	printf("%s\n", "Here is Test2");
	return 199648;

	printf("Test2 - This is client!\n");
	MQueueMessage message;
	message.param0.int32_value = 100;
	message.param1.int32_value = 200;
	push_message_c("Test1_MQueue", &message);
	while(!pop_message_c("Test1_MQueue", &message))
		asm volatile ("pause");
	printf("Get result from server, the result is %d\n", message.param0.int32_value);
	return 0;
}
