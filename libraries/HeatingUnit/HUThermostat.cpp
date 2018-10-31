#include "HUThermostat.h"


/// INIT
HUThermostat::HUThermostat(int tempSensorPin, int tempPlusPin, int tempMinusPin, int relayPin) {
    //setup pin numbers
    _tempSensorPin = tempSensorPin;
    _tempPlusPin = tempPlusPin;
    _tempMinusPin = tempMinusPin;
    _relayPin = relayPin;

    //setup pins
    this->setupPins();
}

void HUThermostat::setupPins() {
    pinMode(_tempSensorPin, INPUT);
    pinMode(_relayPin, OUTPUT);
    pinMode(_tempPlusPin, INPUT);
    pinMode(_tempMinusPin, INPUT);
}

void HUThermostat::handleButtonsInterrupt(bool increasePressed) {
    if(increasePressed && _tempSelected < HUTHERMOSTAT_MAX_TEMP) {
        _tempSelected++;
    } else if(!increasePressed && _tempSelected > HUTHERMOSTAT_MIN_TEMP) {
        _tempSelected--;
    }
    _lastLoopTime = 0;
}

/// THERMOSTAT LOOP
float HUThermostat::temperatureRead() {
    //read temperature and handle relay once @1min
    int sensorValue = analogRead(_tempSensorPin);
    delay(1);
    float analogVoltage = sensorValue * 3300 / 1024;

    //maybe the cause is the wrong resistor 
    //but the correct voltage was about 93% of the read voltage
    //we used an external thermometer and we found out that 90% is more accurate (~89%)
    //enclosure heats sensor more than expected
    analogVoltage *= 0.85;

    float temp = (analogVoltage - 500) / 10;  

    //if temp below 0 something is wrong, sensor may be broken
    if (temp <= 0) {
      _exception_abort = true;
    } 

    return temp;
}

void HUThermostat::temperatureComputed(int numberOfReads) {
    float temp = 0;
    for (int idx = 0; idx < numberOfReads; idx++) {
        temp += this->temperatureRead();
    }
    _tempCurrent = temp / (float)numberOfReads;
    Serial.print("Current temperature is: "); Serial.println(_tempCurrent);
}

void HUThermostat::thermostatLogic() {
    //this checks if heating unit runs for too long
    if(!_exception_abort && _relayOn && _lastRelayOn > 0 && millis() - _lastRelayOn > HUTHERMOSTAT_LONG_TIME) {
        _exception_abort = true;
    }

    //stop relay if warning
    if(_exception_abort) {
        _relayOn = false;
        return;
    }

    if(_tempSelected - _tempCurrent > HUTHERMOSTAT_NEGATIVE_THRESHOLD) {         //if it's cold
        _relayOn = true;
        _lastRelayOn = millis();
    } else if(_tempCurrent - _tempSelected > HUTHERMOSTAT_POSITIVE_THRESHOLD) { //if it's hot
        _relayOn = false;
        _lastRelayOn = 0;
    }
}

void HUThermostat::relayHandler() {
    if(_relayOn) {
        digitalWrite(_relayPin, HIGH);
    } else {
        digitalWrite(_relayPin, LOW);
    }
    delay(1);
}

void HUThermostat::thermostatLoop() {
    if(_lastLoopTime == 0 || millis() - _lastLoopTime >= HUTHERMOSTAT_LOOP_TIME) {
        _lastLoopTime = millis();
        this->temperatureComputed(HUTHERMOSTAT_NUMBER_OF_READS);
        this->thermostatLogic();
        this->relayHandler();
    }
}

/// UTILS
bool HUThermostat::isRelayOn() {
    return _relayOn;
}

bool HUThermostat::isExceptionAbort() {
    return _exception_abort;
}

float HUThermostat::getCurrentTemp() {
    return _tempCurrent;
}

float HUThermostat::getSelectedTemp() {
    return _tempSelected;
}


