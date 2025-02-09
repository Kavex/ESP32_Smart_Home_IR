#include <WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>

// Wi-Fi credentials
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

// SinricPro credentials
#define APP_KEY "Your_SinricPro_App_Key"
#define APP_SECRET "Your_SinricPro_App_Secret"

// SinricPro Device ID
#define Device_ID "Your_SinricPro_Device_ID"

// IR pin configuration
const uint16_t kIrLedPin = 4; // IR transmitter pin
IRsend irsend(kIrLedPin);

// IR codes
const uint32_t IR_CODE_DEVICE_1 = 0x20DF10EF;
//const uint32_t IR_CODE_DEVICE_2 = 0xCDF1ABE2;

// Raw IR data from your Flipper Zero capture for IR_CODE_DEVICE_2
// The array holds alternating marks and spaces in microseconds.
uint16_t rawData[] = {
  4488, 4507, 525, 1716, 525, 1717, 524, 595, 526, 594, 526, 595,
  525, 595, 525, 1716, 525, 595, 525, 1715, 526, 1714, 527, 595,
  525, 594, 526, 1717, 524, 596, 524, 1716, 525, 594, 526, 1716,
  525, 597, 523, 596, 524, 595, 525, 593, 527, 596, 525, 597,
  523, 595, 525, 597, 523, 1716, 525, 1715, 526, 1713, 528, 1716,
  525, 1718, 523, 1715, 526, 1717, 524
};
// Calculate the number of elements in the rawData array
const uint16_t dataSize = sizeof(rawData) / sizeof(rawData[0]);

// Callback function for device control
bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device %s turned %s\n", deviceId.c_str(), state ? "on" : "off");

  if (state) {
    irsend.sendNEC(IR_CODE_DEVICE_1, 32); // Turn on device 1
    
    delay(500);
    //irsend.sendNEC(IR_CODE_DEVICE_2, 32); // Turn on device 2
    // Send the raw IR signal for IR_CODE_DEVICE_2 instead
    // - The third parameter is the carrier frequency in kHz.
    irsend.sendRaw(rawData, dataSize, 38);  

  } else {
    irsend.sendNEC(IR_CODE_DEVICE_1, 32); // Turn off device 1
    delay(500);
    //irsend.sendNEC(IR_CODE_DEVICE_2, 32); // Turn on device 2
    // Send the raw IR signal for IR_CODE_DEVICE_2 instead
    // - The third parameter is the carrier frequency in kHz.
    irsend.sendRaw(rawData, dataSize, 38);  
  }

  return true;
}

void setup() {
  Serial.begin(115200);
  irsend.begin();

  // Wi-Fi setup
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
  Serial.println(WiFi.localIP());

  // SinricPro setup
  SinricProSwitch &mySwitch = SinricPro[Device_ID];
  mySwitch.onPowerState(onPowerState);

  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true);
}

void loop() {
  SinricPro.handle();
}