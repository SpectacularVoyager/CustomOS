#pragma once

#include "CharacterDevice.h"
#include "stdlib/string.h"
#include "utils/utils.h"

CharacterDevice STD_IN={.left=0,.right=0,.buffer={0}};

CharacterDevice* STDIO(){
	return &STD_IN;
}

void CharacterDeviceWrite(CharacterDevice* device,char* str,int len){
	int start=device->right;
	int end=start+len;
	start=MIN(start, CHARACTER_DEVICE_MAX_LEN);
	end=MIN(end, CHARACTER_DEVICE_MAX_LEN);
	device->right=end;
	memcpy(device->buffer+start,str,end-start);
	//printf("%d %d\n",device->left,device->right);
}
int CharacterDeviceRead(CharacterDevice* device,char* target,int len){

	//kprintf("%d %d\n",device->left,device->right);
	int start=device->left;
	int end=device->right;
	start=MIN(start, CHARACTER_DEVICE_MAX_LEN);
	end=MIN(end, CHARACTER_DEVICE_MAX_LEN);
	if(end==start)return 0;
	memcpy(target,device->buffer+start,end-start);
	device->left=end;
	return end-start;
}
