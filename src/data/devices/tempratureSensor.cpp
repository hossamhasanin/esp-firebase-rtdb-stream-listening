#include "tempratureSensor.h"

TempratureSensor::TempratureSensor(uint8_t key) : Device(key , TEMPRATURE_SENSOR){
    temprature = 0;
}

bool TempratureSensor::updatedDeviceState(DeviceStateHolder stateHolder){
    if (stateHolder.intValue == temprature){
        return false;
    }
    temprature = stateHolder.intValue;
    return true;
}

uint8_t TempratureSensor::getTemprature(){
    return temprature;
}