#define BETWEEN(x,a,b) (x>=a && x<=b)

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define ABS(a)	(((a)>0)?(a):-(a))

#define LOGVAL(a) printf("%16s\t0x%X\n",#a,a);
#define LOGVALD(a) printf("%16s\t%p\n",#a,a);

#define FORI(x) for(unsigned int i=0;i<(x);i++)
#define FORJ(x) for(unsigned int j=0;j<(x);j++)

#define CEILDIV(a,b) ((a)+(b)-1)/(b)

#define GETOR(a,b)	((a)!=0)?(a):(b)

