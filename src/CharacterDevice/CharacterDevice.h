#pragma once

#define CHARACTER_DEVICE_MAX_LEN 81920

typedef struct {
	unsigned char buffer[CHARACTER_DEVICE_MAX_LEN];
	int left;
	int right;
} CharacterDevice;
void CharacterDeviceWrite(CharacterDevice*,char* str,int len);
int CharacterDeviceRead(CharacterDevice* device,char* src,int len);

CharacterDevice* STDIO();
