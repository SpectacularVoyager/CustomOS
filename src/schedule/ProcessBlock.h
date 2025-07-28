#pragma once

enum BLOCK_TYPE{
	block_none=0,
	block_fd=1
};


typedef struct{
	int type;
	union{
		struct PID{
			long pid;
		};
	}block;
} BLOCK;
