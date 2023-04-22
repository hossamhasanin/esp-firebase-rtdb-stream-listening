#include "switch.h"

Switch::Switch(uint8_t key) : Device(key , SWITCH){
    state = false;
}

void Switch::updatedDeviceState(DeviceStateHolder stateHolder){
    state = stateHolder.boolValue;
}

bool Switch::getState(){
    return state;
}