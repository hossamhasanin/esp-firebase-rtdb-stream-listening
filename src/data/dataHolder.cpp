#include "dataHolder.h"
// include library for strcmp
#include <string.h>
#include <Arduino.h>

DataItem DataHolder::parseDataFromKeyValue(const char* key, const char* value) {
    // convert key to int
    int keyInt = atoi(key);
    DataItem changedDataItem;
    
    // compare the key with 0
    if (keyInt == temprature) {
        // convert the value to uint8_t
        _data[temprature] = (uint8_t)atoi(value);
        changedDataItem.key = temprature;
        changedDataItem.value = _data[temprature];
    } else if (keyInt == switch1) {
        // convert the value to uint8_t
        _data[switch1] = (uint8_t)atoi(value);
        changedDataItem.key = switch1;
        changedDataItem.value = _data[switch1];
    } else {
        Serial.println("Unknown key "+String(key));
    }

    return changedDataItem;
}