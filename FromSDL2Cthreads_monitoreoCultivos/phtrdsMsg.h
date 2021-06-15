/*******************************************************************************
*
*  phtrdsMsgLyr.h - Service routines for concurrent access to a circular buffer
*                   modelling a message queue
*
*******************************************************************************/

#include    <pthread.h>
#include    <semaphore.h>
#include    "cultivosConf.h"    /* User-defined constants and structs */

#define     LOCAL       0     /* semaphores will not be shared with other processes */

typedef struct                            /* message queue structure */
{
  msg_t             buffer [BUFSIZE];     /* circular buffer */
  int               bufin;                /* nxt free slot ndx */
  int               bufout;               /* nxt msg slot */
  pthread_mutex_t   buffer_lock;          /* mutex for buffer */
  sem_t             items;                /* semaphore for items */
  sem_t             slots;                /* semaphore for slots */
} msgq_t;

extern msgq_t   queue [NUM_QUEUES];       /* declare queue as an array of message queues */

void  initialiseQueues  ( void );
void  destroyQueues     ( void );
void  sendMessage       ( msgq_t *queue_ptr, msg_t msg );
msg_t receiveMessage    ( msgq_t *queue_ptr );
