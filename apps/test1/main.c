#include <ilib.h>

int main(int argc, char * argv[])
{
	run_in_bg();
	print_str("test1 running!\n");
	*(uint *)(0x00020000) = 100;
	while(!create_file("DA:/", argv[1]));
	FILE * fptr = NULL;
	while((fptr = fopen(argv[2], FILE_MODE_APPEND)) == NULL);
	/*if(fptr == NULL)
	{
		print_str("The applicaton failed to open DA:/test1.txt!\n");
		return -1;
	}*/
	uint idx;
	char buffer[KB(1)];
	for(idx = 0; idx < KB(1); idx++)
		buffer[idx] = 'A';
	for(idx = 0; idx < 100; idx++)
	{
		while(!fappend(fptr, buffer, sizeof(buffer)));
		print_str(argv[3]);
		printn(idx);
		print_str("\n");
	}
	fclose(fptr);
	return 0;

	run_in_bg();
	printf("%s\n", "Test1 Start");
	printf("Test2 returned %d\n", il_exec_sync("DA:/isystem/bins/test2", ""));
	printf("%s\n", "Test1 End");
	return 0;

	uint ui;
	printf("Test1 - This is server!\n");
	uint32 mqueue_id = create_mqueue("Test1_MQueue");
	run_in_bg();
	MQueueMessage message;
	while(1)
	{
		while(!pop_message_s(mqueue_id, &message))
			asm volatile ("pause");
		int tid = message.tid;
		int result = message.param0.int32_value + message.param1.int32_value;
		message.param0.int32_value = result;
		push_message_s(mqueue_id, &message);
	}
	delete_mqueue(mqueue_id);
	return 0;
}
