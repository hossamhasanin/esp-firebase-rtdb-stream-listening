#include "powerConsumption.h"

PowerConsumption::PowerConsumption(uint8_t key) : Device(key , POWER_CONSUMPTION){
    powerConsumption = 0;
}

bool PowerConsumption::updatedDeviceState(DeviceStateHolder stateHolder){
    powerConsumption = stateHolder.doubleValue;
    return true;
}

double PowerConsumption::getPowerConsumption(){
    return powerConsumption;
}