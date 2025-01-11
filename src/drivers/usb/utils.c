#include "utils.h"
#include "stdlib/stdio.h"

void USB_PrintEndpoint(XHCI_CONTEXT_ENDPOINT* ep){

    kprintf("+--------------------------------------+-----------------------------------+\n");
    kprintf("| Field                                | Value                             |\n");
    kprintf("+--------------------------------------+-----------------------------------+\n");

    kprintf("| Endpoint State                       | %u                                 |\n", ep->ep_state);
    kprintf("| Max Bursts (Mult)                    | %u                                 |\n", ep->mult);
    kprintf("| Max Primary Streams                  | %u                                 |\n", ep->max_p_streams);
    kprintf("| Linear Stream Array (LSA)            | %u                                 |\n", ep->lsa);
    kprintf("| Interval (in 125μs increments)       | %u                                 |\n", ep->interval);
    kprintf("| Max ESIT Payload High                | %u                                 |\n", ep->max_esit_payload_hi);
    kprintf("| Error Count (CErr)                   | %u                                 |\n", ep->c_err);
    kprintf("| Endpoint Type                        | %u                                 |\n", ep->ep_type);
    kprintf("| Host Initiate Disable (HID)          | %u                                 |\n", ep->hid);
    kprintf("| Max Burst Size                       | %u                                 |\n", ep->max_burst_size);
    kprintf("| Max Packet Size                      | %u bytes                          |\n", ep->max_packet_size);
    kprintf("| Transfer Ring Dequeue Pointer        | 0x%llX                            |\n", ep->tr_dequeue_pointer);
    kprintf("| Average TRB Length                   | %u bytes                          |\n", ep->avg_trb_length);
    kprintf("| Max ESIT Payload Low                 | %u bytes                          |\n", ep->max_esit_payload_lo);

    kprintf("+--------------------------------------+-----------------------------------+\n");
}
