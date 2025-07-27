#include <stdint.h>
#include "specifications/tga/tga.h"
void startWindows(uint32_t* buffer);

int drawTexture(char* texture,int x,int y,int w,int h);

static uint32_t* framebuffer;
