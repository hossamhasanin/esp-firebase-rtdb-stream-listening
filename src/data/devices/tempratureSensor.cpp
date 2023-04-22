#include "tempratureSensor.h"

TempratureSensor::TempratureSensor(uint8_t key) : Device(key , TEMPRATURE_SENSOR){
    temprature = 0;
}

void TempratureSensor::updatedDeviceState(DeviceStateHolder stateHolder){
    temprature = stateHolder.intValue;
}

uint8_t TempratureSensor::getTemprature(){
    return temprature;
}