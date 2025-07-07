#pragma once

#include "stdint.h"

#define ETH_TYPE_ARP	0x806
#define ETH_TYPE_IP		0x800

#define ARP_OP_REQ		1
#define ARP_OP_REPLY	2

#define IP_PROTO_ICMP	1

#define ICMP_TYPE_ECHO	8

uint16_t htons(uint16_t nb);

uint32_t htonl(uint32_t nb) ;

uint32_t IP(uint8_t a,uint8_t b,uint8_t c,uint8_t d);
typedef struct {
	uint16_t src_port;
	uint16_t dest_port;
	uint16_t len;
	uint16_t checksum;
	uint8_t data[1];
} __attribute__((packed)) PACKET_UDP;

typedef struct {
	uint8_t header_len:4;
	uint8_t version:4;
	uint8_t tos;
	uint16_t len;
	uint16_t ident;
	uint16_t flags:3;
	uint16_t frag_off:13;
	uint8_t ttl;
	uint8_t protocol;
	uint16_t header_checksum;
	uint32_t source_ip;
	uint32_t dest_ip;
	uint8_t data[1];
} __attribute__((packed)) PACKET_IP;

// header ... data ... trailer
typedef struct {
	uint8_t mac_src[6];
	uint8_t mac_dest[6];
	uint16_t type;
	uint8_t data[1];
} __attribute__((packed)) PACKET_ETHERNET2_BEGIN;

typedef struct {
	uint16_t hardware_type;
	uint16_t protocol_type;
	uint8_t hardware_len;
	uint8_t protocol_len;
	uint16_t operation;
	uint8_t senderHardware[6];
	uint8_t senderProtocol[6];
	uint8_t targetHardware[4];
	uint8_t targetProtocol[4];
} __attribute__((packed)) PACKET_ARP_UNIX;
typedef struct {
	uint16_t hardware_type;
	uint16_t protocol_type;
	uint8_t hardware_len;
	uint8_t protocol_len;
	uint16_t operation;
	uint8_t data[1];
} __attribute__((packed)) PACKET_ARP;
typedef struct {
	uint8_t opcode;
	uint8_t hardware_type;
	uint8_t address_len;
	uint8_t hops;
	uint32_t TransactionID;
	uint16_t timeElapsed;
	uint16_t flags;
	uint32_t ClientAddr;
	uint32_t AssignedAddr;
	uint32_t NextServerAddr;
	uint32_t RelayAgentAddr;
	uint8_t MAC[16];
	char server_name[64];
	char file_name[128];
	uint32_t MAGIC;
	uint8_t options[1];
} __attribute__((packed)) PACKET_DHCP;

typedef uint16_t net16;
typedef struct {
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
	union{
		struct{
			uint16_t ident;
			uint16_t seq;
			char data[1];
		} echo;
		char raw[1];
	} data;
} __attribute__((packed)) PACKET_ICMP;

void ETH_ADDTRAILER(void* eth_end,uint32_t trailer);
void addARP(PACKET_ETHERNET2_BEGIN* eth,PACKET_ARP* arp);
void addARPUnix(PACKET_ETHERNET2_BEGIN* eth,PACKET_ARP_UNIX* arp);
void addIP(PACKET_ETHERNET2_BEGIN* eth,PACKET_IP* ip);
void addUDP(PACKET_IP* ip,int src,int dest,void* data,int len);

void IP_default(PACKET_IP* ip,unsigned long src,unsigned long dest,int ident,int len);

void ETHERNET_default(PACKET_ETHERNET2_BEGIN* eth,uint8_t* src,uint8_t* dest,unsigned int type);

unsigned long DHCP_default(PACKET_DHCP* dhcp,int op,uint8_t MAC[6]);

inline void* ARP_GET_SENDER_ADDR(PACKET_ARP* arp){
	return arp->data+0;
}
inline void* ARP_GET_SENDER_PROTOCOL(PACKET_ARP* arp){
	return arp->data+arp->hardware_len;
}
inline void* ARP_GET_TARGET_ADDR(PACKET_ARP* arp){
	return arp->data+arp->hardware_len+arp->protocol_len;
}
inline void* ARP_GET_TARGET_PROTOCOL(PACKET_ARP* arp){
	return arp->data+2*arp->hardware_len+arp->protocol_len;
}
inline int ARP_SIZE(PACKET_ARP* arp){
	return sizeof(PACKET_ARP)+2*arp->hardware_len+arp->protocol_len*2;
}
unsigned int checksum(void* _data,int len);

#define DHCP_OPTION_MESSAGE_TYPE	0x35
#define DHCP_OPTION_REQUEST_LIST	0x37
#define DHCP_OPTION_END				0xFF
#define DHCP_MAGIC_COOKIE			0x63825363

#define DHCP_MESSAGE_TYPE_BOOT_REQUEST	0x1
