#pragma once

#include <stdint.h>
#include "dataItem.h"

#define DATA_FIELDS_COUNT 9

#define DOOR_STATE_INDEX 0
#define TEMP_INDEX 1
#define LED1_INDEX 2
#define ELECTRI_INDEX 3
#define RGBL_STATE_INDEX 4
#define GAS_LEAK_ALARM_INDEX 5
#define NUM_OF_PEOPLE_INDEX 6
#define POWER_CONSUMPTION_INDEX 7
#define PASSWORD_WRONG_ALARM_INDEX 8


class DataHolder {
    private :
        uint8_t _data[DATA_FIELDS_COUNT];

        void setIfChanged(int key , uint8_t valueByte , DataItem* changedDataItem);
    
    public :
        DataItem parseDataFromKeyValue(int key, uint8_t value);
        uint8_t get(int key);
        DataItem getDataItem(int key);
        void set(int key , uint8_t value);

};

// create enum for the data fields
enum FeaturesIds {
    // add the fields here
    // for example:
    // field1,
    // field2,
    // field3
    // create member called temprature that is equal to 0
    doorState = 1,
    temp = 2,
    led1 = 3,
    electri = 4,
    rgblState = 5,
    gasLeakAlarm = 6,
    numOfPeople = 7,
    passwordWrongAlarm = 9,
    powerConsumption = 10,
};

