#include "usb.h"
#include "stdlib/stdlib.h"
#include "utils/utils.h"

void USB_PARSE_CONFIG(USB_DEVICE_CONFIGURATION* config,void* data){
	config->config=data;
	config->interfaces=malloc(sizeof(USB_CONFIGURATION_INTERFACE*)*config->config->num_interfaces);
	data+=config->config->len;
	config->interfaces[0].interface=data;
	int n_endp=config->interfaces[0].interface->num_endpoints;
	config->interfaces[0].endpoints=malloc(sizeof(void*)*n_endp);
	data+=config->interfaces->interface->len;
	USB_ENDPOINT_DESCRIPTOR* desc;
	int i=0;
	while((desc=data)->type!=USB_DESC_TYPE_ENDPOINT){
		data+=desc->len;
	}

	config->interfaces[0].endpoints=data;
}
