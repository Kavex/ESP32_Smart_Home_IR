#include <WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>

#define REBOOT_INTERVAL 2700000  // 45 minutes in milliseconds
unsigned long lastRebootTime = 0;
bool bootComplete = false;  // Flag to track first boot

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
const uint32_t IR_CODE_DEVICE_1_ON = 0x20DF10EF;
const uint32_t IR_CODE_DEVICE_1_OFF = 0x20DF40BF;

// Raw IR Data for ON/OFF (Replace with correct raw data)
uint16_t rawDataOn[] = { /* Your IR data here */ };
uint16_t rawDataOff[] = { /* Your IR data here */ };
const uint16_t dataSizeOn = sizeof(rawDataOn) / sizeof(rawDataOn[0]);
const uint16_t dataSizeOff = sizeof(rawDataOff) / sizeof(rawDataOff[0]);

// Callback function for device control
bool onPowerState(const String &deviceId, bool &state) {
  if (!bootComplete) {
    Serial.println("Ignoring first command after boot.");
    return true; // Ignore first command
  }

  Serial.printf("Device %s turned %s\n", deviceId.c_str(), state ? "ON" : "OFF");

  if (state) {
    irsend.sendNEC(IR_CODE_DEVICE_1_ON, 32);
    delay(200);
    irsend.sendRaw(rawDataOn, dataSizeOn, 38);
  } else {
    irsend.sendNEC(IR_CODE_DEVICE_1_OFF, 32);
    delay(200);
    irsend.sendRaw(rawDataOff, dataSizeOff, 38);
  }

  return true;
}

void connectWiFi() {
  WiFi.begin(ssid, password);
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(1000);
    Serial.print(".");
    retries++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi!");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection failed!");
  }
}

void rebootESP32() {
  Serial.println("Rebooting ESP32...");
  delay(1000);
  ESP.restart();
}

void setup() {
  Serial.begin(115200);
  irsend.begin();
  
  connectWiFi();

  // SinricPro setup
  SinricProSwitch &mySwitch = SinricPro[Device_ID];
  mySwitch.onPowerState(onPowerState);

  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true);

  bootComplete = true;  // Set flag after setup completes
  lastRebootTime = millis(); // Start the reboot timer
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting WiFi...");
    connectWiFi();
  }

  SinricPro.handle();

  // Check if 45 minutes have passed
  if (millis() - lastRebootTime >= REBOOT_INTERVAL) {
    rebootESP32();
  }
}
