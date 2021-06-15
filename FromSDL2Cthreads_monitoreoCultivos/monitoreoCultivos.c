/*******************************************************************************
*
*   monitoreoCultivos.c - Made by Juan Cuellar, Nicolas Ibagon, Francisco Suarez and Santiago Uribe
*
*******************************************************************************/

#include    <stdio.h>
#include    <stdlib.h>
#include    <time.h>
#include    <unistd.h>
#include    <string.h>
#include    <errno.h>
#include    "phtrdsMsg.h"

#define NUM_CONSOLE 1
#define NUM_CONTROLLERS 2
#define NUM_SENSORS_HUM 4
#define NUM_SENSORS_TEMP 4
#define NUM_SENSORS_PH 4
#define NUM_SENSORS (NUM_SENSORS_HUM + NUM_SENSORS_TEMP + NUM_SENSORS_PH)
#define SENSORID_MIN 3
#define SENSORID_MAX (NUM_SENSORS + 2)

static void *pConsole ( void *arg_ptr );    /* pConsole process code */
static void *pController ( void *arg_ptr );  /* pController process code */
static void *pSenHumidity ( void *arg_ptr );  /* pSenHumidity process code */
static void *pSenTemperature( void *arg_ptr );  /* pSenTemperature process code */
static void *pSenPH ( void *arg_ptr );  /* pSenPH process code */

int main ( void ){
  pthread_t   pConsole_tid;               /* pCentral process tid */

  /* Create queues */
  initialiseQueues ();

  /* Create threads */
  pthread_create ( &pConsole_tid, NULL, pConsole, NULL );

  /* Wait for threads to finish */
  pthread_join ( pConsole_tid, NULL );

  /* Destroy queues */
  destroyQueues ();

  return ( 0 );
}

static void *pConsole ( void *arg_ptr ){
  CONSOLE_STATES    state, state_next;
  msg_t             InMsg, OutMsg;
  pthread_t         pControllers[NUM_CONTROLLERS+1], pSensors[NUM_SENSORS+1];
  int               index;
  int               SenderQ;

  //Create the instances of the controllers
  for ( index = 1; index <= NUM_CONTROLLERS; index++ ){
    pthread_create ( &pControllers[index], NULL, pController, (void *) &index );
    sleep ( 1 );
  }
  //Create the instances of the sensors
  for ( index=index; index <= NUM_SENSORS+NUM_CONTROLLERS; index++ ){
    if (index%3 == 0){
      pthread_create ( &pSensors[index-NUM_CONTROLLERS], NULL, pSenHumidity, (void *) &index );
      sleep ( 1 );
    }
    else if (index%3 == 1){
      pthread_create ( &pSensors[index-NUM_CONTROLLERS], NULL, pSenTemperature, (void *) &index );
      sleep ( 1 );
    }
    else if (index%3 == 2){
      pthread_create ( &pSensors[index-NUM_CONTROLLERS], NULL, pSenPH, (void *) &index );
      sleep ( 1 );
    }
  }
  state_next = WaitingController;
  printf ( "Bienvenido al Monitoreo de Invernaderos!\n" );
  //First call to signal sGetData
  OutMsg.signal = (int) sGetData;
  OutMsg.value1 = 0;
  OutMsg.value2 = 0;
  OutMsg.value3 = 0;
  OutMsg.sender = CONSOLE_Q;
  //Request for the measurements (Hum, Temp, pH) to the controllers
  sendMessage ( &(queue [CONTROLLER1_Q]), OutMsg );
  sendMessage ( &(queue [CONTROLLER2_Q]), OutMsg );
  for ( ; ; ){
    state = state_next;
    InMsg = receiveMessage ( &(queue [CONSOLE_Q]) );
    switch (state) {
      case WaitingController:
        switch (InMsg.signal) {
          case sSendData:
            printf("Esperando las medidas por parte de los sensores\n");
            sleep(5);
            //Mandamos la senal sGetData
            OutMsg.signal = (int) sGetData;
            OutMsg.value1 = InMsg.value1;
            OutMsg.value2 = InMsg.value2;
            OutMsg.value3 = InMsg.value3;
            OutMsg.sender = CONSOLE_Q;
            SenderQ = InMsg.sender;
            sendMessage ( &(queue [SenderQ]), OutMsg );
            //sendMessage ( &(queue [CONTROLLER1_Q]), OutMsg );
            //sendMessage ( &(queue [CONTROLLER2_Q]), OutMsg );
            state_next = WaitingController;
            break;
        }
      default:
        break;
    }
  }
  return ( NULL );
}

