/*******************************************************************************
*
*  Habra 2 camas para monitorear, cada cama tiene su pController y 2 sensores de cada tipo
*  El controlador 1 maneja los sensores con id 1 y 2. El controlador 2 maneja los sensores con id 3 y 4
*
*******************************************************************************/

#define BUFSIZE     8     /* number of slots in queues */
#define NUM_QUEUES  15     /* number of queues */
#define CONSOLE_Q             0     /* queue 0: pConsole process */
#define CONTROLLER1_Q         1     /* queue 1: pController1 process */
#define CONTROLLER2_Q         2     /* queue 2: pController2 process */
#define SEN_HUMIDITY1_Q       3     /* queue 3: pSenHumidity1 process */
#define SEN_TEMPERATUTE1_Q    4     /* queue 4: pSenTemperature1 process */
#define SEN_PH1_Q             5     /* queue 5: pSenPH1 process */
#define SEN_HUMIDITY2_Q       6     /* queue 6: pSenHumidity2 process */
#define SEN_TEMPERATUTE2_Q    7     /* queue 7: pSenTemperature2 process */
#define SEN_PH2_Q             8     /* queue 8: pSenPH2 process */
#define SEN_HUMIDITY3_Q       9     /* queue 9: pSenHumidity3 process */
#define SEN_TEMPERATUTE3_Q    10     /* queue 10: pSenTemperature3 process */
#define SEN_PH3_Q             11     /* queue 11: pSenPH3 process */
#define SEN_HUMIDITY4_Q       12     /* queue 12: pSenHumidity4 process */
#define SEN_TEMPERATUTE4_Q    13     /* queue 13: pSenTemperature4 process */
#define SEN_PH4_Q             14     /* queue 14: pSenPH4 process */

/***( message structure )******************************************************/

typedef struct{
  int   signal;
  int   value1;
  int   value2;
  int   value3;
  int   sender;
} msg_t;

/***( signals )****************************************************************/

typedef enum{
  sGetData
} CONSOLE; /* Signals of pConsole*/

typedef enum{
  sSendData,
  sGetHum,
  sGetTemp,
  sGetPH
} CONTROLLER; /* Signals of pController*/

typedef enum{
  sSendHum,
  sSendTemp,
  sSendPH
} SENSORS; /* Signals of sensors (pSenHumidity, pSenTemperature, pSenPH)*/

/***( States )************************************************************/

typedef enum{
  WaitingController
} CONSOLE_STATES; /* States of pConsole*/

typedef enum{
  IdleController,
  WaitingHumidity,
  WaitingTemperature,
  WaitingPH
}CONTROLLER_STATES; /* States of pController */

typedef enum{
  IdleHumidity
}SENSORH_STATES; /* States of pSenHumidity */

typedef enum{
  IdleTemperature
}SENSORT_STATES; /* States of pSenTemperature */

typedef enum{
  IdlePH
}SENSORPH_STATES; /* States of pSenPH */
