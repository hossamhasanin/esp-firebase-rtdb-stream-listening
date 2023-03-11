#pragma once

#include <stdint.h>
#include "dataItem.h"

#define DATA_FIELDS_COUNT 9


class DataHolder {
    private :
        // fields
        bool doorState;
        uint8_t temp;
        bool led1;
        bool electri;
        bool rgblState;
        uint8_t numOfPeople;
        double powerConsumption;
        bool gasLeakAlarm;
        bool passwordWrongAlarm;

        // void setIfChanged(int key , uint8_t valueByte , DataItem* changedDataItem);
    
    public :
        // DataItem parseDataFromKeyValue(int key, uint8_t value);
        bool setByteData(uint8_t key, uint8_t value);
        bool setBoolData(uint8_t key, bool value);
        bool setDoubleData(uint8_t key, double value);
        static bool isKeyValid(uint8_t key);

        uint8_t getByteData(uint8_t key);
        // create getters for the fields
        bool getDoorState() {
            return doorState;
        }

        uint8_t getTemp() {
            return temp;
        }

        bool getLed1() {
            return led1;
        }

        bool getElectri() {
            return electri;
        }

        bool getRgblState() {
            return rgblState;
        }

        uint8_t getNumOfPeople() {
            return numOfPeople;
        }

        double getPowerConsumption() {
            return powerConsumption;
        }

        bool getGasLeakAlarm() {
            return gasLeakAlarm;
        }

        bool getPasswordWrongAlarm() {
            return passwordWrongAlarm;
        }

        // create setters for the fields

        void setDoorState(bool doorState) {
            this->doorState = doorState;
        }

        void setTemp(uint8_t temp) {
            this->temp = temp;
        }

        void setLed1(bool led1) {
            this->led1 = led1;
        }

        void setElectri(bool electri) {
            this->electri = electri;
        }

        void setRgblState(bool rgblState) {
            this->rgblState = rgblState;
        }

        void setNumOfPeople(uint8_t numOfPeople) {
            this->numOfPeople = numOfPeople;
        }

        void setPowerConsumption(double powerConsumption) {
            this->powerConsumption = powerConsumption;
        }

        void setGasLeakAlarm(bool gasLeakAlarm) {
            this->gasLeakAlarm = gasLeakAlarm;
        }

        void setPasswordWrongAlarm(bool passwordWrongAlarm) {
            this->passwordWrongAlarm = passwordWrongAlarm;
        }
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
    passwordWrongAlarmId = 9,
    powerConsumptionId = 10,
};

