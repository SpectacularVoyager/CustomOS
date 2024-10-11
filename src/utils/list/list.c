#include "list.h"
#include "stdlib/stdlib.h"
#include "stdlib/stdio.h"
#include "stdlib/string.h"

unsigned int ListLength(ListNode* node){
	int n=0;
	while(node!=0){
		n++;
		node=node->next;
	}
	return n;
}
ListNode* last(ListNode* n){
	if(n==0)return 0;
	while(n->next!=0){
		n=n->next;
	}
	return n;
}
ListNode* ListAdd(ListNode* first,void* val){
	ListNode* n=malloc(sizeof(ListNode));
	n->next=0;
	n->val=val;
	if(first==0){
		return n;
	}
	last(first)->next=n;
	return first;
}
