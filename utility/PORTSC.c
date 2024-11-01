#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint8_t CCS : 1;  // Current Connect Status
    uint8_t PED : 1;  // Port Enabled/Disabled
	uint8_t res1: 1;
    uint8_t OCA : 1;  // Over-current Active
    uint8_t PR : 1;   // Port Reset
    uint8_t PLS : 4;  // Port Link State
    uint8_t PP : 1;   // Port Power
    uint8_t Speed : 4; // Port Speed (Protocol Speed ID)
    uint8_t PIC : 2;  // Port Indicator Control
    uint8_t LWS : 1;  // Port Link State Write Strobe
    uint8_t CSC : 1;  // Connect Status Change
    uint8_t PEC : 1;  // Port Enabled/Disabled Change
    uint8_t WRC : 1;  // Warm Port Reset Change
    uint8_t OCC : 1;  // Over-current Change
    uint8_t PRC : 1;  // Port Reset Change
    uint8_t PLC : 1;  // Port Link State Change
    uint8_t CEC : 1;  // Port Config Error Change
    uint8_t CAS : 1;  // Cold Attach Status
    uint8_t WCE : 1;  // Wake on Connect Enable
    uint8_t WDE : 1;  // Wake on Disconnect Enable
    uint8_t WOE : 1;  // Wake on Over-current Enable
	uint8_t res2: 2;
    uint8_t DR : 1;   // Device Removable
    uint8_t WPR : 1;  // Warm Port Reset
} __attribute__((packed))USBPortStatus;

void pretty_print(USBPortStatus portStatus) {
    printf("Current Connect Status (CCS): %d\n", portStatus.CCS);
    printf("Port Enabled/Disabled (PED): %d\n", portStatus.PED);
    printf("Over-current Active (OCA): %d\n", portStatus.OCA);
    printf("Port Reset (PR): %d\n", portStatus.PR);
    printf("Port Link State (PLS): %d\n", portStatus.PLS);
    printf("Port Power (PP): %d\n", portStatus.PP);
    printf("Port Speed (Speed): %d\n", portStatus.Speed);
    printf("Port Indicator Control (PIC): %d\n", portStatus.PIC);
    printf("Port Link State Write Strobe (LWS): %d\n", portStatus.LWS);
    printf("Connect Status Change (CSC): %d\n", portStatus.CSC);
    printf("Port Enabled/Disabled Change (PEC): %d\n", portStatus.PEC);
    printf("Warm Port Reset Change (WRC): %d\n", portStatus.WRC);
    printf("Over-current Change (OCC): %d\n", portStatus.OCC);
    printf("Port Reset Change (PRC): %d\n", portStatus.PRC);
    printf("Port Link State Change (PLC): %d\n", portStatus.PLC);
    printf("Port Config Error Change (CEC): %d\n", portStatus.CEC);
    printf("Cold Attach Status (CAS): %d\n", portStatus.CAS);
    printf("Wake on Connect Enable (WCE): %d\n", portStatus.WCE);
    printf("Wake on Disconnect Enable (WDE): %d\n", portStatus.WDE);
    printf("Wake on Over-current Enable (WOE): %d\n", portStatus.WOE);
    printf("Device Removable (DR): %d\n", portStatus.DR);
    printf("Warm Port Reset (WPR): %d\n", portStatus.WPR);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hexadecimal>\n", argv[0]);
        return 1;
    }

    uint32_t inputHex = (uint32_t)strtoul(argv[1], NULL, 16);
    USBPortStatus portStatus;
	*(unsigned int*)&portStatus=inputHex;

    // Pretty print the struct
    pretty_print(portStatus);

    return 0;
}

