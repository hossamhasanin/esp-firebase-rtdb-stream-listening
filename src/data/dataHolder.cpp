#include "dataHolder.h"
// include library for strcmp
#include <string.h>
#include <Arduino.h>

DataItem DataHolder::parseDataFromKeyValue(const char* key, const char* value) {
    // convert key to int
    int keyInt = atoi(key);
    DataItem changedDataItem;
    
    uint8_t valueByte = (uint8_t)atoi(value);
    // compare the key with 0
    if (keyInt == temprature && _data[temprature] != valueByte) {
        // convert the value to uint8_t
        _data[temprature] = valueByte;
        changedDataItem.key = temprature;
        changedDataItem.value = _data[temprature];
    } else if (keyInt == switch1 && _data[switch1] != valueByte) {
        // convert the value to uint8_t
        _data[switch1] = valueByte;
        changedDataItem.key = switch1;
        changedDataItem.value = _data[switch1];
    } else {
        Serial.println("Unknown key "+String(key));
    }

    return changedDataItem;
}