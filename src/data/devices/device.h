#pragma once


#include "deviceStateHolder.h"
#include "devicesTypes.h"


#define DATA_FIELDS_COUNT 14

class Device{
    uint8_t key;
    DevicesTypes type;

    public:

        Device(uint8_t key , DevicesTypes type){
            this->key = key;
            this->type = type;
        }
        
        uint8_t getKey(){
            return key;
        }

        DevicesTypes getType(){
            return type;
        }

        virtual bool updatedDeviceState(DeviceStateHolder stateHolder);
};

// create enum for the data fields
enum FeaturesIds {
    // add the fields here
    // for example:
    // field1,
    // field2,
    // field3
    // create member called temprature that is equal to 0
    doorStateId = 1,
    tempId = 2,
    led1Id = 3,
    electriId = 4,
    rgblStateId = 5,
    gasLeakAlarmId = 6,
    numOfPeopleId = 7,
    acCommands = 8,
    passwordWrongAlarmId = 9,
    powerConsumptionId = 10,
};
