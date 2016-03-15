//888888888888888888888888888888888888888888
//---------[Prerequisites for ACH]----------
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ach.h>
//888888888888888888888888888888888888888888

#define		FLOCK_BOTS_CHAN_REF_NAME         "flock-bots-ref"    ///> flock-bots ach channel
#define  WHEEL_LEFT 0 	//> left wheel
#define  WHEEL_RIGHT 1 	//> right wheel

typedef struct flock_bots_ref {
  double ref[2]; 		//< reference or let and righ wheel
}__attribute__((packed)) flock_bots_ref_t;
