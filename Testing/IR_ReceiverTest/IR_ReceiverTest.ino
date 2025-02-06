#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

// Define the GPIO pin where your IR receiver is connected.
const uint16_t kRecvPin = 14;

// Create an IRrecv object for the receiver.
IRrecv irrecv(kRecvPin);

// Structure to hold the decoded IR results.
decode_results results;

void setup() {
  // Initialize serial communication for debugging.
  Serial.begin(115200);
  // Give the ESP32 some time to initialize.
  delay(200);
  
  // Start the IR receiver.
  irrecv.enableIRIn();  
  Serial.println("IR receiver initialized on pin 14");
}

void loop() {
  // Check if an IR code has been received.
  if (irrecv.decode(&results)) {
    // Print the raw value of the received code in hexadecimal.
    Serial.print("Received IR code: 0x");
    Serial.println(results.value, HEX);
    
    // Optionally, print additional details using IRutils:
    // For a more detailed breakdown, uncomment the following line:
    // Serial.println(resultToHumanReadableBasic(&results));
    
    // Prepare to receive the next value.
    irrecv.resume();  
  }
}
