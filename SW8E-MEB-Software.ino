
#include "pinmap.h"
#include "conversions.hpp"
#include "comm.hpp"
#include "aht10.hpp"
#include "expboard.hpp"
#include <Wire.h>
#include "LEDs.h"



// Don't try to enable LXFT1.
// This oscillator isn't connected on these boards
// and on the FR2433 cuases thrusters to arm during the 2 seconds
// energia tries to do so.
extern volatile uint16_t vlo_freq;
void enableXtal(void)
{
#if (!defined(__MSP430FR2XX_4XX_FAMILY__) && (defined(__MSP430_HAS_CS__) || defined(__MSP430_HAS_CS_A__))) 
    /* ACLK = VLO = ~ 12 KHz */
    vlo_freq = 8000;
    /* Source ACLK from VLO */
    CSCTL2 |= SELA__VLOCLK;
  }
#endif
#if (defined(__MSP430FR2XX_4XX_FAMILY__) && (defined(__MSP430_HAS_CS__) || defined(__MSP430_HAS_CS_A__)))
  /* ACLK = REFO = ~ 32 KHz */
  vlo_freq = 32768;
  /* Source ACLK from REFO */
  CSCTL4 |= SELA__REFOCLK;
  CSCTL3 |= SELREF__REFOCLK;


  /* Clear the Oscillator fault interrupt flag */
  CSCTL7 &= ~(DCOFFG|XT1OFFG|FLLULIFG);
  SFRIFG1 &= ~OFIFG;
#endif

#ifdef __MSP430_HAS_BC2__
  /* ACLK = VLO = ~ 12 KHz */
  vlo_freq = 12000;
  /* No XTAL present. Default P2.6/7 to GPIO */
  P2SEL &= ~(BIT6|BIT7);
  /* Source ACLK from VLO */
  BCSCTL3 |= LFXT1S_2;
#endif

#if defined(__MSP430_HAS_UCS__)
  UCSCTL6 &= ~(XT1DRIVE_3);
#endif
}


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

<<<<<<< HEAD
ExpansionBoard msb(MSB_ADDR);
=======
// LED Strip
LED_Strip ledStrip;
>>>>>>> 53c4b5b (Replaced old LEDs with neopixel code)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Task functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void task_shutdown(){   
  // Serial.println("task_shutdown()"); Serial.flush();

  // Shutdown if one is scheduled
  if(shutdownEnable){
    if(millis() >= shutdownTime) {
      digitalWrite(SYS_POWER, LOW); // Actually shutdown
      while(1);
    }
  }
}

void task_update_leds(){
  #define SDOWN_IND_BLINK     250     // Shutdown indicator blink rate (period in ms)
  static unsigned long shutdownLedBlink = millis();
  static bool shutdownBlinkOn = false;

  // When shutdown enabled the LEDs will blink
  // This is handled in the shutdown task
  if(!shutdownEnable){
    // Update LEDs (normal meanings)
    // KILL_STAT line is low when thrusters have power (are net armed)
    auto net_arm = (digitalRead(KILL_STAT) == LOW);
    
    if(net_arm)
      ledStrip.set_One(1, YELLOW);
    else
      ledStrip.set_One(1, BLACK);
    if(leak_detected)
      ledStrip.set_One(7, RED);
    else
      ledStrip.set_One(7, BLACK);
  }else{
    // Blink LEDs indicating shutdown is going to occur
    if(millis() >= shutdownLedBlink){
      // Toggle LEDs
      if(shutdownBlinkOn){
        // Set all to red
        ledStrip.set_All(RED);
      }else{
        // Set all to black
        ledStrip.set_All(BLACK);
      }
      shutdownBlinkOn = !shutdownBlinkOn;

      // Toggle again after SDOWN_IND_BLINK milliseconds
      shutdownLedBlink += SDOWN_IND_BLINK;
    }
  }
  ledStrip.update_LEDs();
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
  return;
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
  if(!comm.readMessage(msg, msg_len, msg_id)){
    // No message
    return;
  }

  // Handle message. Use MSG_STARTS_WITH and MSG_EQUALS for comparisons
  const uint8_t CMD_MSB_PREFIX[] = {'M', 'S', 'B'};

  if(data_startswith(msg, msg_len, CMD_MSB_PREFIX, sizeof(CMD_MSB_PREFIX))){
    // 'M', 'S', 'B', [data]
    // [data] is arbitrary sized data to be sent to mech systems board
    msb.transfer(&msg[3], msg_len - 3, NULL, 0);
    comm.acknowledge(msg_id, ACK_ERR_NONE, NULL, 0);
  }else{
    comm.acknowledge(msg_id, ACK_ERR_UNKNOWN_MSG, NULL, 0);
  }
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
  // Conversions::checkBigEndian();

  // Setup communication with jetson
  // comm.begin(Serial, PC_BAUD);
  Serial.begin(57600);
  Serial.println("STARTED!!!");

  return;

  // Setup GPIO pins
  pinMode(SYS_POWER, OUTPUT);
  pinMode(KILL_STAT, INPUT);
  pinMode(LEAK_SEN, INPUT);


  // Initial pin states
  digitalWrite(SYS_POWER, HIGH);      // Keep system on once code starts (no need to hold on button)

  // Setup I2C in master mode
  Wire.begin();
  Wire.setClock(100000);              // For some reason, FR2433 defaults to 280kHz clock. Don't know why.

  // Start AHT10
  // if(!aht10.begin()){
    aht10Ec = AHT10_ERR_INIT;
<<<<<<< HEAD
  // }
=======
  }

  // Configure SPI
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);              // Denotes the transmission precedence at Most Significant Bit First
  SPI.setDataMode(SPI_MODE1);             // Sets the MSP430 into SPI Mode 1, which means the system is Idle when the CLK is low, and updates on the rising CLK Edge
#if F_CPU==8000000L
  SPI.setClockDivider(SPI_CLOCK_DIV2);    //Sets clock for SPI communication at 8/2=4Mhz
#elif F_CPU==16000000L
  SPI.setClockDivider(SPI_CLOCK_DIV4);    // 16MHz / 4 = 4MHz
#else
  #error Unknown clock division for this frequency!
#endif
  delay(10);
  
  // reset strip
  ledStrip.reset_strip();                 // Resets the strip to all BLACK
  delay(500);

  // Set initial LED state
  ledStrip.set_One(0, GREEN);
  ledStrip.update_LEDs();
>>>>>>> 53c4b5b (Replaced old LEDs with neopixel code)
}

void loop(){
  return;
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
