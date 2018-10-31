/**
 * Heating Unit Controller - Thermostat hardware implementation
 * Bogdan Coticopol, October 2018
 */


#ifndef HUTHERMOSTAT_h
#define HUTHERMOSTAT_h

#define HUTHERMOSTAT_MAX_TEMP             35.0
#define HUTHERMOSTAT_MIN_TEMP             17.0
#define HUTHERMOSTAT_LONG_TIME            21600000
#define HUTHERMOSTAT_POSITIVE_THRESHOLD   0.5
#define HUTHERMOSTAT_NEGATIVE_THRESHOLD   0.5
#define HUTHERMOSTAT_LOOP_TIME            10000
#define HUTHERMOSTAT_NUMBER_OF_READS      128


#include <Arduino.h>
#include <Math.h>

class HUThermostat 
{
private:
//vars
    //pins
    int _tempSensorPin;
    int _tempPlusPin;
    int _tempMinusPin;
    int _relayPin;
    //temps
    float _tempCurrent = 0;
    float _tempSelected = HUTHERMOSTAT_MIN_TEMP;
    //warning exception
    bool _exception_abort = false;
    //relay
    unsigned long _lastRelayOn = 0;
    bool _relayOn = false;
    //thermostat loop
    unsigned long _lastLoopTime = 0;
    
//methods
    void setupPins();
    float temperatureRead();
    void thermostatLogic();
    void relayHandler();
    void temperatureComputed(int numberOfReads);
public:
//methods
    HUThermostat(int tempSensorPin, int tempPlusPin, int tempMinusPin, int relayPin);
    void handleButtonsInterrupt(bool increasePressed);
    void thermostatLoop();
    bool isRelayOn();
    bool isExceptionAbort();
    float getCurrentTemp();
    float getSelectedTemp();
};

#endif