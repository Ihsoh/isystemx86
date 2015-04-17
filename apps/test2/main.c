#include <ilib.h>

int main(int argc, char * argv[])
{
	uint count = df_count("DA:/isystem/");
	struct RawBlock * subs = (struct RawBlock *)malloc(count * sizeof(struct RawBlock));
	df("DA:/isystem/", subs, count);
	uint index;
	for(index = 0; index < count; index++)
		if(subs[index].type == BLOCK_TYPE_FILE)
		{
			struct FileBlock * fblk = (struct FileBlock *)(subs + index);
			printf("<FILE> %s\n", fblk->filename);
		}
		else if(subs[index].type == BLOCK_TYPE_DIR)
		{
			struct DirBlock * dblk = (struct DirBlock *)(subs + index);
			printf("<DIR > %s\n", dblk->dirname);
		}
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