static void *pController ( void *arg_ptr ){
  CONTROLLER_STATES   state, state_next;
  msg_t               InMsg, OutMsg;
  int                 queueNo, *data_ptr;
  int                 whoami;
  int                 sensorsArray[3];
  data_ptr = (int *) arg_ptr;
  queueNo = whoami = *data_ptr;

  printf ( "\t\t\t\t Controller with ID %d is free...\n", whoami );
  fflush ( stdout );
  state_next = IdleController;

  for ( ; ; ){
    state = state_next;
    InMsg = receiveMessage ( &(queue [queueNo]) );
    switch (state) {
      case IdleController:
        switch (InMsg.signal) {
          case sGetData:
            OutMsg.signal = (int) sGetHum;
            OutMsg.value1 = 0;
            OutMsg.value2 = 0;
            OutMsg.value3 = 0;
            OutMsg.sender = whoami;
            if (whoami == 1){
              sendMessage ( &(queue [SEN_HUMIDITY1_Q]), OutMsg ); /* send message to pSenHumidity1 process */
              sendMessage ( &(queue [SEN_HUMIDITY2_Q]), OutMsg ); /* send message to pSenHumidity2 process */
            }
            else if (whoami == 2){
              sendMessage ( &(queue [SEN_HUMIDITY3_Q]), OutMsg ); /* send message to pSenHumidity3 process */
              sendMessage ( &(queue [SEN_HUMIDITY4_Q]), OutMsg ); /* send message to pSenHumidity4 process */
            }
            state_next = WaitingHumidity;
            break;
          default:
            break;
        }
      case WaitingHumidity:
        switch (InMsg.signal) {
          case sSendHum:
            sensorsArray[0] = InMsg.value1; //En el arreglo sensors array que es el que se le manda a pConsole despues, se marca la humedad obtenida
            OutMsg.signal = (int) sGetTemp;
            OutMsg.value1 = InMsg.value1;
            OutMsg.value2 = InMsg.value2;
            OutMsg.value3 = InMsg.value3;
            OutMsg.sender = whoami;
            if (whoami == 1){
              sendMessage ( &(queue [SEN_TEMPERATUTE1_Q]), OutMsg ); /* send message to pSenTemperature1 process */
              sendMessage ( &(queue [SEN_TEMPERATUTE2_Q]), OutMsg ); /* send message to pSenTemperature2 process */
            }
            else if (whoami == 2){
              sendMessage ( &(queue [SEN_TEMPERATUTE3_Q]), OutMsg ); /* send message to pSenTemperature3 process */
              sendMessage ( &(queue [SEN_TEMPERATUTE4_Q]), OutMsg ); /* send message to pSenTemperature4 process */
            }
            state_next = WaitingTemperature;
            break;
          default:
            break;
        }
      case WaitingTemperature:
        switch (InMsg.signal) {
          case sSendTemp:
            sensorsArray[1] = InMsg.value2; //En el arreglo sensors array que es el que se le manda a pConsole despues, se marca la temperatura obtenida
            OutMsg.signal = (int) sGetPH;
            OutMsg.value1 = InMsg.value1;
            OutMsg.value2 = InMsg.value2;
            OutMsg.value3 = InMsg.value3;
            OutMsg.sender = whoami;
            if (whoami == 1){
              sendMessage ( &(queue [SEN_PH1_Q]), OutMsg ); /* send message to pSenPH1 process */
              sendMessage ( &(queue [SEN_PH2_Q]), OutMsg ); /* send message to pSenPH2 process */
            }
            else if (whoami == 2){
              sendMessage ( &(queue [SEN_PH3_Q]), OutMsg ); /* send message to pSenPH3 process */
              sendMessage ( &(queue [SEN_PH4_Q]), OutMsg ); /* send message to pSenPH4 process */
            }
            state_next = WaitingPH;
            break;
          default:
            break;
        }
      case WaitingPH:
        switch (InMsg.signal) {
          case sSendPH:
            sensorsArray[2] = InMsg.value3; //En el arreglo sensors array que es el que se le manda a pConsole despues, se marca el PH obtenido
            printf("sSenData from pController %d: Humidity=%d Temperature=%d pH=%d \n", whoami, InMsg.value1, InMsg.value2, InMsg.value3);
            fflush ( stdout );
            OutMsg.signal = (int) sSendData;
            OutMsg.value1 = sensorsArray[0];
            OutMsg.value2 = sensorsArray[1];
            OutMsg.value3 = sensorsArray[2];
            OutMsg.sender = whoami;
            sendMessage ( &(queue [CONSOLE_Q]), OutMsg ); /* send message to pConsole process */
            state_next = IdleController;
            break;
          default:
            break;
        }
      default:
        break;
    }
  }
  return ( NULL );
}

