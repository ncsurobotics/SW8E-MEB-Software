
#include "pinmap.h"
#include "conversions.hpp"
#include "comm.hpp"
#include "aht10.hpp"
#include <Wire.h>


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Macros
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Auto shutdown causes (comment out to disable shutdown for a specific cause)
#define SHUTDOWN_NONE       0
#define SHUTDOWN_LEAK       1
// #define SHUTDOWN_TEMP    2

// AHT10 error codes
#define AHT10_ERR_NONE      0
#define AHT10_ERR_INIT      -1

// Other parameters
#define PC_BAUD             57600   // Baud rate for comm with the computer

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Globals
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Comm object
Communication comm;

// Sensor objects
AHT10 aht10;
int8_t aht10Ec = 0;

// System shutdown control
unsigned int shutdownTime = 0;
bool shutdownEnable = false;
unsigned int shutdownCause = 0;

// Sensor data
float temp = 0.0f;
float humid = 0.0f;
bool leak_detected = false;
bool over_temp_detected = false;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Task functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void task_shutdown(){
  #define SDOWN_IND_BLINK     250     // Shutdown indicator blink rate (period in ms)

  static unsigned long shutdownLedBlink = millis();

  // Shutdown if one is scheduled
  if(shutdownEnable){
    if(millis() >= shutdownTime) {
      digitalWrite(SYS_POWER, LOW); // Actually shutdown
      while(1);
    }
    if(millis() >= shutdownLedBlink){
      // Blink LEDs before shutdown
      int newState = !digitalRead(EXTRA_LED);
      shutdownLedBlink += SDOWN_IND_BLINK;
      digitalWrite(SYS_LED, newState);
      digitalWrite(TEMP_LED, newState);
      digitalWrite(LEAK_LED, newState);
      digitalWrite(THRU_LED, newState);
      digitalWrite(EXTRA_LED, newState);
    }
  }
}

void task_update_leds(){
  // When shutdown enabled the LEDs will blink
  // This is handled in the shutdown task
  if(shutdownEnable)
    return;

  // Update LEDs
  digitalWrite(THRU_LED, !digitalRead(KILL_STAT));
  if(leak_detected)
    digitalWrite(LEAK_LED, HIGH);
  if(over_temp_detected)
    digitalWrite(TEMP_LED, HIGH);
}

void task_read_sensors(){
  // ----------------------------------------------------------------------------------------------
  // Read AHT10 temp sensor
  // ----------------------------------------------------------------------------------------------
  #define TEMP_SHUTDOWN_TIME  10000                 // How long to delay shutdown after overtemp detected (in ms)
  #define OVER_TEMP           40                    // Over temp threshold degrees C

  if(aht10Ec == AHT10_ERR_NONE && aht10.read(&temp, &humid)){
    #if defined(SHUTDOWN_TEMP)
      if(!shutdownEnable && temp > OVER_TEMP){
        shutdownTime = millis() + TEMP_SHUTDOWN_TIME;
        shutdownEnable = true;
        shutdownCause = SHUTDOWN_TEMP;
      }    
    #else
      over_temp_detected = true;
    #endif
  }
  // ----------------------------------------------------------------------------------------------


  // ----------------------------------------------------------------------------------------------
  // Leak sensor
  // ----------------------------------------------------------------------------------------------
  #define LEAK_DETECT_TIME    250                   // How long a leak must be detected for (in ms) to trigger shutdown
  #define LEAK_SHUTDOWN_TIME  10000                 // How long to delay shutdown after leak detected (in ms)

  static unsigned long leakStart = 0;               // When leak first detected
  static bool prevLeak = LOW;                       // Was leak detected last iteration

  int leak = digitalRead(LEAK_SEN) == HIGH;
  if (leak && !prevLeak) {
    // Leak just now detected
    leakStart = millis();
  }else if(leak && prevLeak){
    #ifdef SHUTDOWN_LEAK
      // Leak has been detected. If long enough, trigger shutdown (unless it has already been triggered)
      if(!shutdownEnable && millis() - leakStart > LEAK_DETECT_TIME){
        shutdownTime = millis() + LEAK_SHUTDOWN_TIME;
        shutdownEnable = true;
        shutdownCause = SHUTDOWN_LEAK;
      }
    #else
      leak_detected = true;
    #endif
  }
  prevLeak = leak;
  // ----------------------------------------------------------------------------------------------
}

void task_send_sensor_data(){
  static uint8_t buf[MAX_MSG_LEN];

  // Send AHT10 data
  buf[0] = 'T';
  buf[1] = 'E';
  buf[2] = 'M';
  buf[3] = 'P';
  Conversions::convertFloatToData(temp, &buf[4], true);
  Conversions::convertFloatToData(humid, &buf[8], true);
  comm.sendMessage(buf, 12);

  // Send leak status
  buf[0] = 'L';
  buf[1] = 'E';
  buf[2] = 'A';
  buf[3] = 'K';
  buf[4] = leak_detected;
  comm.sendMessage(buf, 5);

  // Send arm / kill status (NET arm / kill)
  buf[0] = 'T';
  buf[1] = 'A';
  buf[2] = 'R';
  buf[3] = 'M';
  buf[4] = !digitalRead(KILL_STAT);
  comm.sendMessage(buf, 5);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// System setup and main loop
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Task{
public:
  Task(void (*target_func)(void), unsigned long period, unsigned long start_delay) : 
      target_func(target_func),
      period(period),
      last_run(-start_delay){}

  void (*target_func)(void);
  unsigned long period;
  unsigned long last_run;
};

#define NTASKS    4
Task tasks[NTASKS] = {
  //      Target function               period (ms)           start delay(ms)
  Task(   &task_shutdown,               100,                  30               ),
  Task(   &task_update_leds,            100,                  25               ),
  Task(   &task_read_sensors,           50,                   0                ),
  Task(   &task_send_sensor_data,       50,                   10               )
};

void setup(){
  // Run before communication
  Conversions::checkBigEndian();

  // Setup communication with jetson
  comm.begin(Serial, PC_BAUD);
  
  // Setup GPIO pins
  pinMode(SYS_POWER, OUTPUT);
  pinMode(KILL_STAT, INPUT);
  pinMode(LEAK_SEN, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GRN_LED, OUTPUT);
  pinMode(BLU_LED, OUTPUT);
  pinMode(YLW_LED, OUTPUT);
  pinMode(WHT_LED, OUTPUT);

  // Initial pin states
  digitalWrite(SYS_POWER, HIGH);      // Keep system on once code starts (no need to hold on button)
  digitalWrite(SYS_LED, HIGH);        // Turn system power LED on once system boots
  digitalWrite(TEMP_LED, LOW);        // Overtemp LED off by default
  digitalWrite(LEAK_LED, LOW);        // Leak LED off by default
  digitalWrite(THRU_LED, LOW);        // Thruster kill status LED off by default 
  digitalWrite(EXTRA_LED, LOW);       // Currently unused white LED off by default

  // Setup I2C in master mode
  Wire.begin();

  // Start AHT10
  if(!aht10.begin()){
    aht10Ec = AHT10_ERR_INIT;
  }
}

void loop(){
  static unsigned curr_task = 0;
  
  // Check if the current task is ready to run. If so, run it
  if(millis() - tasks[curr_task].last_run >= tasks[curr_task].period){
    tasks[curr_task].last_run = millis();
    tasks[curr_task].target_func();
  }

  // Check the next task next iteraion
  curr_task++;  
  if(curr_task > NTASKS)
    curr_task = 0;  
}
