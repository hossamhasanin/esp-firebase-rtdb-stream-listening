#include "powerConsumption.h"

PowerConsumption::PowerConsumption(uint8_t key) : Device(key , POWER_CONSUMPTION){
    powerConsumption = 0;
}

void PowerConsumption::updatedDeviceState(DeviceStateHolder stateHolder){
    powerConsumption = stateHolder.doubleValue;
}

double PowerConsumption::getPowerConsumption(){
    return powerConsumption;
}