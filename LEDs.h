// LED Strip update and writing code header
// COLORS = RRGGBB
  // WHITE = 0xFFFFFF
  // RED = 0xFF0000
  // GREEN = 0x00FF00
  // BLUE = 0x0000FF
  // YELLOW = 0xF08000
  // MAGENTA = 0xFF005E
  // BLACK = 0x000000

#include <stdint.h>
#include <SPI.h>

// Required defines for the code
#define N_LEDS (4) // The number of LEDS in the strip

// COLORS = RRGGBB
#define WHITE (0xFFFFFF)
#define RED (0xFF0000)
#define GREEN (0x00FF00)
#define BLUE (0x0000FF)
#define YELLOW (0xF08000)
#define MAGENTA (0xFF005E)
#define BLACK (0x000000) // Turns the LED OFF


// LED Strip class that contains the LED value arrays as parameters and functions for writing and updating the LED Strip
class LED_Strip{
private:
  // LED Value Arrays
  uint8_t reds[N_LEDS]; // Holds the red value data for every LED in the strip
  uint8_t greens[N_LEDS]; // Holds the green value data for every LED in the strip
  uint8_t blues[N_LEDS]; // Holds the blue value data for every LED in the strip
  
public:
  // LED Write and Update Function Declarations
  
  // Configures the RGB values for a specified LED based on a passed-in 24-bit value in RGB format
  void set_One(int led, uint32_t value);
  
  // Configures the RGB Passes in an array of all the LED values and updates all the LEDs in the array at once
  void set_All(uint32_t value);
  
  // Transmits the signals to the LEDs necessary for them to recognize the 1s and 0s as 1s and 0s
  void write_LED(uint8_t r, uint8_t g, uint8_t b);
  
  // Sends the data required to write to the LEDs
  void update_LEDs();
  
  // Turns off all the LEDs on the strip
  void reset_strip();
};
