#pragma once
#include "syscall.h"
#include "utils/utils.h"

// long readv(int fd, const struct iovec *iov, int iovcnt){
//
// }
//
// long preadv(int fd, const struct iovec *iov, int iovcnt,int offset){
//
// }
// long pwritev(int fd, const struct iovec *iov, int iovcnt,int offset){
//
// }
//
// long preadv2(int fd, const struct iovec *iov, int iovcnt,int offset, int flags){
//
// }
// long pwritev2(int fd, const struct iovec *iov, int iovcnt,int offset, int flags){
//
// }
long writev(int fd, struct iovec *iov, int iovcnt){
	int sum=0;
	FORI(iovcnt){
		sum+=write(fd,iov[i].iov_base,iov[i].iov_len);
	}
	return sum;
}
