#include <ilib.h>

int main(int argc, char * argv[])
{
	printd(stod("1996.48"));
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
