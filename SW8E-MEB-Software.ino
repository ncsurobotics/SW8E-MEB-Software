
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

// Communication stuff
Communication comm;
uint8_t msg[MAX_MSG_LEN];
unsigned int msg_len;
uint16_t msg_id;

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
  // Serial.println("task_shutdown()"); Serial.flush();


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
  // Serial.println("task_update_leds()"); Serial.flush();


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
  // Serial.println("task_read_sensors()"); Serial.flush();


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
  // Serial.println("task_send_sensor_data()"); Serial.flush();

  // Send AHT10 data
  msg[0] = 'T';
  msg[1] = 'E';
  msg[2] = 'M';
  msg[3] = 'P';
  Conversions::convertFloatToData(temp, &msg[4], true);
  Conversions::convertFloatToData(humid, &msg[8], true);
  comm.sendMessage(msg, 12);

  // Send leak status
  msg[0] = 'L';
  msg[1] = 'E';
  msg[2] = 'A';
  msg[3] = 'K';
  msg[4] = leak_detected;
  comm.sendMessage(msg, 5);

  // Send arm / kill status (NET arm / kill)
  msg[0] = 'T';
  msg[1] = 'A';
  msg[2] = 'R';
  msg[3] = 'M';
  msg[4] = !digitalRead(KILL_STAT);
  comm.sendMessage(msg, 5);
}

void task_receive_pc(){
  // Serial.println("task_receive_pc()"); Serial.flush();

  // Helper macros for comparisons
  #define MSG_STARTS_WITH(x)      data_startswith(msg, msg_len, (x), sizeof((x)))
  #define MSG_EQUALS(x)           data_matches(msg, msg_len, (x), sizeof(x))
  
  if(!comm.readMessage(msg, msg_len, msg_id)){
    // No message
    return;
  }

  // Handle message. Use MSG_STARTS_WITH and MSG_EQUALS for comparisons
  /*
  EXAMPLE MESSAGE HANDLING CODE

  if(MSG_EQUALS(((uint8_t[]){'M', 'S', 'G', 'A'}))){
    do_something();
    comm.acknowledge(msg_id, ACK_ERR_NONE, NULL, 0);
  }else if(MSG_STARTS_WITH(((uint8_t[]){'M', 'S', 'G', 'B'}))){
    // Validate length (right number of arguments)
    if(msg_len != 8){
      comm.acknowledge(msg_id, ACK_ERR_INVALID_ARGS, NULL, 0);
      return;
    }
    
    // Make sure this command's action is allowed (ie resources it requires are avaiable)
    if(!required_sensor_available()){
      comm.acknowledge(msg_id, ACK_ERR_INVALID_CMDS, NULL, 0);
      return;
    }
    
    // Get arguments
    float arg = Conversions::convertDataToFloat(&msg[4], true);
    
    // Perform the requested action and acknowledge with no error
    do_something_with_sensor(arg);
    comm.acknowledge(msg_id, ACK_ERR_NONE, NULL, 0);
  }
  */
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

#define NTASKS    5
Task tasks[NTASKS] = {
  //      Target function               period (ms)           start delay(ms)
  Task(   &task_shutdown,               100,                  30               ),
  Task(   &task_update_leds,            100,                  25               ),
  Task(   &task_read_sensors,           50,                   0                ),
  Task(   &task_send_sensor_data,       50,                   10               ),
  Task(   &task_receive_pc,             20,                   5                )
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
  if(curr_task == NTASKS)
    curr_task = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Helper functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Check if two byte arrays are identical
 * @param a First byte array
 * @param len_a Length of first array
 * @param b Second byte array
 * @param len_b Length of second array
 * @return true If arrays match
 * @return false If arrays do not match
 */
static bool data_matches(const uint8_t *a, uint32_t len_a, const uint8_t *b, uint32_t len_b){
    if(len_a != len_b)
        return false;
    for(uint32_t i = 0; i < len_a; ++i){
        if(a[i] != b[i])
            return false;
    }
    return true;
}

/**
 * Check if one array starts with the data in another array
 * @param a The array to search in ("full" data)
 * @param len_a Length of array a
 * @param b The array to search for ("sub" / "prefix" data)
 * @param len_b Length of array b
 * @return true If array a starts with array b
 * @return false If array a does not start with array b
 */
static bool data_startswith(const uint8_t *a, uint32_t len_a, const uint8_t *b, uint32_t len_b){
    if(len_a < len_b)
        return false;
    for(uint32_t i = 0; i < len_b; ++i){
        if(a[i] != b[i])
            return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
