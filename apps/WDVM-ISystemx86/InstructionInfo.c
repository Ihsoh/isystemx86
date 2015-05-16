#include <ilib/ilib.h>
#include <dslib/linked_list.h>
#include "InstructionInfo.h"

InstructionInfo * InstructionInfo_New(void)
{
    InstructionInfo * ii = malloc(sizeof(InstructionInfo));
    if(ii == NULL)
        return NULL;
    ii->pi = dsl_lnklst_new();
    return ii;
}

void InstructionInfo_Free(InstructionInfo * ii)
{
    if(ii == NULL)
        return;
    dsl_lnklst_delete_all_object_node(ii->pi);
    free(ii);
}

InstructionInfo * InstructionInfo_Read(ILFILE * fd)
{
	InstructionInfo * ii = InstructionInfo_New();
    ii->ID = bh_readuint16(fd);
    ii->pcount = bh_readbyte(fd);
    int i;
    for(i = 0; i < ii->pcount; ++i)
    {
    	ParameterInfo pi = ParameterInfo_Read(fd);
    	ParameterInfo * new_pi = malloc(sizeof(ParameterInfo));
        memcpy(new_pi, &pi, sizeof(ParameterInfo));
        DSLLinkedListNode * node = dsl_lnklst_new_object_node(new_pi);
        dsl_lnklst_add_node(ii->pi, node);
    }
    return ii;
}