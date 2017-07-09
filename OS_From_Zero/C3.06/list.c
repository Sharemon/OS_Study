#include "tLib.h"

#define firstNode headNode.postNode
#define lastNode  headNode.preNode


void NodeInit(node_t * node)
{
	node->preNode = node;
	node->postNode = node;
}

void ListInit(list_t * list)
{
	list->firstNode = &(list->headNode);
	list->lastNode = &(list->headNode);
	
	list->nodeCount = 0;
}

uint32_t ListCount(list_t * list)
{
	return list->nodeCount;
}

node_t * ListFirst(list_t * list)
{
	if (list->nodeCount != 0)
		return list->firstNode;
	else
		return  (node_t *)0;
}

node_t * ListLast(list_t * list)
{
	if (list->nodeCount != 0)
		return list->lastNode;
	else
		return  (node_t *)0;
}

node_t * ListPre(list_t * list, node_t * node)
{
	if (node->preNode == node)
	{
		return (node_t *)0;
	}
	else
	{
		return node->preNode;
	}
}

node_t * ListPost(list_t * list, node_t * node)
{
	if (node->postNode == node)
	{
		return (node_t *)0;
	}
	else
	{
		return node->postNode;
	}
}

void ListRemoveAll(list_t *list)
{
	uint32_t i;
	node_t * nextNode;
	node_t * curNode;
	
	nextNode = list->firstNode;
	for (i=0; i<list->nodeCount; i++)
	{
		curNode = nextNode;
		curNode ->preNode = curNode;	
		curNode->postNode = curNode;
		
		nextNode = nextNode->postNode;
	}
	
	list->firstNode = &(list->headNode);
	list->lastNode = &(list->headNode);
	list->nodeCount = 0;
}

void ListAddFirst(list_t * list, node_t * node)
{
	// there is some difference with tutorials
	// the head node is always the 0th node of the list.
	node->preNode = &(list->headNode);
	node->postNode = list->firstNode;
	
	list->firstNode->preNode = node;
	list->firstNode = node;
	
	list->nodeCount ++;
}

void ListAddLast(list_t * list, node_t * node)
{
	node->postNode = &(list->headNode);
	node->preNode = list->lastNode;
	
	list->lastNode->postNode = node;
	list->lastNode = node;
	
	list->nodeCount ++;
}


node_t * ListRemoveFirst(list_t * list)
{
	if (list->nodeCount == 0)
	{
		return (node_t *)0;
	}
	else
	{
		node_t * node = list->firstNode;
		
		list->firstNode = list->firstNode->postNode;
		list->firstNode->preNode = &(list->headNode);
		list->nodeCount--;
		
		return node;
	}
}

void ListInsertAfter(list_t * list, node_t * nodeAfter, node_t * nodeToInsert)
{
	nodeToInsert->preNode = nodeAfter;
	nodeToInsert->postNode = nodeAfter->postNode;
	
	nodeAfter->postNode = nodeToInsert;
	nodeToInsert->postNode->preNode = nodeToInsert;
	
	list->nodeCount ++;
}

void ListRemove (list_t * list, node_t * node)
{
	node->preNode->postNode = node->postNode;
	node->postNode->preNode = node->preNode;
	
	list->nodeCount --;
}

