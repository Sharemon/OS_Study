#ifndef _TLIB_H
#define _TLIB_H

#include <stdint.h>

typedef struct
{
	uint32_t bitmap;
}bitmap_t;

void BitmapInit(bitmap_t * bitmap);
void BitmapSet(bitmap_t * bitmap, uint32_t pos);
void BitmapClear(bitmap_t * bitmap, uint32_t pos);

uint32_t BitmapGetFirstSet(bitmap_t * bitmap);
uint32_t BitmapPosCount(void);

typedef struct _node_t
{
	struct _node_t * postNode;
}node_t;

typedef struct _list_t
{
	node_t headNode;
	node_t tailNode;
	uint32_t nodeCount;
}list_t;

#define nodeParent(node, parent, name) \
						(parent *)((uint32_t)node - (uint32_t)&(((parent *)0)->name))


void NodeInit(node_t * node);
void ListInit(list_t * list);
uint32_t ListCount(list_t * list);
node_t * ListFirst(list_t * list);
node_t * ListLast(list_t * list);
node_t * ListPre(list_t * list, node_t * node);
node_t * ListPost(list_t * list, node_t * node);
void ListRemoveAll(list_t *list);
void ListAddFirst(list_t * list, node_t * node);
void ListAddLast(list_t * list, node_t * node);
node_t * ListRemoveFirst(list_t * list);
void ListInsertAfter(list_t * list, node_t * nodeAfter, node_t * nodeToInsert);
void ListRemove (list_t * list, node_t * node);

#endif
