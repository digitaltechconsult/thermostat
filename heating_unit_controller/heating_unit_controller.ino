#include "HUWiFi.h"
#include "HUDisplay.h"
#include "HUThermostat.h"

#define SSID                "your network name"
#define PASS                "your network pass"
#define TEMP_SENSOR_PIN     A0
#define INCREASE_TEMP_PIN   D3
#define DECREASE_TEMP_PIN   D4
#define RELAY_PIN           D7
#define INTERRUPT_DELAY     150

//Global stuff here
unsigned long lastInterruptCall = 0;

//Hardware & thermostat
HUThermostat thermostat(TEMP_SENSOR_PIN, INCREASE_TEMP_PIN, DECREASE_TEMP_PIN, RELAY_PIN);

//WiFi stuff
HUWiFi wifi(SSID, PASS);

//OLED Display
HUDisplay screen;

/**
 * Display functions
 */

void DrawThermostatScreen() {
  screen.clear();

  screen.drawWifi(!wifi.isConnected());
  screen.drawHeat(thermostat.isRelayOn());
  screen.drawWarning(thermostat.isExceptionAbort());

  screen.drawCurrentTemperature(thermostat.getCurrentTemp());
  screen.drawSelectedTemperature(thermostat.getSelectedTemp());

  delay(1);
  screen.display();
}

void DrawProgress(int progress, String text) {
  screen.drawProgress(progress, text);
}

/**
 * Hardware workaround
 */
void TempIncreaseInterrupt() {
  Serial.println("TempIncreaseInterrupt() called.");
  if(millis() -lastInterruptCall >= INTERRUPT_DELAY) {
       lastInterruptCall = millis();
        thermostat.handleButtonsInterrupt(true);
        
    }
}

void TempDecreaseInterrupt() {
  Serial.println("TempDecreaseInterrupt() called.");
   if(millis() -lastInterruptCall >= INTERRUPT_DELAY) {
       lastInterruptCall = millis();
        thermostat.handleButtonsInterrupt(false);
    }
}

void setup() {
  //for debug
  Serial.begin(115200);

  //hardware interrupts for buttons
  attachInterrupt(digitalPinToInterrupt(INCREASE_TEMP_PIN), TempIncreaseInterrupt, HIGH);
  attachInterrupt(digitalPinToInterrupt(DECREASE_TEMP_PIN), TempDecreaseInterrupt, HIGH);
  lastInterruptCall = 0;

  //display setup
  screen.initDisplay(false);
  screen.drawStartupScreen();

  //connect to WiFi
  wifi.init(DrawProgress);
  //setup web server
  wifi.setupWebserver(&thermostat);
}

void loop() {
  thermostat.thermostatLoop();

  //second as importance
  DrawThermostatScreen();

  //check if WiFi is connected, if not retry connection
  if(wifi.canRetryConnection()) {
    wifi.init(DrawProgress);
  }
  //OTA
  wifi.OTA();

  //webserver
  wifi.webserverHandler();
}
