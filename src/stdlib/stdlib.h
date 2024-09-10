void AssignMallocMemoryMap(struct multiboot_tag *tag);

void* malloc(unsigned long size);

void* mallocA(unsigned long size,unsigned long align);

void MallocSetStart(unsigned long ptr);
