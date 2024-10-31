#pragma once
#include "utils/bit.h"

#define USB_REQUEST_TYPE(dir,type,recipient)	((((dir)&0x1)<<7)|(((type)&0x3)<<5)|(((recipient)&0x1F)<<0))

#define USB_GET_DESC_VAL(index,type) ((index)|((type)<<8))

#define USB_DIRECTION_H2D	(0)
#define USB_DIRECTION_D2H	(1)

#define USB_DESC_TYPE_DEVICE	(0x1)
#define USB_DESC_TYPE_STRING	(0x3)

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