static void *pSenHumidity ( void *arg_ptr ){
  SENSORH_STATES  state, state_next;
  msg_t           InMsg, OutMsg;
  int             queueNo, *data_ptr;
  int             whoami, SenderQ;
  int             lower = 0;
  int             upper = 20;
  int             hValue;
  data_ptr = (int *) arg_ptr;
  queueNo = whoami = *data_ptr;

  printf ( "\t\t\t\t Humidity Sensor with ID %d is free...\n", whoami );
  fflush ( stdout );
  state_next = IdleHumidity;
  for ( ; ; ){
    state = state_next;
    InMsg = receiveMessage ( &(queue [queueNo]) );
    switch (state) {
      case IdleHumidity:
        switch (InMsg.signal) {
          case sGetHum:
            OutMsg.signal = (int) sSendHum;
            hValue = (rand() % (upper - lower + 1)) + lower;
            OutMsg.value1 = hValue;
            OutMsg.value2 = InMsg.value2;
            OutMsg.value3 = InMsg.value3;
            OutMsg.sender = whoami;
            SenderQ = InMsg.sender;
            sendMessage ( &(queue [SenderQ]), OutMsg ); /* send message to pController process */
            state_next = IdleHumidity;
            break;
          default:
            break;
        }
      default:
        break;
    }
  }
  return ( NULL );
}

static void *pSenTemperature ( void *arg_ptr ){
  SENSORT_STATES  state, state_next;
  msg_t           InMsg, OutMsg;
  int             queueNo, *data_ptr;
  int             whoami, SenderQ;
  int             lower = -10;
  int             upper = 20;
  int             tValue;
  data_ptr = (int *) arg_ptr;
  queueNo = whoami = *data_ptr;

  printf ( "\t\t\t\t Temperature Sensor with ID %d is free...\n", whoami );
  fflush ( stdout );
  state_next = IdleTemperature;
  for ( ; ; ){
    state = state_next;
    InMsg = receiveMessage ( &(queue [queueNo]) );
    switch (state) {
      case IdleTemperature:
        switch (InMsg.signal) {
          case sGetTemp:
            OutMsg.signal = (int) sSendTemp;
            OutMsg.value1 = InMsg.value1;
            tValue = (rand() % (upper - lower + 1)) + lower;
            OutMsg.value2 = tValue;
            OutMsg.value3 = InMsg.value3;
            OutMsg.sender = whoami;
            SenderQ = InMsg.sender;
            sendMessage ( &(queue [SenderQ]), OutMsg ); /* send message to pConstroller process */
            state_next = IdleTemperature;
            break;
          default:
            break;
        }
      default:
        break;
    }
  }
  return ( NULL );
}

static void *pSenPH ( void *arg_ptr ){
  SENSORPH_STATES state, state_next;
  msg_t           InMsg, OutMsg;
  int             queueNo, *data_ptr;
  int             whoami, SenderQ;
  int             lower = 0;
  int             upper = 14;
  int             phValue;
  data_ptr = (int *) arg_ptr;
  queueNo = whoami = *data_ptr;

  printf ( "\t\t\t\t pH Sensor with ID %d is free...\n", whoami );
  fflush ( stdout );
  state_next = IdlePH;
  for ( ; ; ){
    state = state_next;
    InMsg = receiveMessage ( &(queue [queueNo]) );
    switch (state) {
      case IdlePH:
        switch (InMsg.signal) {
          case sGetPH:
            OutMsg.signal = (int) sSendPH;
            OutMsg.value1 = InMsg.value1;
            OutMsg.value2 = InMsg.value2;
            phValue = (rand() % (upper - lower + 1)) + lower;
            OutMsg.value3 = phValue;
            OutMsg.sender = whoami;
            SenderQ = InMsg.sender;
            sendMessage ( &(queue [SenderQ]), OutMsg ); /* send message to pConstroller process */
            state_next = IdlePH;
            break;
          default:
            break;
        }
      default:
        break;
    }
  }
  return ( NULL );
}
