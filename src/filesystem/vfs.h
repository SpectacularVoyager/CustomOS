#include "drivers/ext2/ext2.h"

#include "usertask/Task.h"
typedef struct VFILE_t{

} VFILE;

typedef struct {
	int type;
	union file{
		EXT2_INODE ext2;
		VFILE virt;
	};
} FILE;

typedef struct {
	char* fullpath;
	FILE file;
} MOUNT;
