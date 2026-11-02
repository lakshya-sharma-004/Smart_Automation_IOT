#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>
#include <map>

// -------------------- Wi-Fi Credentials --------------------
const char* WIFI_SSID = "Lucky's Pixel";
const char* WIFI_PASS = "0987654321";

// -------------------- SinricPro Credentials --------------------
#define APP_KEY    "c453ce09-e2e6-4b9a-9871-4dc951be065f"
#define APP_SECRET "966bfecc-9139-421d-9acb-9bf0ac9f1f5c-ba12fcf9-5725-45af-8be1-34ac531a3098"

// Replace with your actual SinricPro device IDs
#define DEVICE_ID_1 "6ae711c57d04ce9fd6362134"
#define DEVICE_ID_2 "6ae719426d3b5ef5cd9ebebd"
#define DEVICE_ID_3 "6ae719326d3b5ef5cd9ebe8b"


// -------------------- Relay Pins --------------------
#define RELAY_1 D1
#define RELAY_2 D2
#define RELAY_3 D5


// Active-LOW relay module
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

// Map device IDs to relay pins
std::map<String, int> devicePins = {
  {DEVICE_ID_1, RELAY_1},
  {DEVICE_ID_2, RELAY_2},
  {DEVICE_ID_3, RELAY_3},

};

// -------------------- Relay Control --------------------
bool onPowerState(const String &deviceId, bool &state) {
  auto it = devicePins.find(deviceId);

  if (it == devicePins.end()) {
    Serial.println("Unknown device ID");
    return false;
  }

  int relayPin = it->second;

  digitalWrite(relayPin, state ? RELAY_ON : RELAY_OFF);

  Serial.print("Device ");
  Serial.print(deviceId);
  Serial.print(" is ");
  Serial.println(state ? "ON" : "OFF");

  return true;
}

// -------------------- Wi-Fi Connection --------------------
bool connectWiFi() {
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  unsigned long startAttempt = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startAttempt < 20000) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    return true;
  }

  Serial.print("Wi-Fi connection failed. Status code: ");
  Serial.println(WiFi.status());

  return false;
}

// -------------------- SinricPro Setup --------------------
void setupSinricPro() {
  for (const auto &device : devicePins) {
    SinricProSwitch &mySwitch = SinricPro[device.first];
    mySwitch.onPowerState(onPowerState);
  }

  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true);

  Serial.println("SinricPro initialized");
}

// -------------------- Setup --------------------
void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize relay pins and keep them OFF
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);


  digitalWrite(RELAY_1, RELAY_OFF);
  digitalWrite(RELAY_2, RELAY_OFF);
  digitalWrite(RELAY_3, RELAY_OFF);


  Serial.println();
  Serial.println("NodeMCU Home Automation Starting...");

  if (connectWiFi()) {
    setupSinricPro();
  } else {
    Serial.println("Wi-Fi not connected. Restarting in 10 seconds...");
    delay(10000);
    ESP.restart();
  }
}

// -------------------- Main Loop --------------------
void loop() {
  SinricPro.handle();
}