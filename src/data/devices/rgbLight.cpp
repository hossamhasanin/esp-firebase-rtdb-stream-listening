#include "rgbLight.h"

RgbLight::RgbLight(uint8_t key) : Device(key , RGB_LIGHT){
    isOn = false;
    colorId = 0;
}

void RgbLight::updatedDeviceState(DeviceStateHolder stateHolder){
    isOn = stateHolder.boolValue;
    colorId = stateHolder.intValue;
}

bool RgbLight::getIsOn(){
    return isOn;
}

uint8_t RgbLight::getColorId(){
    return colorId;
}