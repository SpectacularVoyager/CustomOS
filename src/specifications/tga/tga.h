#include "drivers/ext2/ext2.h"


typedef struct {
	unsigned char magic1;             // must be zero
	unsigned char colormap;           // must be zero
	unsigned char encoding;           // must be 2
	unsigned short cmaporig, cmaplen; // must be zero
	unsigned char cmapent;            // must be zero
	unsigned short x;                 // must be zero
	unsigned short y;                 // image's height
	unsigned short h;                 // image's height
	unsigned short w;                 // image's width
	unsigned char bpp;                // must be 32
	unsigned char pixeltype;          // must be 40
	unsigned char data[1];
} __attribute__((packed))TGA_FILE;

int TGA_DRAW(char* path);
