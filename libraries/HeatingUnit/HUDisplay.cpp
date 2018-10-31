#include "HUDisplay.h"

//icons
HUIcon logo = {51, 0, logo_width, logo_height, logo_bits, true, 0};
HUIcon wifiIcon = {0, 0, wifi_icon_width, wifi_icon_height, wifi_icon_bits, false, 0};
HUIcon heatIcon = {110, 0, heat_icon_width, heat_icon_height, heat_icon_bits, false, 0};
HUIcon warningIcon = {110, 0, warning_icon_width, warning_icon_height, warning_icon_bits, false, 0};


/**
 *  Create a new OLED display instance and flip it if we need to
 */
HUDisplay::HUDisplay(): _display(0x3c, D1, D2) {}

void HUDisplay::initDisplay(bool flipScreen) {
    _display.init();
    if (flipScreen) {
        _display.flipScreenVertically();
    }
}

/**
 * Icon drawing with blink implementation
 */

void HUDisplay::drawIcon(bool blinkCondition, bool defaultState, HUIcon *icon) {
    if(blinkCondition) {
        if(millis() - icon->timer >= HUDISPLAY_BLINK_RATE) {
        icon->timer = millis();
        icon->visible = !icon->visible;
        }
    } else {
        icon->visible = defaultState;
    }
    
    if(icon->visible) {
        _display.drawXbm(icon->x, icon->y, icon->width, icon->height, icon->bits);
    }
}

/**
 * Drawing stuff here
 */
void HUDisplay::drawLogo() {
    this->drawIcon(false, true, &logo);
}

void HUDisplay::drawOEMName() {
    _display.setFont(ArialMT_Plain_10);
    _display.setTextAlignment(TEXT_ALIGN_CENTER);
    _display.drawString(64, 50, "Heating Unit v.0.2.0"); //(OTA+WEB)
}

void HUDisplay::clear() {
    _display.clear();
}

void HUDisplay::display() {
    _display.display();
}

void HUDisplay::drawStartupScreen() {
    this->clear();
    this->drawLogo();
    this->drawOEMName();
    this->display();
    delay(2000);
}

void HUDisplay::drawCurrentTemperature(float temp) {
    _display.setTextAlignment(TEXT_ALIGN_CENTER);
    _display.setFont(Dialog_plain_48);
    _display.drawString(64, 15, String((int)round(temp)) + "\u00B0");
}

void HUDisplay::drawSelectedTemperature(float temp) {
    _display.setTextAlignment(TEXT_ALIGN_RIGHT);
    _display.setFont(ArialMT_Plain_16);
    _display.drawString(128, 48, String((int)round(temp)) + "\u00B0");
}

void HUDisplay::drawProgress(int progress, String text) {
    _display.clear();
    _display.drawProgressBar(0, 32, 120, 10, progress);
    _display.setTextAlignment(TEXT_ALIGN_CENTER);
    _display.setFont(ArialMT_Plain_10);
    _display.drawString(64, 15, text);
    _display.display();
}

void HUDisplay::drawWifi(bool blinkCondition) {
    this->drawIcon(blinkCondition, true, &wifiIcon);
}

void HUDisplay::drawHeat(bool blinkCondition) {
    this->drawIcon(blinkCondition, false, &heatIcon);
}

void HUDisplay::drawWarning(bool blinkCondition) {
    this->drawIcon(blinkCondition, false, &warningIcon);
}