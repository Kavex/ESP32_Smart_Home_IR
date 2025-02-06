#include <IRremoteESP8266.h>
#include <IRsend.h>

// Define the IR code using a macro.
#define IR_CODE 0x20DF10EF

// Define the GPIO pin for the IR LED. Change this if needed.
const uint16_t kIrLed = 4;

// Create an IRsend object.
IRsend irsend(kIrLed);

void setup() {
  // Initialize serial communication for debugging.
  Serial.begin(115200);
  delay(200);
  
  // Initialize the IR transmitter.
  irsend.begin();
  Serial.println("IR transmitter initialized");
}

void loop() {
  // Print the code being sent.
  Serial.print("Sending IR code: 0x");
  Serial.println(IR_CODE, HEX);

  // Send the code using the NEC protocol with 32 bits.
  irsend.sendNEC(IR_CODE, 32);

  // Wait for 30 seconds before sending the code again.
  delay(30000);
}
