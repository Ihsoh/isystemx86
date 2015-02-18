#include "linked_list.h"
#include "types.h"
#include "dslib.h"

#define	NODEPTR_VALUE(nodeptr) ((nodeptr)->value.value)
#define	NODEPTR_TYPE(nodeptr) ((nodeptr)->value.type)

#define	DSL_LNKLST_NEW_XXX_NODE(xxx, XXX)	\
	DSLLinkedListNode *	\
	dsl_lnklst_new_##xxx##_node(IN xxx value)	\
	{	\
		DSLLinkedListNode * node = dsl_malloc(sizeof(DSLLinkedListNode));	\
		if(node == NULL)	\
			return NULL;	\
		NODEPTR_VALUE(node).xxx##_value = value;	\
		NODEPTR_TYPE(node) = DSLVALUE_##XXX;	\
		return node;	\
	}

DSL_LNKLST_NEW_XXX_NODE(int8, INT8)
DSL_LNKLST_NEW_XXX_NODE(int16, INT16)
DSL_LNKLST_NEW_XXX_NODE(int32, INT32)
DSL_LNKLST_NEW_XXX_NODE(int64, INT64)
DSL_LNKLST_NEW_XXX_NODE(uint8, UINT8)
DSL_LNKLST_NEW_XXX_NODE(uint16, UINT16)
DSL_LNKLST_NEW_XXX_NODE(uint32, UINT32)
DSL_LNKLST_NEW_XXX_NODE(uint64, UINT64)
DSL_LNKLST_NEW_XXX_NODE(float, FLOAT)
DSL_LNKLST_NEW_XXX_NODE(double, DOUBLE)
DSL_LNKLST_NEW_XXX_NODE(object, OBJECT)
DSL_LNKLST_NEW_XXX_NODE(bool, BOOL)

DSLLinkedList *
dsl_lnklst_new(void)
{
	DSLLinkedList * list = dsl_malloc(sizeof(DSLLinkedList));
	if(list == NULL)
		return NULL;
	list->head = NULL;
	list->foot = NULL;
	list->count = 0;
	return list;
}

BOOL
dsl_lnklst_add_node(IN OUT DSLLinkedList * list, 
					IN OUT DSLLinkedListNode * node)
{
	if(list == NULL || node == NULL)
		return FALSE;
	if(list->head == NULL && list->foot == NULL)
	{
		list->head = node;
		list->foot = node;
		node->prev = NULL;
		node->next = NULL;
		list->count++;
	}
	else if(list->head != NULL && list->foot != NULL)
	{
		list->foot->next = node;
		node->prev = list->foot;
		node->next = NULL;
		list->foot = node;
		list->count++;
	}
	else
		return FALSE;
	return TRUE;
}

BOOL
dsl_lnklst_remove_node(	IN OUT DSLLinkedList * list, 
						IN OUT DSLLinkedListNode * node)
{
	if(list == NULL || node == NULL)
		return FALSE;
	if(list->head == node && list->foot == node)
	{
		list->head = NULL;
		list->foot = NULL;
	}
	else if(list->head == node && list->foot != node)
	{
		node->next->prev = NULL;
		list->head = node->next;
	}
	else if(list->head != node && list->foot == node)
	{
		node->prev->next = NULL;
		list->foot = node->prev;
	}
	else if(list->head != node && list->foot != node)
	{
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	else
		return FALSE;
	list->count--;
	return TRUE;
}

BOOL
dsl_lnklst_delete_node(	IN OUT DSLLinkedList * list,
						IN OUT DSLLinkedListNode * node)
{
	if(list == NULL || node == NULL)
		return FALSE;
	if(dsl_lnklst_remove_node(list, node))
	{
		dsl_free(node);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL
dsl_lnklst_delete_all_node(IN OUT DSLLinkedList * list)
{
	if(list == NULL)
		return FALSE;
	while(list->head != NULL)
		dsl_lnklst_delete_node(list, list->head);
	return TRUE;
}

BOOL
dsl_lnklst_delete_object_node(	IN OUT DSLLinkedList * list,
								IN OUT DSLLinkedListNode * node)
{
	if(list == NULL || node == NULL || node->value.type != DSLVALUE_OBJECT)
		return FALSE;
	if(dsl_lnklst_remove_node(list, node))
	{
		dsl_free(node->value.value.object_value);
		dsl_free(node);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL
dsl_lnklst_delete_all_object_node(IN OUT DSLLinkedList * list)
{
	if(list == NULL)
		return FALSE;
	while(list->head != NULL)
		dsl_lnklst_delete_object_node(list, list->head);
}

BOOL
dsl_lnklst_clear(OUT DSLLinkedList * list)
{
	if(list == NULL)
		return FALSE;
	list->head = NULL;
	list->foot = NULL;
	list->count = 0;
	return TRUE;
}

int32
dsl_lnklst_find(IN DSLLinkedList * list,
				IN DSLLinkedListNode * node)
{
	if(list == NULL)
		return -1;
	int32 index = 0;
	DSLLinkedListNode * next = list->head;
	while(next != NULL)
	{
		if(next == node)
			return index;
		next = next->next;
		index++;
	}
	return -1;
}

DSLLinkedListNode *
dsl_lnklst_get(	IN DSLLinkedList * list,
				IN int32 index)
{
	if(list == NULL || index < 0 || index >= list->count)
		return NULL;
	DSLLinkedListNode * next = list->head;
	while(index-- != 0)
		next = next->next;
	return next;
}

int32
dsl_lnklst_count(IN DSLLinkedList * list)
{
	if(list == NULL)
		return -1;
	return list->count;	
}
