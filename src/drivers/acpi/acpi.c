#include "acpi.h"
#include "../../stdlib/stdio.h"
#include "../../stdlib/string.h"
#include "../../utils/ports.h"

int ACPISDT_Checksum(ACPISDTHeader *tableHeader)
{
    unsigned char sum = 0;

    for (int i = 0; i < tableHeader->Length; i++)
    {
        sum += ((char *) tableHeader)[i];
    }

    return sum == 0;
}

ACPIHeaders ACPI_INIT(RSDP_t* rsdp){
	//kprintf(INFO "SIGNATURE\t%p\n",rsdp->Signature);
	//kprintf(INFO "CHECKSUM\t%p\n",rsdp->Checksum);
	//kprintf(INFO "OEMID\t%6s\n",rsdp->OEMID);
	//kprintf(INFO "REVISION\t%p\n",rsdp->Revision);
	//kprintf(INFO "ADDRESS \t%p\n",rsdp->RsdtAddress);
	
	RSDT* rsdt;
	if(rsdp->Revision==0){
		kprintf("USING ACPI 1.0\n");
		rsdt=(void*)rsdp->RsdtAddress;
	}else{
		kprintf(ERROR "WE DO NOT SUPPORT ACPI 2.0\n");
	}
	//kprintf(INFO"SIGNATURE %4s\n",rsdt->h.Signature);
	//kprintf(INFO"LENGTH %x\n",rsdt->h.Length);
	//kprintf(INFO"OEMID %6s\n",rsdt->h.OEMID);
	//kprintf(INFO"OEM TABLE ID %8s\n",rsdt->h.OEMTableID);
	if(!ACPISDT_Checksum(&rsdt->h)){
		kprintf(ERROR "CHECKSUM DOES NOT MATCH\n");
	}
	ACPISDTHeader* facp;
	ACPISDTHeader* mcfg;

    int entries = (rsdt->h.Length - sizeof(rsdt->h)) / 4;

	uint32_t* ptr= (uint32_t *) rsdt->PointerToOtherSDT;
    for (int i = 0; i < entries; i++)
    {
        ACPISDTHeader *h = (ACPISDTHeader*)ptr[i];
        if (strncmp(h->Signature, "FACP", 4))
            facp= h;
        if (strncmp(h->Signature, "MCFG", 4))
            mcfg= h;
    }
	return (ACPIHeaders){.mcfg=(MCFGHeader*)mcfg,.fadt=(FADT*)facp,.rsdt=rsdt};

}
