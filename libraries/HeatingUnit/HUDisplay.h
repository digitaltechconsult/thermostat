/**
 * Heating Unit Controller - SSD1306Wire OLED implementation
 * Bogdan Coticopol, October 2018
 */


#ifndef HUDisplay_h
#define HUDisplay_h

#define HUDISPLAY_BLINK_RATE 500

#include <Arduino.h>
#include "SSD1306Wire.h"
#include "HUIcon.h"

//images
#include "logo.h" //logo image
#include "wifi_icon.h" //wifi icon
#include "heat_icon.h"
#include "warning_icon.h"
#include "iot_icon.h"

//fonts
#include "font_48.h" //a bigger font

class HUDisplay 
{

private:  
//vars
    SSD1306Wire _display;
//methods
    void drawLogo();
    void drawOEMName();
    void drawIcon(bool blinkCondition, bool defaultState, HUIcon *icon);
    
public:
    HUDisplay();
    void initDisplay(bool flipScreen);
    void clear();
    void display();
    void drawStartupScreen();
    void drawWifi(bool blinkCondition);
    void drawHeat(bool blinkCondition);
    void drawWarning(bool blinkCondition);
    void drawCurrentTemperature(float temp);
    void drawSelectedTemperature(float temp);
    void drawProgress(int progress, String text);
};

#endif