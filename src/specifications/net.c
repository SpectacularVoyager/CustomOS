#include "net.h"
#include "stdlib/string.h"
#include "utils/utils.h"
#include "utils/bit.h"
unsigned int checksum(void* _data,int len){
	uint16_t* data=(uint16_t*)_data;
	int sum=0;
	FORI(len/2){
		sum+=htons(data[i]);
	}
	if(len%2==1){
		printf(__FILE__ ": ODD LEN CHECKSUM MAY NOT WORK\n");
		sum+=U8(_data+len-1);
	}
	sum=0xFFFF-((sum&0xFFFF)+(sum>>16));
	return sum;
}
void ETHERNET_default(PACKET_ETHERNET2_BEGIN* eth,uint8_t* src,uint8_t* dest,unsigned int type){
	memcpy(eth->mac_dest,dest,6);
	memcpy(eth->mac_src,src,6);
	eth->type=htons(type);
}
void ETH_ADDTRAILER(void* eth_end,uint32_t trailer){
	*(uint8_t*)(eth_end)=trailer;
}
void addARP(PACKET_ETHERNET2_BEGIN* eth,PACKET_ARP* arp){
	void* ethdata=&eth->data;
	memcpy(ethdata,arp,sizeof(PACKET_ARP));
	//SET TRAILER
	ETH_ADDTRAILER(ethdata+sizeof(PACKET_ARP),0);
}
void addIP(PACKET_ETHERNET2_BEGIN* eth,PACKET_IP* ip){
	void* ethdata=&eth->data;
	memcpy(ethdata,ip,ip->len);
	//SET TRAILER
	ETH_ADDTRAILER(ethdata+ip->len,0);
}
void IP_Recompute(PACKET_IP* ip,int datalen){

	ip->header_checksum=0;
	ip->len=htons((ip->header_len<<2)+datalen);
	ip->header_checksum=htons(checksum(ip,20));
}
void addUDP(PACKET_IP* ip,int src,int dest,void* data,int len){
	PACKET_UDP* udp=(PACKET_UDP*)&ip->data;
	udp->src_port=src;
	udp->dest_port=dest;
	udp->len=htons(8+len);
	//udp->checksum=checksum(data,len);
	memcpy(ip->data+8,data,len);
	IP_Recompute(ip, 8+len);
}
void IP_default(PACKET_IP* ip,unsigned long src,unsigned long dest,int ident,int len){
	ip->version=4;
	ip->header_len=5;
	ip->tos=0;
	ip->len=(ip->header_len<<2)+len;
	ip->ident=ident;
	ip->flags=0;
	ip->frag_off=0;
	ip->ttl=0x40;
	ip->protocol=0x11;
	ip->header_checksum=0;
	ip->source_ip=src;
	ip->dest_ip=dest;
	
	IP_Recompute(ip,0);

	//ip->header_checksum=checksum(ip,20);
}
void ARP_default(PACKET_ARP* arp,int op,uint8_t mac[6],unsigned int target){
	arp->hardware_type=1;
	arp->protocol_type=ETH_TYPE_ARP;
	arp->hardware_len=6;
	arp->protocol_len=4;
	arp->operation=op;

	memcpy(arp->data+0,mac,6);
	memcpy(arp->data+6,&target,4);
	memcpy(arp->data+10,&target,6);
	memcpy(arp->data+16,mac,4);
}
uint16_t htons(uint16_t nb) {
	return (nb>>8) | (nb<<8);
}

uint32_t htonl(uint32_t nb) {
	return ((nb>>24)&0xff)      |
		((nb<<8)&0xff0000)   |
		((nb>>8)&0xff00)     |
		((nb<<24)&0xff000000);
}
uint32_t IP(uint8_t a,uint8_t b,uint8_t c,uint8_t d){
	uint32_t addr=(a<<24)|(b<<16)|(c<<8)|d;
	return htonl(addr);
}
