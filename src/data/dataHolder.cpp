#include "dataHolder.h"
// include library for strcmp
#include <string.h>
#include <Arduino.h>

DataItem DataHolder::parseDataFromKeyValue(const char* key, const char* value) {
    // convert key to int
    int keyInt = atoi(key);
    DataItem changedDataItem;
    
    uint8_t valueByte;
    // compare the key with 0
    if (keyInt == temprature) {
        // parse the value to int
        valueByte = (uint8_t)atoi(value);

        setIfChanged(temprature, valueByte, &changedDataItem);
    } else if (keyInt == switch1) {
        // parse the value to bool
        if (strcmp(value, "true") == 0) {
            valueByte = true;
        } else {
            valueByte = false;
        }

        setIfChanged(switch1, valueByte, &changedDataItem);
    } else {
        Serial.println("Unknown key "+String(key));
    }

    return changedDataItem;
}

void DataHolder::setIfChanged(int key , uint8_t valueByte , DataItem* changedDataItem) {
    
    if (valueByte == _data[key]) return;

    _data[key] = valueByte;
    changedDataItem->key = key;
    changedDataItem->value = _data[key];
}