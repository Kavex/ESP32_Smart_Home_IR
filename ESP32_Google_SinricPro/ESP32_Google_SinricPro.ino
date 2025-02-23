#include <WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>

#define REBOOT_INTERVAL 2700000  // 45 minutes in milliseconds
unsigned long lastRebootTime = 0;
bool bootComplete = false;  // Flag to prevent IR signal on first boot

// Wi-Fi credentials
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

// SinricPro credentials
#define APP_KEY "Your_SinricPro_App_Key"
#define APP_SECRET "Your_SinricPro_App_Secret"
#define Device_ID "Your_SinricPro_Device_ID"

// IR pin configuration
const uint16_t kIrLedPin = 4; // IR transmitter pin
IRsend irsend(kIrLedPin);

// IR codes
const uint32_t IR_CODE_DEVICE_1 = 0x20DF10EF;

uint16_t rawData[] = {
  4488, 4507, 525, 1716, 525, 1717, 524, 595, 526, 594, 526, 595,
  525, 595, 525, 1716, 525, 595, 525, 1715, 526, 1714, 527, 595,
  525, 594, 526, 1717, 524, 596, 524, 1716, 525, 594, 526, 1716,
  525, 597, 523, 596, 524, 595, 525, 593, 527, 596, 525, 597,
  523, 595, 525, 597, 523, 1716, 525, 1715, 526, 1713, 528, 1716,
  525, 1718, 523, 1715, 526, 1717, 524
};
const uint16_t dataSize = sizeof(rawData) / sizeof(rawData[0]);

// Callback function for device control
bool onPowerState(const String &deviceId, bool &state) {
  if (!bootComplete) {
    Serial.println("Ignoring first command after boot.");
    bootComplete = true; // Allow future commands
    return true; // Ignore first execution
  }

  Serial.printf("Device %s turned %s\n", deviceId.c_str(), state ? "on" : "off");

  if (state) {
    irsend.sendNEC(IR_CODE_DEVICE_1, 32); // Turn on device 1
    delay(500);
    irsend.sendRaw(rawData, dataSize, 38);
  } else {
    irsend.sendNEC(IR_CODE_DEVICE_1, 32); // Turn off device 1
    delay(500);
    irsend.sendRaw(rawData, dataSize, 38);
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

  lastRebootTime = millis(); // Start reboot timer
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
