#include <ilib.h>

int main(int argc, char * argv[])
{
	run_in_bg();
	create_file("DA:/", "test2.txt");
	FILE * fptr = NULL;
	while((fptr = fopen("DA:/test2.txt", FILE_MODE_APPEND)) == NULL);
	/*if(fptr == NULL)
	{
		print_str("The applicaton failed to open DA:/test2.txt!\n");
		return -1;
	}*/
	uint idx;
	char buffer[KB(1)];
	for(idx = 0; idx < KB(1); idx++)
		buffer[idx] = 'B';
	for(idx = 0; idx < 1000; idx++)
	{
		while(!fappend(fptr, buffer, sizeof(buffer)));
		print_str("test2: ");
		printn(idx);
		print_str("\n");
	}
	fclose(fptr);
	return 0;

	//asm volatile ("movb $0, %ax; divb %ax");
	//while(1);
	//*((uint *)0x9fffffff) = 0;
	//asm volatile ("int $0xa1;");
	//asm volatile ("movw $0, %ax; divb %ax;");
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
