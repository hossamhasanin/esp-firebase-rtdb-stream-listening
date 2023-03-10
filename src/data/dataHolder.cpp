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
    
    if (valueByte == get(key)) {
        changedDataItem->key = -1;
        changedDataItem->value = -1;
        return;
    }

    set(key, valueByte);
    changedDataItem->key = key;
    changedDataItem->value = get(key);
}

uint8_t DataHolder::get(int key) {
    if (key == doorState){
        return _data[DOOR_STATE_INDEX];
    } else if (key == temp){
        return _data[TEMP_INDEX];
    } else if (key == led1){
        return _data[LED1_INDEX];
    } else if (key == electri){
        return _data[ELECTRI_INDEX];
    } else if (key == rgblState){
        return _data[RGBL_STATE_INDEX];
    } else if (key == gasLeakAlarm){
        return _data[GAS_LEAK_ALARM_INDEX];
    } else if (key == numOfPeople){
        return _data[NUM_OF_PEOPLE_INDEX];
    } else if (key == powerConsumption){
        return _data[POWER_CONSUMPTION_INDEX];
    } else if (key == passwordWrongAlarm){
        return _data[PASSWORD_WRONG_ALARM_INDEX];
    } else {
        throw "Unknown key";
    }
}

DataItem DataHolder::getDataItem(int key) {
    DataItem dataItem;
    dataItem.key = key;
    dataItem.value = get(key);
    return dataItem;
}

void DataHolder::set(int key , uint8_t value) {
    if (key == doorState){
        _data[DOOR_STATE_INDEX] = value;
        Serial.println("saved doorState to memory: "+String(value));
    } else if (key == temp){
        _data[TEMP_INDEX] = value;
        Serial.println("saved temp to memory: "+String(value));
    } else if (key == led1){
        _data[LED1_INDEX] = value;
        Serial.println("saved led1 to memory: "+String(value));
    } else if (key == electri){
        _data[ELECTRI_INDEX] = value;
        Serial.println("saved electri to memory: "+String(value));
    } else if (key == rgblState){
        _data[RGBL_STATE_INDEX] = value;
        Serial.println("saved rgblState to memory: "+String(value));
    } else if (key == gasLeakAlarm){
        _data[GAS_LEAK_ALARM_INDEX] = value;
        Serial.println("saved gasLeakAlarm to memory: "+String(value));
    } else if (key == numOfPeople){
        _data[NUM_OF_PEOPLE_INDEX] = value;
        Serial.println("saved numOfPeople to memory: "+String(value));
    } else if (key == powerConsumption){
        _data[POWER_CONSUMPTION_INDEX] = value;
        Serial.println("saved powerConsumption to memory: "+String(value));
    } else if (key == passwordWrongAlarm){
        _data[PASSWORD_WRONG_ALARM_INDEX] = value;
        Serial.println("saved passwordWrongAlarm to memory: "+String(value));
    } else {
        throw "Unknown key";
    }
}