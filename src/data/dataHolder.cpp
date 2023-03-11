#include "dataHolder.h"
// include library for strcmp
#include <string.h>
#include <Arduino.h>

// DataItem DataHolder::parseDataFromKeyValue(int key, uint8_t value) {
//     // convert key to int
//     // int keyInt = key;
//     DataItem changedDataItem;
    
//     // compare the key with 0
//     // if (keyInt == temprature) {
//     //     // parse the value to int
//     //     setIfChanged(temprature, value, &changedDataItem);
//     // } else if (keyInt == switch1) {

//     //     setIfChanged(switch1, value, &changedDataItem);
//     // } else {
//     //     Serial.println("Unknown key "+String(key));
//     // }
    
//     setIfChanged(key, value, &changedDataItem);

//     return changedDataItem;
// }

// void DataHolder::setIfChanged(int key , uint8_t valueByte , DataItem* changedDataItem) {
    
//     if (valueByte == get(key)) {
//         changedDataItem->key = -1;
//         changedDataItem->value = -1;
//         return;
//     }

//     set(key, valueByte);
//     changedDataItem->key = key;
//     changedDataItem->value = get(key);
// }

// uint8_t DataHolder::get(int key) {
//     if (key == doorStateId){
//         return doorState;
//     } else if (key == tempId){
//         return temp;
//     } else if (key == led1Id){
//         return led1;
//     } else if (key == electriId){
//         return electriId;
//     } else if (key == rgblStateId){
//         return rgblState;
//     } else if (key == gasLeakAlarmId){
//         return gasLeakAlarm;
//     } else if (key == numOfPeopleId){
//         return numOfPeople;
//     } else if (key == powerConsumptionId){
//         return powerConsumption;
//     } else if (key == passwordWrongAlarmId){
//         return passwordWrongAlarm;
//     } else {
//         throw "Unknown key";
//     }
// }
bool DataHolder::setByteData(uint8_t key, uint8_t value) {
    if (key == tempId){
        if (temp == value){
            return false;
        }
        temp = value;
        return true;
    } else if (key == numOfPeopleId){
        if (numOfPeople == value){
            return false;
        }
        numOfPeople = value;
        return true;
    } else {
        throw "Unknown key";
    }
}

bool DataHolder::setBoolData(uint8_t key, bool value) {
    if (key == doorStateId){
        if (doorState == value){
            return false;
        }
        doorState = value;
        return true;
    } else if (key == led1Id){
        if (led1 == value){
            return false;
        }
        led1 = value;
        return true;
    } else if (key == electriId){
        if (electri == value){
            return false;
        }
        electri = value;
        return true;
    } else if (key == rgblStateId){
        if (rgblState == value){
            return false;
        }
        rgblState = value;
        return true;
    } else if (key == gasLeakAlarmId){
        if (gasLeakAlarm == value){
            return false;
        }
        gasLeakAlarm = value;
        return true;
    } else if (key == passwordWrongAlarmId){
        if (passwordWrongAlarm == value){
            return false;
        }
        passwordWrongAlarm = value;
        return true;
    } else {
        throw "Unknown key";
    }
}

bool DataHolder::setDoubleData(uint8_t key, double value) {
    if (key == powerConsumptionId){
        if (powerConsumption == value){
            return false;
        }
        powerConsumption = value;
        return true;
    } else {
        throw "Unknown key";
    }
}

uint8_t DataHolder::getByteData(uint8_t key) {
    if (key == led1Id){
        return led1;
    } else if (key == electriId){
        return electri;
    } else if (key == rgblStateId){
        return rgblState;
    } else {
        throw "Un supported get key "+String(key);
    }
}

bool DataHolder::isKeyValid(uint8_t key) {
    if (key == doorStateId){
        return true;
    } else if (key == tempId){
        return true;
    } else if (key == led1Id){
        return true;
    } else if (key == electriId){
        return true;
    } else if (key == rgblStateId){
        return true;
    } else if (key == gasLeakAlarmId){
        return true;
    } else if (key == numOfPeopleId){
        return true;
    } else if (key == powerConsumptionId){
        return true;
    } else if (key == passwordWrongAlarmId){
        return true;
    } else {
        return false;
    }
}