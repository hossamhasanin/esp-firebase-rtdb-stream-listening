#include "rgbLight.h"
#include <Arduino.h>

RgbLight::RgbLight(uint8_t key) : Device(key , RGB_LIGHT){
    isOn = false;
    colorId = 0;
}

bool RgbLight::updatedDeviceState(DeviceStateHolder stateHolder){

    if (strcmp(stateHolder.fieldName , "isOn") == 0){
        if (stateHolder.boolValue == isOn){
            return false;
        }
        isOn = stateHolder.boolValue;
        return true;
    } else if (strcmp(stateHolder.fieldName , "colorId") == 0){
        if (stateHolder.intValue == colorId){
            return false;
        }
        Serial.printf("\n RGB color id %d\n",  stateHolder.intValue);
        Serial.printf("\n Stored color id %d\n",  colorId);
        colorId = stateHolder.intValue;
        return true;
    } else {
        Serial.printf("\n RGB color id %d\n",  stateHolder.intValue);
        Serial.printf("\n Stored color id %d\n",  colorId);
        isOn = stateHolder.boolValue;
        colorId = stateHolder.intValue; 
        return true;
    }
}

bool RgbLight::getIsOn(){
    return isOn;
}

uint8_t RgbLight::getColorId(){
    return colorId;
}