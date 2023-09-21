#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <arpa/inet.h>
#include <assert.h>

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

  struct stat statObj;
  if(fstat(fd, &statObj) < 0)
      return -1;
  off_t size = statObj.st_size;
  printf("%ld \t File size\n", size);

  unsigned char * buf = calloc(1, size);
  ssize_t bytes_read;
  if((bytes_read = read(fd, buf, size)) == -1){
    perror("Unable to read");
  }
  printf("%ld \t Bytes read\n", bytes_read);

  pcap_hdr_t * global = (pcap_hdr_t *) buf;
  void * end = (unsigned char *)buf + size;

  /*
   * Magic Number (32 bits): an unsigned magic number, whose value is either the hexadecimal number 0xA1B2C3D4 or the hexadecimal number 0xA1B23C4D.
   * If the value is 0xA1B2C3D4, time stamps in Packet Records (see Figure 2) are in seconds and microseconds; if it is 0xA1B23C4D, time stamps in Packet Records are in seconds and nanoseconds.
   * These numbers can be used to distinguish sections that have been saved on little-endian machines from the ones saved on big-endian machines, and to heuristically identify pcap files.
   * https://datatracker.ietf.org/doc/id/draft-gharris-opsawg-pcap-00.html
   */
  printf("magic number %x\n", global->magic_number); // prints a1b2c3d4
  printf("snapshot length (max length of captured packets, in octets) %d\n", global->snaplen);
  printf("data link type %x\n", global->network);  // prints zero
  assert(global->network == 0);

  pcap_packet_hdr_t * pcap_packet_hdr = (pcap_packet_hdr_t *) ((unsigned char *)global + sizeof(pcap_hdr_t));

  int n = 1;
  int syn_count = 0;
  int ack_count = 0;
  int syn_ack_count = 0;
  while((void *)pcap_packet_hdr < (void *)end){
    // Because we have a data link type of zero, that means we have LINKTYPE_NULL
    // which refers to BSD loopback encapsulation https://www.tcpdump.org/linktypes.html
    // which means that the following frame is a link layer header:
    // "the link layer header is a 4-byte field, in host byte order, containing a value of 2 for IPv4 packets, a value of either 24, 28, or 30 for IPv6 packets"

    uint32_t * link_layer_hdr = (uint32_t *) ((unsigned char * )pcap_packet_hdr + sizeof(pcap_packet_hdr_t));
    struct iphdr * ip_header = (struct iphdr *) ((unsigned char *)link_layer_hdr + sizeof(uint32_t));
    struct tcphdr * tcp_header = (struct tcphdr *) ((unsigned char *)ip_header + sizeof(struct iphdr));

    // 44 bytes = 4 byte link layer hdr, 20 byte iphdr, 20 byte tcphdr
    // to advance, the next pcap packet header is global + n*44 or global + n*(captured_packet_length)
    //printf("\nCaptured %d bytes\n", pcap_packet_hdr->captured_packet_length); // prints 44 bytes
    if(n == 0) assert(pcap_packet_hdr->captured_packet_length == 44);

    // get the link type
    //printf("%d (2 means PF_INET or IP protocol)\n", *link_layer_hdr); // 2 => PF_INET
                                                                    // aka ipv4
    assert(*link_layer_hdr == 2);
    // the 4 byte link-layer header indicates an IPv4 packet
    // so the remaining 40 bytes must be an IPv4 packet
    // printf("%lu (IP header size)\n", sizeof(struct iphdr)); // 20
    // printf("%lu (TCP header size)\n", sizeof(struct tcphdr)); // 20

    // struct iphdr comes from linux/ip.h
    //printf("%hhu (protocol, TCP is 6)\n", ip_header->protocol); // 6
    assert(ip_header->protocol == 6);
    // printf("%hu (IP version field, should be 4)\n", ip_header->version); // 4
    assert(ip_header->version == 4);

    // ChatGPT saved my sanity by pointing out that these are in network byte order
    // so I must convert to host byte order using ntohl() / noths()
    if(tcp_header->syn && tcp_header->ack){
        syn_ack_count++;
    }else if(tcp_header->syn){
        syn_count++;
    }else if(tcp_header->ack){
        ack_count++;
    }
    //printf("%u (seq)\n", ntohl(tcp_header->seq));
    //printf("%u (ack seq)\n", ntohl(tcp_header->ack_seq));
    //printf("%u (source)\n", ntohs(tcp_header->source));
    //printf("%u (dest)\n", ntohs(tcp_header->dest));
    //printf("%u (win)\n", ntohs(tcp_header->window));
    if(n == 0) assert(ntohs(tcp_header->window) == 512);


    // to advance, the next pcap packet header is:
    //   pcap_packet_hdr + sizeof(pcap_packet_hdr_t) + captured_packet_length
    pcap_packet_hdr = (pcap_packet_hdr_t *) ((unsigned char *)pcap_packet_hdr + sizeof(*pcap_packet_hdr) + pcap_packet_hdr->captured_packet_length);
    n++;
  }

  printf("Num syn: %d\n", syn_count);
  printf("Num syn_ack: %d\n", syn_ack_count);
  printf("Num ack: %d\n", ack_count);

  printf("Ratio syn_ack/syn: %f\n", (float)syn_ack_count/syn_count);

  return 0;
}
