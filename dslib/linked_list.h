#ifndef	_LINKED_LIST_H_
#define	_LINKED_LIST_H_
	
#include "types.h"
#include "value.h"

typedef struct _DSLLinkedListNode
{
	DSLValue						value;
	struct _DSLLinkedListNode *		prev;
	struct _DSLLinkedListNode *		next;
} DSLLinkedListNode, * DSLLinkedListNodePtr;

typedef struct _DSLLinkedList
{
	DSLLinkedListNode *	head;
	DSLLinkedListNode *	foot;
	int32				count;
} DSLLinkedList, * DSLLinkedListPtr;

#define	EXTERN_DSL_LNKLST_NEW_XXX_NODE(xxx)	\
	extern	\
	DSLLinkedListNode *	\
	dsl_lnklst_new_##xxx##_node(IN xxx value);

EXTERN_DSL_LNKLST_NEW_XXX_NODE(int8)
EXTERN_DSL_LNKLST_NEW_XXX_NODE(int16)
EXTERN_DSL_LNKLST_NEW_XXX_NODE(int32)
EXTERN_DSL_LNKLST_NEW_XXX_NODE(int64)
EXTERN_DSL_LNKLST_NEW_XXX_NODE(uint8)
EXTERN_DSL_LNKLST_NEW_XXX_NODE(uint16)
EXTERN_DSL_LNKLST_NEW_XXX_NODE(uint32)
EXTERN_DSL_LNKLST_NEW_XXX_NODE(uint64)
EXTERN_DSL_LNKLST_NEW_XXX_NODE(float)
EXTERN_DSL_LNKLST_NEW_XXX_NODE(double)
EXTERN_DSL_LNKLST_NEW_XXX_NODE(object)
EXTERN_DSL_LNKLST_NEW_XXX_NODE(bool)


extern
DSLLinkedList *
dsl_lnklst_new(void);

extern
BOOL
dsl_lnklst_free(DSLLinkedListPtr list);

extern
BOOL
dsl_lnklst_add_node(IN OUT DSLLinkedList * list, 
					IN OUT DSLLinkedListNode * node);

extern
BOOL
dsl_lnklst_remove_node(	IN OUT DSLLinkedList * list, 
						IN OUT DSLLinkedListNode * node);

extern
BOOL
dsl_lnklst_delete_node(	IN OUT DSLLinkedList * list,
						IN OUT DSLLinkedListNode * node);

extern
BOOL
dsl_lnklst_delete_all_node(IN OUT DSLLinkedList * list);

extern
BOOL
dsl_lnklst_delete_object_node(	IN OUT DSLLinkedList * list,
								IN OUT DSLLinkedListNode * node);

extern
BOOL
dsl_lnklst_delete_all_object_node(IN OUT DSLLinkedList * list);

extern
BOOL
dsl_lnklst_clear(OUT DSLLinkedList * list);

extern
int32
dsl_lnklst_find(IN DSLLinkedList * list,
				IN DSLLinkedListNode * node);

extern
DSLLinkedListNode *
dsl_lnklst_get(	IN DSLLinkedList * list,
				IN int32 index);

extern
int32
dsl_lnklst_count(IN DSLLinkedList * list);

#endif
