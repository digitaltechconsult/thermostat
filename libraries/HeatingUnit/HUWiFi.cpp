#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "HUWiFi.h"

char _ssid[20];
char _password[20];
bool _skip = false;
unsigned long _lastConnectTry = 0;
ESP8266WebServer server(80);

HUWiFi::HUWiFi(String ssid, String password) {
    ssid.toCharArray(_ssid, 20);
    password.toCharArray(_password, 20);
}

/**
 * Setup WiFi connection
 * this method wil call a callback to display progress onscreen
 */
void HUWiFi::init(std::function<void(int, String)> progress) {
    WiFi.begin(_ssid, _password);
    int idx = 0;
    progress(idx, "Connecting to " + String(_ssid));

    //as long as we don't have a wifi connection
    //and we should not skip the init process
    while (!this->isConnected() && !_skip) {
        //display progress
        delay(500);
        idx++;
        progress(idx * 2, "Connecting to " + String(_ssid));

        //if we waited enough then skip wifi init
        if (idx >= 50) {
            _skip = true; //this exits loop, skip WiFi connection routine
            progress(100, "Error connecting to " + String(_ssid));
            WiFi.disconnect();
            delay(2000);
        }
    }

    //if connection was successfuly
    if (this->isConnected()) {
        WiFi.hostname(HUWIFI_HOSTNAME);
        String ip = WiFi.localIP().toString();
        progress(100, String(_ssid) + " (" + ip + ")");

        //OTA
        this->setupOTA();

        //webserver
        server.begin();

        delay(2000);
    }
}

/**
 * Helper method to find out if WiFi is connected or not
 */
bool HUWiFi::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

/**
 * Method that allow to retry if wifi not connected
 */
bool HUWiFi::canRetryConnection() {
    if(!this->isConnected() && millis() - _lastConnectTry > HUWIFI_RETRY_INTERVAL) {
        _lastConnectTry = millis();
        _skip = false;
        return true;
    }
    return false;
}

void HUWiFi::setupOTA() {
    ArduinoOTA.setHostname(HUWIFI_HOSTNAME);
    ArduinoOTA.begin();

    ArduinoOTA.onStart([](){
        //handle start of upload
        //TODO: Display a progress bar or something
        //stop relay
    });

    ArduinoOTA.onEnd([](){
        ESP.restart();
    });

}

void HUWiFi::setupWebserver(HUThermostat *thermostat) {
    String contentType = "text/html";

    server.on("/temperature", [contentType, thermostat]() {
        String page ="<html> <body style=\"font-family:Arial;text-align:center;\"> <p>Current temp is: " + String(thermostat->getCurrentTemp()) +
    "</p> <p>Selected temp is: " + String(thermostat->getSelectedTemp()) + 
    "</p> <p>Relay on: "+ String(thermostat->isRelayOn()) + 
    "</p> <p>Exception abort: "+ String(thermostat->isExceptionAbort()) + 
    "</p> <form action = \"/temperature\" method = \"GET\"> <input type=\"submit\" name=\"command\" value=\"decrease\" style=\"width: 75px;height: 75px;\"/> <input type=\"submit\" name=\"command\" value=\"increase\" style=\"width: 75px;height: 75px;\"/> </form> </body> </html>";
        String command = server.arg("command");
        if(command.equals("increase")) {
            thermostat->handleButtonsInterrupt(true);
            delay(10);
            server.sendHeader("Location", String("/temperature"), true);
            server.send(302, "text/plain", "");
        } else if(command.equals("decrease")) {
            delay(10);
            thermostat->handleButtonsInterrupt(false);
            server.sendHeader("Location", String("/temperature"), true);
            server.send(302, "text/plain", "");
        } else {
            server.send(200, contentType, page);
        }
    });
}

void HUWiFi::OTA() {
    ArduinoOTA.handle();
    yield();
}


void HUWiFi::webserverHandler() {
    server.handleClient();
    yield();
}

