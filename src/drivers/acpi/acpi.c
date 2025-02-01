#include "acpi.h"
#include "../../stdlib/stdio.h"
#include "drivers/acpi/base.h"
#include "stdlib/string.h"
#include "utils/utils.h"

int ACPISDT_Checksum(ACPISDTHeader *tableHeader)
{
	LOGVALD(tableHeader);
    unsigned char sum = 0;

    for (unsigned int i = 0; i < tableHeader->Length; i++)
    {
        sum += ((char *) tableHeader)[i];
    }

    return sum == 0;
}

ACPIHeaders ACPI_INIT(RSDP_t* rsdp){
	kprintf(INFO "SIGNATURE\t%p\n",rsdp->Signature);
	kprintf(INFO "CHECKSUM\t%p\n",rsdp->Checksum);
	kprintf(INFO "OEMID\t%6s\n",rsdp->OEMID);
	kprintf(INFO "REVISION\t%p\n",rsdp->Revision);
	kprintf(INFO "ADDRESS \t%p\n",rsdp->RsdtAddress);
	
	RSDT* rsdt=NULL;
	if(rsdp->Revision==0){
		//printf("USING ACPI 1.0\n");
		rsdt=(void*)rsdp->RsdtAddress;
	}else{
		printf(ERROR "WE DO NOT SUPPORT ACPI 2.0\n");
	}
	//kprintf(INFO"SIGNATURE %4s\n",rsdt->h.Signature);
	//kprintf(INFO"LENGTH %x\n",rsdt->h.Length);
	//kprintf(INFO"OEMID %6s\n",rsdt->h.OEMID);
	//kprintf(INFO"OEM TABLE ID %8s\n",rsdt->h.OEMTableID);
	LOGVALD(rsdt);
	if(!ACPISDT_Checksum((ACPISDTHeader*)rsdt)){
		printf(ERROR "CHECKSUM DOES NOT MATCH\n");
	}
	ACPISDTHeader* facp=0;
	ACPISDTHeader* mcfg=0;
	ACPISDTHeader* apic=0;

    int entries = (rsdt->h.Length - sizeof(rsdt->h)) / 4;
	uint32_t* ptr= (uint32_t *) rsdt->PointerToOtherSDT;
    for (int i = 0; i < entries; i++)
    {
        ACPISDTHeader *h = (ACPISDTHeader*)ptr[i];
        if (strncmp(h->Signature, "FACP", 4)==0)
            facp= h;
        if (strncmp(h->Signature, "MCFG", 4)==0)
            mcfg= h;
        if (strncmp(h->Signature, "APIC", 4)==0)
            apic= h;
    }
	return (ACPIHeaders){.mcfg=(MCFGHeader*)mcfg,.fadt=(FADT*)facp,.apic=(MADT*)apic,.rsdt=rsdt};

}
