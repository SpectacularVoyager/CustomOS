#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8
#define COM5 0x5F8
#define COM6 0x4F8
#define COM7 0x5E8
#define COM8 0x4E8 


int init(unsigned int port);

char SerialRead(unsigned int port);

void SerialWrite(unsigned int port,char a);

void _putSerial(char a);
