#define LEAK_DETECT_TIME    250                   // How long a leak must be detected for (in ms) to trigger shutdown
#define LEAK_SHUTDOWN_TIME  10000                 // How long to delay shutdown after leak detected (in ms)
#define UVOLT_DETECT_TIME     8000                // How long undervoltage must be for (in ms) to trigger shutdown
#define UVOLT_SHUTDOWN_TIME   10000               // How long to delay shutdown after undervoltage detected (in ms)
#define TOO_LOW_VOLTAGE       13.5f
#define PC_BAUD             57600   // Baud rate for comm with the computer


// AHT10 error codes
#define AHT10_ERR_NONE      0
#define AHT10_ERR_INIT      -1

// Other parameters




typedef enum bool {
    true,
    false
} bool;

enum SHUTDOWN_REASONS {
    NOT_SHUTDOWN,
    SHUTDOWN_NONE,
    SHUTDOWN_LEAK,
    SHUTDOWN_VOLTAGE
};


//
// Created by Ben Caunt on 9/13/23.
//
typedef struct sensor_data {
    float sys_voltage;
    bool leak_detected_flag;
    bool under_volt_flag;
} sensor_data_t;


sensor_data_t CreateSensorData(float sys_voltage, bool leak_detected_flag, bool under_volt_flag) {
    return (sensor_data_t) {sys_voltage,
                            leak_detected,
                            under_volt};
}


bool check_for_leak() {
    static unsigned long leakStart = 0;               // When leak first detected
    static bool prevLeak = LOW;                       // Was leak detected last iteration
    int leak = digitalRead(LEAK_SEN) == HIGH;
    if (leak && !prevLeak) {
        // Leak just now detected
        leakStart = millis();
    } else if (leak && prevLeak) {
        // Leak has been detected. If long enough, trigger shutdown (unless it has already been triggered)
        if (!shutdownEnable && millis() - leakStart > LEAK_DETECT_TIME) {
            // -----------------------------------------------------------------------
            // Comment to disable shutdown due to leak
            shutdownTime = millis() + LEAK_SHUTDOWN_TIME;
            shutdownEnable = true;
            shutdownCause = SHUTDOWN_LEAK;
            notify_pc_shutdown();
            // -----------------------------------------------------------------------
            leak_detected = true;
        }
    }
    prevLeak = leak;

    if (leak) {
        return true;
    }
    return false;

}

void task_read_sensors() {

    // ----------------------------------------------------------------------------------------------
    // Leak sensor
    // ----------------------------------------------------------------------------------------------



    // ----------------------------------------------------------------------------------------------

    // ----------------------------------------------------------------------------------------------
    // System Voltage (using voltage divider)
    // ----------------------------------------------------------------------------------------------

    static unsigned long undervoltStart = 0;          // When undervoltage detected
    static bool prevUndervolt = false;                // Was undervoltage on last iteration

    // Voltage divider: R1 = 1M, R2 = 220k
    // Vin = (Vout * (R1+R2)) / R2
    // Vout = (ADC_READ / 1024) * 3.3V     (ADC is 10-bit and VCC referenced)
    const float scale_factor = ((1e6f + 220e3f) / 220e3f);  // (R1+R2)/R2
    uint16_t read_val = analogRead(VSYS_DIV);
    sys_voltage = (((read_val / 1024.0f) * 3.3f) * scale_factor);

    // Only check for undervoltage if thrusters are killed. This ensures voltage drops from thrusters
    // will not trigger a shutdown.
    bool net_arm = (digitalRead(KILL_STAT) == LOW);
    bool undervolt = (sys_voltage < TOO_LOW_VOLTAGE) && !net_arm;
    if (undervolt && !prevUndervolt) {
        undervoltStart = millis();
    }
    prevUndervolt = undervolt;
    // ----------------------------------------------------------------------------------------------
}