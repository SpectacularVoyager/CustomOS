#pragma once
#include "utils/bit.h"
#include <stdint.h>

#define USB_REQUEST_TYPE(dir,type,recipient)	((((dir)&0x1)<<7)|(((type)&0x3)<<5)|(((recipient)&0x1F)<<0))

#define USB_GET_DESC_VAL(index,type) ((index)|((type)<<8))

#define USB_DIRECTION_H2D	(0)
#define USB_DIRECTION_D2H	(1)

#define USB_DESC_TYPE_DEVICE	(0x1)
#define USB_DESC_TYPE_CONFIG	(0x2)
#define USB_DESC_TYPE_STRING	(0x3)
#define USB_DESC_TYPE_INTERFACE	(0x4)

#define USB_REQUEST_GET_STATUS				(0 )
#define USB_REQUEST_CLEAR_FEATURE			(1 )
#define USB_REQUEST_RESERVED_FUTURE_USE_1	(2 )
#define USB_REQUEST_SET_FEATURE				(3 )
#define USB_REQUEST_RESERVED_FUTURE_USE_2	(4 )
#define USB_REQUEST_SET_ADDRESS				(5 )
#define USB_REQUEST_GET_DESCRIPTOR			(6 )
#define USB_REQUEST_SET_DESCRIPTOR			(7 )
#define USB_REQUEST_GET_CONFIGURATION		(8 )
#define USB_REQUEST_SET_CONFIGURATION		(9 )
#define USB_REQUEST_GET_INTERFACE			(10)
#define USB_REQUEST_SET_INTERFACE			(11)
#define USB_REQUEST_SYNCH_FRAME				(12)
#define USB_REQUEST_SET_SEL					(48)
#define USB_REQUEST_SET_ISOCH_DELAY			(49)

typedef struct {
	uint8_t len;
	uint8_t type;
	uint16_t str;
}__attribute__((packed)) USB_STRING_DESC_VAL;

typedef struct {
	uint8_t len;
	uint8_t type;
	uint16_t usb_release;
	uint8_t clazz;
	uint8_t subclass;
	uint8_t protocol;
	uint8_t maxpackets;
	uint16_t vendorid;
	uint16_t productid;
	uint16_t bcd_device;
	uint8_t manufacturer_idx;
	uint8_t product_idx;
	uint8_t serial_idx;
	uint8_t numConfigs;
}__attribute__((packed)) USB_DEVICE_DESCRIPTOR;

typedef struct {
	uint8_t len;
	uint8_t config_len;
	uint16_t total_len;
	uint8_t num_interfaces;
	uint8_t config_val;
	uint8_t config_index;
	uint8_t attribs;
	uint8_t max_power;
}__attribute__((packed)) USB_CONFIG_DESCRIPTOR;

typedef struct {
	uint8_t len;
	uint8_t type;
	uint8_t interface_number;
	uint8_t alternate;
	uint8_t num_endpoints;
	uint8_t clazz;
	uint8_t subclazz;
	uint8_t protocol;
	uint8_t interface_index;
}__attribute__((packed)) USB_INTERFACE_DESCRIPTOR;

