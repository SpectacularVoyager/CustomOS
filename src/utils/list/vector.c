#include "vector.h"
#include "stdlib/stdlib.h"
#include "stdlib/string.h"
void* VECTOR_GET(Vector* v,unsigned int n){
	if(n>=v->cap)return 0;
	return v->data+n*v->stride;
}
void VECTOR_RESIZE(Vector* v,unsigned int n){
	v->cap=n;
	//FREE(v->data)
	v->data=malloc(v->stride*n);
}
inline void VECTOR_CHECK_AND_RESIZE(Vector* v,unsigned int n){
	if(v->cap<=n){
		if(2*v->cap>n){
			VECTOR_RESIZE(v,2*v->cap);
		}
		VECTOR_RESIZE(v,n);
	}
}
inline void* VECTOR_OFF(Vector* v,int n){
	return v->data+n*v->stride;
}
//void VECTOR_INSERT(Vector* v,int n,void* val){
//	VECTOR_RESIZE(v,n);
//	memcpy(VECTOR_OFF(v,n),val,v->stride);
//}
void VECTOR_APPEND(Vector* v,void* val){
	unsigned int n=v->n;
	VECTOR_RESIZE(v,n);
	memcpy(VECTOR_OFF(v,n),val,v->stride);
	v->n++;
}
int VECTOR_PUT(Vector* v,unsigned int n,void* val){
	if(n>=v->cap)return 0;
	memcpy(VECTOR_OFF(v,n),val,v->stride);
	return 1;
}
Vector VECTOR_INIT(int n,int stride){
	Vector v;
	v.stride=stride;
	v.cap=n;
	v.n=0;
	v.data=malloc(v.stride*n);
	return v;
}
