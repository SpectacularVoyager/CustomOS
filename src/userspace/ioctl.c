#include <stdlib/stdio.h>
#include <sys/ioctl.h>
#include <stdarg.h>

int ioctl(int fd, int op,...){
	switch(op){
		case TIOCGWINSZ:
			va_list args;
			struct winsize* w; 
			va_start(args, 1);  
			w=va_arg(args, struct winsize*);
			w->ws_xpixel=1024;
			w->ws_ypixel=768;
			w->ws_row=25;
			w->ws_col=80;
			va_end(args);
			printf("IOCTL:FD[%x] -> TIOCGWINSZ,%x\n",fd,w);	
			return 0;
			break;
		default:
			printf("IOCTL:FD[%x] -> %x,%x\n",fd,op);	

	}
	return 0;
}

