
///////////////////////////////////////////////////////////////////////////////
/// Macros
///////////////////////////////////////////////////////////////////////////////

// Pinmap

#define SYS_POWER   P2_4            // Keep high to maintain system power
#define KILL_STAT   P2_3            // Input signal indicating thruster kill status
#define LEAK_SEN    P1_0            // Leak sensor input

#define RED_LED     P1_3            // Red status LED
#define GRN_LED     P1_4            // Green status LED
#define BLU_LED     P2_5            // Blue status LED
#define YLW_LED     P2_6            // Yellow status LED
#define WHT_LED     P2_7            // White status LED

#define SYS_LED     GRN_LED         // System power LED
#define TEMP_LED    RED_LED         // Overtemperature LED
#define LEAK_LED    BLU_LED         // Leak detected LED
#define THRU_LED    YLW_LED         // Thruster status LED

// Note: P1_5, P1_6, P1_7 reserved for SPI
// Note: P2_0, P2_1, P2_2 reserved for I2C
// Note: P1_1, P1_2 reserved for UART


// Other parameters

#define LEAK_DETECT_TIME    250     // How long a leak must be detected for (in ms) to trigger shutdown
#define LEAK_SHUTDOWN_TIME  10000   // How long to delay shutdown after leak detected


///////////////////////////////////////////////////////////////////////////////
/// Globals
///////////////////////////////////////////////////////////////////////////////

unsigned long leakStart;      // When leak first detected
bool prevLeak;                // Was leak detected last iteration

unsigned long shutdownTime;   // Time to shutdown at (or after) millis
bool shutdownEnable;          // Set true to shutdown at given time


///////////////////////////////////////////////////////////////////////////////
/// Functions
///////////////////////////////////////////////////////////////////////////////

void setup() {
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
  digitalWrite(WHT_LED, LOW);         // Currently unused white LED off by default

  // Leak detection initial values
  leakStart = 0;
  prevLeak = LOW;

  // Shutdown system initial values
  shutdownTime = 0;
  shutdownEnable = false;
}


void loop() {
  unsigned long now = millis();
  
  // Update thruster kill status LED
  // Thruster kill status signal low = thrusters armed (relay coil- grounded)
  // LED should be on when thrusters armed
  digitalWrite(THRU_LED, !digitalRead(KILL_STAT));


  // TODO: Read AHT10 temp sensor. If temp too high, turn on overtemp LED


  // Check for leak detection
  int leak = digitalRead(LEAK_SEN);
  if (leak && !prevLeak) {
    // Leak just now detected
    leakStart = now;
  }else if(leak && prevLeak){
    // Leak has been detected. If long enough, trigger shutdown (unless it has already been triggered)
    if(!shutdownEnable && now - leakStart > LEAK_DETECT_TIME){
      shutdownTime = now + LEAK_SHUTDOWN_TIME;
      shutdownEnable = true;
    }
  }
  prevLeak = leak;

  // Slow down execution. Better to use sleep mode, but...
  // No timings less than millisecond level are used so delay 1ms min between iterations
  delay(1);
  
}
