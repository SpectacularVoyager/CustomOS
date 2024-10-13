//#define VECTOR(x) Vector_##x
//
//#define VECTOR_DEF(x) typedef struct {\
//	x* ptr;\
//	int size;\
//}VECTOR(x);\
//x VECTOR_##x##_GET(VECTOR(x)* v,int n){\
//	return v->ptr[n];\
//}
typedef struct {
	void* data;
	unsigned int stride;
	unsigned int n;
	unsigned int cap;
} Vector;

void* VECTOR_GET(Vector* v,unsigned int n);

void VECTOR_RESIZE(Vector* v,unsigned int n);

void VECTOR_INSERT(Vector* v,int n,void* val);

void VECTOR_APPEND(Vector* v,void* val);
