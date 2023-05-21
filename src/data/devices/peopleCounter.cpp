#include "peopleCounter.h"

PeopleCounter::PeopleCounter(uint8_t key ) : Device(key , PEOPLE_COUNTER){
    peopleCount = 0;
}

bool PeopleCounter::updatedDeviceState(DeviceStateHolder stateHolder){
    if (stateHolder.intValue == peopleCount){
        return false;
    }
    peopleCount = stateHolder.intValue;
    return true;
}

uint8_t PeopleCounter::getPeopleCount(){
    return peopleCount;
}