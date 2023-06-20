#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "glib-2.0/glib.h"


typedef struct pcap_hdr_s {
  guint32 magic_number;   /* magic number */
  guint16 version_major;  /* major version number */
  guint16 version_minor;  /* minor version number */
  gint32  thiszone;       /* GMT to local correction */
  guint32 sigfigs;        /* accuracy of timestamps */
  guint32 snaplen;        /* max length of captured packets, in octets */
  guint32 network;        /* data link type */
} pcap_hdr_t;

typedef struct pcap_packet_hdr_s {
  guint32 ts_seconds;
  guint32 ts_fraction;
  guint32 captured_packet_length;
  guint32 original_packet_length;
} pcap_packet_hdr_t;

char * INPUT = "synflood.pcap";

void main(){
  int fd;

  fd = open(INPUT, O_RDONLY);

  size_t header_size = sizeof(pcap_hdr_t);
  size_t packet_header_size = sizeof(pcap_packet_hdr_t);

  unsigned char * buf = calloc(1, header_size + packet_header_size);
  read(fd, buf, header_size + packet_header_size);
  //for (size_t i = 0; i < header_size; ++i) {
  //  printf("%02x ", buf[i]);
  //}

  pcap_hdr_t * global = (pcap_hdr_t *) buf;
  printf("magic number %x\n", global->magic_number);
  printf("snapshot length %d\n", global->snaplen);

  pcap_packet_hdr_t * packet = (pcap_packet_hdr_t *) (buf + packet_header_size);
  printf("cap length %d\n", packet->captured_packet_length);
}
