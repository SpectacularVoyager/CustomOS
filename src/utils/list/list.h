#pragma once

typedef struct ListNode_t{
	void* val;
	struct ListNode_t* next;
} ListNode;

#define FOR_NODE(x) for(int i=0,Node* node=x;n!=0;n=n->next,i++)

ListNode* ListAdd(ListNode* first,void* val);

unsigned int ListLength(ListNode* node);

//#define LIST_NODE_DEF(x) typedef struct ListNode##x##_t{\
//	x* val;\
//	struct ListNode##x* next;\
//} ListNode##x;
//
//LIST_NODE_DEF(int);
//#define LIST_NODE(x) ListNode##x
