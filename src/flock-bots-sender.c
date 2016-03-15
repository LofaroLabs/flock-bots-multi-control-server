/* 
 * tcpclient.c - A simple TCP client
 * usage: tcpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

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
    exit(0);
}


int8_t* getWheels(double x, double y){


  int8_t* c = malloc(sizeof(int8_t)*2);
  c[0] = 0;
  c[1] = 0;


  double xx = 100.0*x;
  double yy = 100.0*y;
  double v = (100.0-abs(xx))*(yy/100.0)+yy;
  double w = (100.0-abs(yy))*(xx/100.0)+xx;
  double r = (v+w)/2.0;
  double l = (v-w)/2.0;

  if (r > 100.0)  r =  100.0;
  if (r < -100.0) r = -100.0;
  if (l > 100.0)  l =  100.0;
  if (l < -100.0) l = -100.0;

  c[0] = (int8_t)l;
  c[1] = (int8_t)r;

//  double a = atan2(y,x);
//  double h = sqrt(x*x + y*y);


  return c;
}


int main(int argc, char **argv) {
    int sockfd, portno, n;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];


    /* open ach */
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



    /* check command line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* connect: create a connection with the server */
    if (connect(sockfd, &serveraddr, sizeof(serveraddr)) < 0) 
      error("ERROR connecting");
    while(1){
      /* Get the current feed-forward (state) */
      /* for size check */
      size_t fs;
      r = ach_get( &chan_flock_bots_ref, &F_ref, sizeof(F_ref), &fs, NULL, ACH_O_WAIT );
      if(ACH_OK != r) {
          assert( sizeof(F_ref) == fs );
      }

      /* get message line from the user */
//      printf("Please enter msg: ");
//      bzero(buf, BUFSIZE);
//      fgets(buf, BUFSIZE, stdin);

      /* send the message line to the server */
      
      int8_t* c = getWheels(F_ref.ref[0],F_ref.ref[1]);
      uint8_t* buf = malloc(sizeof(int8_t)*6);
      buf[0] = 0x5;
      buf[1] = 'D';
      buf[2] = 'M';
      buf[3] = 'W';
      memset( &buf[4], &c[0], sizeof(int8_t));
      memset( &buf[5], &c[1], sizeof(int8_t));


      n = write(sockfd, buf, strlen(buf));
      if (n < 0) 
        error("ERROR writing to socket");
        connect(sockfd, &serveraddr, sizeof(serveraddr));
    }

    close(sockfd);
    return 0;
}
