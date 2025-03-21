#include "list.h"
#include "stdlib/stdlib.h"
#include "stdlib/stdio.h"
#include "stdlib/string.h"
#include "utils/utils.h"
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
ListNode* ListRemove(ListNode* first,ListNode* n){
	if(n==first){
		return first->next;
	}
	ListNode* cur=first;
	while(cur->next!=0){
		if(cur->next==n){
			cur->next=cur->next->next;
			//FREE CUR.next
			n->next=0;
		}
	}
	return first;
}
