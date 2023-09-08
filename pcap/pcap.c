#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <arpa/inet.h>

typedef struct pcap_hdr_s {
  uint32_t magic_number;   /* magic number */
  uint16_t version_major;  /* major version number */
  uint16_t version_minor;  /* minor version number */
  int32_t  thiszone;       /* GMT to local correction */
  uint32_t sigfigs;        /* accuracy of timestamps */
  uint32_t snaplen;        /* max length of captured packets, in octets */
  uint32_t network;        /* data link type */
} pcap_hdr_t;

typedef struct pcap_packet_hdr_s {
  uint32_t ts_seconds;
  uint32_t ts_fraction;
  uint32_t captured_packet_length;
  uint32_t original_packet_length;
} pcap_packet_hdr_t;

char * INPUT = "synflood.pcap";

int main(){
  int fd;

  fd = open(INPUT, O_RDONLY);

  size_t header_size = sizeof(pcap_hdr_t);
  size_t packet_header_size = sizeof(pcap_packet_hdr_t);

  unsigned char * buf = calloc(1, header_size + packet_header_size);
  ssize_t res = read(fd, buf, header_size + packet_header_size);
  if(res == -1){
    perror("Unable to read");
  }
  //for (size_t i = 0; i < header_size; ++i) {
  //  printf("%02x ", buf[i]);
  //}

  // da buffa (ahnold voice)
  pcap_hdr_t * global = (pcap_hdr_t *) buf;

  /*
   * Magic Number (32 bits): an unsigned magic number, whose value is either the hexadecimal number 0xA1B2C3D4 or the hexadecimal number 0xA1B23C4D.
   * If the value is 0xA1B2C3D4, time stamps in Packet Records (see Figure 2) are in seconds and microseconds; if it is 0xA1B23C4D, time stamps in Packet Records are in seconds and nanoseconds.
   * These numbers can be used to distinguish sections that have been saved on little-endian machines from the ones saved on big-endian machines, and to heuristically identify pcap files.
   * https://datatracker.ietf.org/doc/id/draft-gharris-opsawg-pcap-00.html
   */
  printf("magic number %x\n", global->magic_number); // prints a1b2c3d4

  printf("snapshot length (max length of captured packets, in octets) %d\n", global->snaplen);

  //printf("data link type %d\n", global->network & 0xFFFB); // prints zero
  printf("data link type %x\n", global->network);  // prints zero

  // Because we have a data link type of zero, that means we have LINKTYPE_NULL
  // which refers to BSD loopback encapsulation https://www.tcpdump.org/linktypes.html
  // which means that the following frame is a link layer header:
  // "the link layer header is a 4-byte field, in host byte order, containing a value of 2 for IPv4 packets, a value of either 24, 28, or 30 for IPv6 packets"

  pcap_packet_hdr_t * packet = (pcap_packet_hdr_t *) (buf + header_size);
  printf("cap length %d\n", packet->captured_packet_length); // prints 44 bytes

  // read in the next 44 bytes (captured packet length)
  unsigned char * packet_cap = calloc(packet->captured_packet_length, sizeof(unsigned char));
  if(read(fd, packet_cap, packet->captured_packet_length) == -1){
    perror("Unable to read");
  }

  // get the link type
  printf("%d (2 means PF_INET or IP protocol)\n", *packet_cap); // 2 => PF_INET
                                                                // aka ipv4
  // the 4 byte link-layer header indicates an IPv4 packet
  // so the remaining 40 bytes must be an IPv4 packet

  /* why do we shift four?
     due to the layout of the IP version field and Internet Header Length (IHL) that are located in the first byte (not 4th byte)
     of an IP header.
     The IP version field is actually the most significant 4 bits of the first byte of an ipv4 packet.
     So if you're reading it as a full byte (8 bits), you're also incorporating the IHL (Internet Header Length) which is the second 4 bits of the first byte.
     In order to isolate the version you need to shift the first byte 4 bits to the right.
     This is because the IP version information is in the high 4 bits of the first byte, not in the following 4 bytes.
  */
  printf("%d (IP version field, should be 4)\n", *(packet_cap + 4) >> 4);

  // struct iphdr comes from linux/ip.h
  struct iphdr * foo = (struct iphdr *) ((void *)packet_cap + 4);
  printf("%hhu (protocol, TCP is 6)\n", foo->protocol);
  printf("%hu (IP version field, should be 4)\n", foo->version);

  printf("\n%lu (IP header size)\n", sizeof(struct iphdr));
  printf("%lu (TCP header size)\n", sizeof(struct tcphdr));

  struct tcphdr * bar = (struct tcphdr *) ((void *)foo + sizeof(struct iphdr));
  printf("\nSkipped ahead %lu bytes\n", (uint64_t)bar - (uint64_t)foo);
  printf("%u (seq)\n", ntohl(bar->seq));
  printf("%u (ack seq)\n", ntohl(bar->ack_seq));
  printf("%u (source)\n", ntohs(bar->source));
  printf("%u (dest)\n", ntohs(bar->dest));
  printf("%u (win)\n", ntohs(bar->window));

  return 0;
}
