#include "network.h"
#include "drivers/networking/rtl8139/rtl8139.h"
#include "specifications/net.h"
#include "stdlib/stdio.h"
#include "stdlib/string.h"
#include "stdlib/stdlib.h"
#include "utils/utils.h"
#include <stdint.h>
void __kprintMac(unsigned char* data,unsigned int len){
	FORI(len-1){
		kprintf("%d::",data[i]);
	}kprintf("%d\n",data[len-1]);
}
int compareAddr(unsigned char* v1,unsigned char* v2,unsigned int len){
	FORI(len){
		int val=v1[i]>v2[i];
		if(val!=0)return val;
	}
	return 0;
}

void NetworkOnARP(PACKET_ETHERNET2_BEGIN* eth){
	RTL8139* NIC=nic();
	PACKET_ARP_UNIX* arp=(PACKET_ARP_UNIX*)&eth->data;
	int op = htons(arp->operation);
	if(op==ARP_OP_REQ){

		unsigned char myaddr[4]={192,168,0,100};
		if(compareAddr(arp->targetProtocol,myaddr,4)==0){
			kprintf("ASKING FOR MY ADDRESS\n");

			PACKET_ARP_UNIX reply;
			reply.protocol_len=arp->protocol_len;
			reply.hardware_len=arp->hardware_len;
			reply.operation=htons(ARP_OP_REPLY);
			reply.hardware_type=arp->hardware_type;
			reply.protocol_type=arp->protocol_type;

			memcpy(reply.targetHardware,arp->senderHardware,6);
			memcpy(reply.targetProtocol,arp->senderProtocol,4);


			memcpy(reply.senderHardware,NIC->header->MAC,6);
			memcpy(reply.senderProtocol,myaddr,4);
			
			khexdump(&reply,sizeof(PACKET_ARP_UNIX),32);
			PACKET_ETHERNET2_BEGIN* ret=malloc(1584);
			ETHERNET_default(ret,arp->senderHardware,NIC->header->MAC,ETH_TYPE_ARP);
			addARPUnix(ret,&reply);
			RTL8139_SEND(ret,14+sizeof(PACKET_ARP_UNIX));
		}else{
			kprintf("ASKING FOR:");
			__kprintMac(arp->targetProtocol,4);
		}
	}else{

	}
}
void NetworkOnICMP(PACKET_ETHERNET2_BEGIN* pack){
	RTL8139* NIC=nic();
	PACKET_IP* ip=(PACKET_IP*)&pack->data;
	PACKET_ICMP* icmp=(PACKET_ICMP*)&ip->data;

	if(icmp->type==ICMP_TYPE_ECHO){
		int seq=htons(icmp->data.echo.seq);
		int ident=htons(icmp->data.echo.ident);
		kprintf("RECV ECHO [%x] (%d)\n",ident,seq);
		PACKET_ETHERNET2_BEGIN* ret=malloc(1584);
		PACKET_IP* newIP=(PACKET_IP*)ret->data;
		ETHERNET_default(ret,pack->mac_dest,NIC->header->MAC,ETH_TYPE_IP);
		IP_default(newIP,ip->dest_ip,ip->source_ip,0x100,16);
		newIP->protocol=IP_PROTO_ICMP;

		PACKET_ICMP* newICMP=(PACKET_ICMP*)newIP->data;
		newICMP->type=htons(ICMP_TYPE_ECHO);
		newICMP->data.echo.ident=icmp->data.echo.ident;
		newICMP->data.echo.seq=icmp->data.echo.seq;
		newICMP->code=0;
		newICMP->checksum=htons(checksum(newICMP,8));
		newIP->ident=ip->ident;
		
		RTL8139_SEND(ret,14+htons(newIP->len));
		
	}else{
		kprintf("RECV ICMP[%d]\n",icmp->type);
	}
}

void NetworkOnIPv4(PACKET_ETHERNET2_BEGIN* pack){
	PACKET_IP* ip=(PACKET_IP*)&pack->data;
	switch(ip->protocol){
		case IP_PROTO_ICMP:
			NetworkOnICMP(pack);
			break;
		default:
			kprintf("UNKNOWN IP PROTOCOL %d\n",ip->protocol);
	}
}
