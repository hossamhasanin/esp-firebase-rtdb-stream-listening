#include "switch.h"

Switch::Switch(uint8_t key , bool canSendStateToUart , bool canUpdateStateToUart) : Device(key , SWITCH , canSendStateToUart , canUpdateStateToUart){
    state = false;
}

bool Switch::updatedDeviceState(DeviceStateHolder stateHolder){
    if (stateHolder.boolValue == state){
        return false;
    }
    state = stateHolder.boolValue;
    return true;
}

bool Switch::getState(){
    return state;
}