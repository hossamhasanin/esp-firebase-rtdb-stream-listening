#include "peopleCounter.h"

PeopleCounter::PeopleCounter(uint8_t key ) : Device(key , PEOPLE_COUNTER){
    peopleCount = 0;
}

void PeopleCounter::updatedDeviceState(DeviceStateHolder stateHolder){
    peopleCount = stateHolder.intValue;
}

uint8_t PeopleCounter::getPeopleCount(){
    return peopleCount;
}