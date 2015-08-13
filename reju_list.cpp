#include"reju_list.h"
#include<malloc.h>
#include<stdlib.h>

/*分配值为i的节点，并返回节点地址*/
Reju_List_PNode Reju_List_MakeNode(Reju_List_Item i)
{
	Reju_List_PNode p = NULL;
	p = (Reju_List_PNode)malloc(sizeof(Reju_List_Node));
	if(p!=NULL)
	{
		p->data = i;
		p->previous = NULL;
		p->next = NULL;
	}
	return p;
}
/*释放p所指的节点*/
void Reju_List_FreeNode(Reju_List_PNode p)
{
	 free(p);
}
/*构造一个空的双向链表*/
Reju_List_DList * Reju_List_InitList()
{
	Reju_List_DList *plist = (Reju_List_DList *)malloc(sizeof(Reju_List_DList));
	Reju_List_PNode head = Reju_List_MakeNode(0);
	if(plist!=NULL)
	{
		if(head!=NULL)
		{
			plist->head = head;
			plist->tail = head;
			plist->size = 0;
		}
		else
			return NULL;
	}
	return plist;
}

/*摧毁一个双向链表*/
void Reju_List_DestroyList(Reju_List_DList *plist)
{
	Reju_List_ClearList(plist);
	free(Reju_List_GetHead(plist));
	free(plist);
}

/*判断链表是否为空表*/
int Reju_List_IsEmpty(Reju_List_DList *plist)
{
	if(Reju_List_GetSize(plist)==0&&Reju_List_GetTail(plist)==Reju_List_GetHead(plist))
		return 1;
	else
		return 0;
}
/*将一个链表置为空表，释放原链表节点空间*/
void Reju_List_ClearList(Reju_List_DList *plist)
{
	Reju_List_PNode temp,p;
	p = Reju_List_GetTail(plist);
	while(!Reju_List_IsEmpty(plist))
	{
		temp = Reju_List_GetPrevious(p);
		Reju_List_FreeNode(p);
		p = temp;
		plist->tail = temp;
		plist->size--;
	}
}

/*返回头节点地址*/
Reju_List_PNode Reju_List_GetHead(Reju_List_DList *plist)
{
	return plist->head;
}

/*返回尾节点地址*/
Reju_List_PNode Reju_List_GetTail(Reju_List_DList *plist)
{
	return plist->tail;
}

/*返回链表大小*/
int Reju_List_GetSize(Reju_List_DList *plist)
{
	return plist->size;
}

/*返回p的直接后继位置*/
Reju_List_PNode Reju_List_GetNext(Reju_List_PNode p)
{
	return p->next;
}

/*返回p的直接前驱位置*/
Reju_List_PNode Reju_List_GetPrevious(Reju_List_PNode p)
{
	return p->previous;
}

/*将pnode所指节点插入第一个节点之前*/
Reju_List_PNode Reju_List_InsFirst(Reju_List_DList *plist,Reju_List_PNode pnode)
{
	Reju_List_PNode head = Reju_List_GetHead(plist);

	if(Reju_List_IsEmpty(plist))
		plist->tail = pnode;
	plist->size++;

	pnode->next = head->next;
	pnode->previous = head;

	if(head->next!=NULL)
		head->next->previous = pnode;
	head->next = pnode;

	return pnode;
}

/*将链表第一个节点删除,返回该节点的地址*/
Reju_List_PNode Reju_List_DelFirst(Reju_List_DList *plist)
{
	Reju_List_PNode head = Reju_List_GetHead(plist);
	Reju_List_PNode p=head->next;
	if(p!=NULL)
	{
		if(p==Reju_List_GetTail(plist))
			plist->tail = p->previous;
		head->next = p->next;
		head->next->previous = head;
		plist->size--;

	}
	return p;
}

/*获得节点的数据项*/
Reju_List_Item Reju_List_GetItem(Reju_List_PNode p)
{
	return p->data;
}

/*设置节点的数据项*/
void Reju_List_SetItem(Reju_List_PNode p,Reju_List_Item i)
{
	p->data = i;
}

/*删除链表中的尾节点并返回地址，改变链表的尾指针指向新的尾节点*/
Reju_List_PNode Reju_List_Remove(Reju_List_DList *plist)
{
	Reju_List_PNode p=NULL;
	if(Reju_List_IsEmpty(plist))
		return NULL;
	else
	{
		p = Reju_List_GetTail(plist);
		p->previous->next = p->next;
		plist->tail = p->previous;
		plist->size--;
		return p;
	}
}
/*在链表中p位置之前插入新节点s*/
Reju_List_PNode Reju_List_InsBefore(Reju_List_DList *plist,Reju_List_PNode p,Reju_List_PNode s)
{
	s->previous = p->previous;
	s->next = p;
	p->previous->next = s;
	p->previous = s;

	plist->size++;
	return s;
}
/*在链表中p位置之后插入新节点s*/
Reju_List_PNode Reju_List_InsAfter(Reju_List_DList *plist,Reju_List_PNode p,Reju_List_PNode s)
{
	s->next = p->next;
	s->previous = p;

	if(p->next != NULL)
		p->next->previous = s;
	p->next = s;

	if(p == Reju_List_GetTail(plist))
		plist->tail = s;

	plist->size++;
	return s;
}
