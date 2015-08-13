#ifndef _REJU_LIST_H_
#define _REJU_LIST_H_

typedef  void*  Reju_List_Item;
typedef struct Reju_List_Node* Reju_List_PNode;

/*定义节点类型*/
typedef struct Reju_List_Node
{
	Reju_List_Item data;		/*数据域*/
	Reju_List_PNode previous; /*指向前驱*/
	Reju_List_PNode next;		/*指向后继*/
}Reju_List_Node;


/*定义链表类型*/
typedef struct
{
	Reju_List_PNode head;		/*指向头节点*/
	Reju_List_PNode tail;		/*指向尾节点*/
	int size;
}Reju_List_DList;

/*分配值为i的节点，并返回节点地址*/
Reju_List_PNode Reju_List_MakeNode(Reju_List_Item i);

/*释放p所指的节点*/
void Reju_List_FreeNode(Reju_List_PNode p);

/*构造一个空的双向链表*/
Reju_List_DList* Reju_List_InitList();

/*摧毁一个双向链表*/
void Reju_List_DestroyList(Reju_List_DList *plist);

/*将一个链表置为空表，释放原链表节点空间*/
void Reju_List_ClearList(Reju_List_DList *plist);

/*返回头节点地址*/
Reju_List_PNode Reju_List_GetHead(Reju_List_DList *plist);

/*返回尾节点地址*/
Reju_List_PNode Reju_List_GetTail(Reju_List_DList *plist);

/*返回链表大小*/
int Reju_List_GetSize(Reju_List_DList *plist);

/*返回p的直接后继位置*/
Reju_List_PNode Reju_List_GetNext(Reju_List_PNode p);

/*返回p的直接前驱位置*/
Reju_List_PNode Reju_List_GetPrevious(Reju_List_PNode p);

/*将pnode所指节点插入第一个节点之前*/
Reju_List_PNode Reju_List_InsFirst(Reju_List_DList *plist,Reju_List_PNode pnode);

/*将链表第一个节点删除并返回其地址*/
Reju_List_PNode Reju_List_DelFirst(Reju_List_DList *plist);

/*获得节点的数据项*/
Reju_List_Item Reju_List_GetItem(Reju_List_PNode p);

/*设置节点的数据项*/
void Reju_List_SetItem(Reju_List_PNode p,Reju_List_Item i);

/*删除链表中的尾节点并返回其地址，改变链表的尾指针指向新的尾节点*/
Reju_List_PNode Reju_List_Remove(Reju_List_DList *plist);

/*在链表中p位置之前插入新节点S*/
Reju_List_PNode Reju_List_InsBefore(Reju_List_DList *plist,Reju_List_PNode p,Reju_List_PNode s);

/*在链表中p位置之后插入新节点s*/
Reju_List_PNode Reju_List_InsAfter(Reju_List_DList *plist,Reju_List_PNode p,Reju_List_PNode s);
#endif
