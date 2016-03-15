/* 
 * udpserver.c - A simple UDP echo server 
 * usage: udpserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

/* For Ach IPC */
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include <inttypes.h>
#include <ach.h>

/* Flock Bots */
#include "flock-bots.h"

#define BUFSIZE 1024




/* ach_channel IDs */
ach_channel_t chan_flock_bots_ref; // flockbots reference channel 



/*
 * error - wrapper for perror
 */
void error(char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char **argv) {
  int sockfd; /* socket */
  int portno; /* port to listen on */
  int clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  struct hostent *hostp; /* client host info */
  char buf[BUFSIZE]; /* message buf */
  char *hostaddrp; /* dotted decimal host addr string */
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */


  /* Create Ach Channel */
  struct flock_bots_ref F_ref;
  memset( &F_ref, 0, sizeof(F_ref));

  enum ach_status r0 = ach_create( FLOCK_BOTS_CHAN_REF_NAME, 10, 512, NULL );
  if( ACH_OK != r0 ) {
    fprintf( stderr, "Channel Status: %s\n", ach_result_to_string(r0) );
    if(ACH_EEXIST != r0){
      exit(EXIT_FAILURE);
    }
  }

  /* Open Ach Channel */
  enum ach_status r = ach_open(&chan_flock_bots_ref, FLOCK_BOTS_CHAN_REF_NAME , NULL);
  assert( ACH_OK == r );


  /* Reset the Channel */
   /* Write to the feed-forward channel */
   ach_put( &chan_flock_bots_ref, &F_ref, sizeof(F_ref));


  /* 
   * check command line arguments 
   */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  portno = atoi(argv[1]);

  /* 
   * socket: create the parent socket 
   */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) 
    error("ERROR opening socket");

  /* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
   */
  optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
	     (const void *)&optval , sizeof(int));

  /*
   * build the server's Internet address
   */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)portno);

  /* 
   * bind: associate the parent socket with a port 
   */
  if (bind(sockfd, (struct sockaddr *) &serveraddr, 
	   sizeof(serveraddr)) < 0) 
    error("ERROR on binding");

  /* 
   * main loop: wait for a datagram, then echo it
   */
  clientlen = sizeof(clientaddr);
  while (1) {

    /*
     * recvfrom: receive a UDP datagram from a client
     */
    bzero(buf, BUFSIZE);
    n = recvfrom(sockfd, buf, BUFSIZE, 0,
		 (struct sockaddr *) &clientaddr, &clientlen);
    if (n < 0)
      error("ERROR in recvfrom");

    /* 
     * gethostbyaddr: determine who sent the datagram
     */
    hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
			  sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    if (hostp == NULL)
      error("ERROR on gethostbyaddr");
    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL)
      error("ERROR on inet_ntoa\n");
    printf("server received datagram from %s (%s)\n", 
	   hostp->h_name, hostaddrp);
    printf("server received %d/%d bytes: %s\n", (int)strlen(buf), (int)n, buf);
   


    /* Split buffer */
    char * pch;
    pch = strtok (buf," ");
    while (pch != NULL)
    {
      printf ("%s\n",pch);
      
      /* Compare */
      char* lhs = "joy";
      int rc = strcmp(lhs, pch);
      if(rc == 0){
        printf("-------joy------\n");
        pch = strtok (NULL, " ");
      /* Joy Stick */
        while (pch != NULL){
          char* joy_right = "right";
          int rc_joy_right = strcmp(joy_right, pch);
          if(rc_joy_right == 0){
            printf("-------right------\n");
            pch = strtok (NULL, " ");
            int i = 0;
            while (pch != NULL){
              F_ref.ref[i] = atof(pch);
              i++;
              pch = strtok (NULL, " ");
              if( i == 2 ){
                ach_put( &chan_flock_bots_ref, &F_ref, sizeof(F_ref));
              }
            }
          pch = strtok (NULL, " ");
          }
        }
      }
      pch = strtok (NULL, " ");
    }
    
    printf("ach chan = %f , %f\n", F_ref.ref[0],F_ref.ref[1]);

 
    /* 
     * sendto: echo the input back to the client 
     */
    n = sendto(sockfd, buf, strlen(buf), 0, 
	       (struct sockaddr *) &clientaddr, clientlen);
    if (n < 0) 
      error("ERROR in sendto");
  }
}
