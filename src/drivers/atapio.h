//#define ATAPIO_DATA_PORT		0x1f0
//#define ATAPIO_ERROR_PORT		0x1f1
//#define ATAPIO_SECTOR_COUNT		0x1f2
//#define ATAPIO_SECTOR_NUMBER	0x1f3
//#define ATAPIO_CYLINDER_LOW		0x1f4
//#define ATAPIO_CYLINDER_HIGH	0x1f5
//#define ATAPIO_DRIVE_HEAD		0x1f6
//#define ATAPIO_STATUS_REG		0x1f7
//#define ATAPIO_COMMAND_REG		0x1f7
//
#define ATAPIO_STATUS_BUSY		0x80

void ATAPIO_Identify();

void ATAPIO_AttachIRQHandler();

