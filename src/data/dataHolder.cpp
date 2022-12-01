#include "dataHolder.h"
// include library for strcmp
#include <string.h>
#include <Arduino.h>

DataItem DataHolder::parseDataFromKeyValue(int key, uint8_t value) {
    // convert key to int
    // int keyInt = key;
    DataItem changedDataItem;
    
    // compare the key with 0
    // if (keyInt == temprature) {
    //     // parse the value to int
    //     setIfChanged(temprature, value, &changedDataItem);
    // } else if (keyInt == switch1) {

    //     setIfChanged(switch1, value, &changedDataItem);
    // } else {
    //     Serial.println("Unknown key "+String(key));
    // }
    
    setIfChanged(key, value, &changedDataItem);

    return changedDataItem;
}

void DataHolder::setIfChanged(int key , uint8_t valueByte , DataItem* changedDataItem) {
    
    if (valueByte == _data[key]) return;

    _data[key] = valueByte;
    changedDataItem->key = key;
    changedDataItem->value = _data[key];
}

uint8_t DataHolder::get(int key) {
    return _data[key];
}