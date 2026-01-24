#include "pcap.h"

void packet_handler(u_char *args, const struct pcap_pkthdr *header,
                    const u_char *packet) {
  printf("Packet captured with length %d\n", header->len);
}
int main() {

  pcap_if_t *alldevsp, *device;
  pcap_t *handle;

  char errbuf[PCAP_ERRBUF_SIZE];

  if (pcap_findalldevs(&alldevsp, errbuf) != 0) {

    fprintf(stderr, "Error finding devices %s\n", errbuf);

    return 1;
  }

  for (pcap_if_t *d = alldevsp; d; d = d->next) {
    printf("%s\n", d->name);
    // if (d->description) {
    //   printf("%s\n", d->description);
    // }
  }

  device = alldevsp;
  handle = pcap_open_live(device->name, BUFSIZ, 1, 1000, errbuf);

  // capture packets with the handle
}
