/**
 * Heating Unit Controller - ESP8266 WiFi implementation
 * Bogdan Coticopol, October 2018
 */

#ifndef HUWiFi_h
#define HUWiFi_h

#include "HUThermostat.h"

#define HUWIFI_RETRY_INTERVAL 600000
#define HUWIFI_HOSTNAME "HUC01"

class HUWiFi 
{
private:
    void setupOTA();
public:
    HUWiFi(String ssid, String password);
    void init(std::function<void(int, String)> progress);
    bool isConnected();
    bool canRetryConnection();
    void OTA();
    void webserverHandler();
    void setupWebserver(HUThermostat *thermostat);
};

#endif