#include "tLib.h"

#define firstNode headNode.postNode
#define lastNode  tailNode.postNode


void NodeInit(node_t * node)
{
	node->postNode = node;
}

void ListInit(list_t * list)
{
	list->firstNode = &(list->headNode);
	list->lastNode = &(list->tailNode);
	
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
		curNode->postNode = curNode;
		
		nextNode = nextNode->postNode;
	}
	
	ListInit(list);
}

void ListAddFirst(list_t * list, node_t * node)
{
	node->postNode = list->firstNode;
	
	list->firstNode = node;
	
	// If node is the only node in list, also point lastNode to this new node
	if (list->nodeCount == 0)
	{
		list->lastNode = node;
	}
	
	list->nodeCount ++;
}

void ListAddLast(list_t * list, node_t * node)
{
	
	list->lastNode->postNode = node;
	
	list->lastNode = node;
	
	node->postNode = &(list->headNode);
	
	// If node is the only node in list, also point firstNode to this new node
	if (list->nodeCount == 0)
	{
		list->firstNode = node;
	}
	
	list->nodeCount ++;
}


node_t * ListRemoveFirst(list_t * list)
{
	if (list->nodeCount == 0)
	{
		return (node_t *)0;
	}
	else if (list->nodeCount == 1)
	{
		node_t * node = list->firstNode;
		
		ListInit(list);
		
		return node;
	}
	else
	{
		node_t * node = list->firstNode;
		
		list->firstNode = list->firstNode->postNode;
		
		list->nodeCount--;
		
		return node;
	}
}

void ListInsertAfter(list_t * list, node_t * nodeAfter, node_t * nodeToInsert)
{
	nodeToInsert->postNode = nodeAfter->postNode;
	nodeAfter->postNode = nodeToInsert;
	
	list->nodeCount ++;
}


void ListRemove (list_t * list, node_t * node)
{
	node_t * tempNode;
	
	for(tempNode=list->firstNode; tempNode->postNode!=node;tempNode=tempNode->postNode)
	{
		;
	}
	
	tempNode->postNode = node->postNode;
	
	list->nodeCount --;
}

