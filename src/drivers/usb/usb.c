#include "usb.h"
#include "stdlib/stdlib.h"
#include "utils/utils.h"

void USB_PARSE_CONFIG(USB_DEVICE_CONFIGURATION* config,void* data){
	config->config=data;
	if(config->config->num_interfaces!=1)printf("DO NOT SUPPORT NUM INTERFACE APART FROM 1 FOUND 0x%x",config->config->num_interfaces);
	data+=config->config->len;
	config->intf=malloc(sizeof(USB_CONFIGURATION_INTERFACE));
	config->intf->interface=data;
	data+=config->intf->interface->len;
	config->intf->descriptors=data;
	config->intf->endpoint=malloc(sizeof(void**)*config->intf[0].interface->num_endpoints);
	int i=0;
	while(i<config->intf[0].interface->num_endpoints){
		while(1){
			USB_BASE_DESCRIPTOR* desc=data;
			if(desc->type==USB_DESC_TYPE_ENDPOINT)break;
			data+=desc->len;
		}
		config->intf->endpoint[i]=data;
		data+=((USB_BASE_DESCRIPTOR*)data)->len;
		i++;
	}

}
