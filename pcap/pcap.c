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

char * INPUT = "synflood.pcap";

void main(){
  int fd;

  fd = open(INPUT, O_RDONLY);

  size_t header_size = sizeof(pcap_hdr_t);
  unsigned char * buf = calloc(1, header_size);
  read(fd, buf, header_size);
  //for (size_t i = 0; i < header_size; ++i) {
  //  printf("%02x ", buf[i]);
  //}

  pcap_hdr_t * global = (pcap_hdr_t *) buf;
  printf("%x", global->magic_number);
}
