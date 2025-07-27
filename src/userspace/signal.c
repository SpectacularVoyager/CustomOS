#include"signal.h"
#include "stdlib/stdio.h"
int sigprocmask(int how, const sigset_t * set,sigset_t *oldset){
	switch(how){
		case SIG_BLOCK:
			// printf("BLOCK\n");
			break;
		case SIG_UNBLOCK:
			// printf("UNBLOCK\n");
			break;
		case SIG_SETMASK:
			// printf("SET MASK\n");
			break;
	}
	// printf("\t%p\t%p\n",set,oldset);
	return -1;
}
