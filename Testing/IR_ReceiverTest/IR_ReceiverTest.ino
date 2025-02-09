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
    // Print the decoded IR code in hexadecimal.
    Serial.print("Received IR code: 0x");
    Serial.println(results.value, HEX);
    
    // Print a human-readable breakdown (if you want additional details).
    // Uncomment the next line if desired.
    // Serial.println(resultToHumanReadableBasic(&results));
    
    // Print the raw timing data.
    // The raw timing data is stored in results.rawbuf and the count is results.rawlen.
    Serial.print("Raw IR timings (");
    Serial.print(results.rawlen);
    Serial.println(" values):");
    for (uint16_t i = 0; i < results.rawlen; i++) {
      Serial.print(results.rawbuf[i]);
      if (i < results.rawlen - 1) {
        Serial.print(", ");
      }
    }
    Serial.println();  // Newline for clarity

    // Prepare to receive the next IR value.
    irrecv.resume();  
  }
}
