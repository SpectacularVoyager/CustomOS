#define BETWEEN(x,a,b) (x>=a && x<=b)

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define ABS(a)	(((a)>0)?(a):-(a))

#define LOGVAL(a) printf(TRACE "%16s\t0x%X\n",#a,a);
