#include "rgbLight.h"

RgbLight::RgbLight(uint8_t key) : Device(key , RGB_LIGHT){
    isOn = false;
    colorId = 0;
}

bool RgbLight::updatedDeviceState(DeviceStateHolder stateHolder){
    if (stateHolder.boolValue == isOn || stateHolder.intValue == colorId){
        return false;
    }
    isOn = stateHolder.boolValue;
    colorId = stateHolder.intValue;
    return true;
}

bool RgbLight::getIsOn(){
    return isOn;
}

uint8_t RgbLight::getColorId(){
    return colorId;
}