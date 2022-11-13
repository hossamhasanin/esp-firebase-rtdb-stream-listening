#include "dataHolder.h"
// include library for strcmp
#include <string.h>
#include <Arduino.h>

void DataHolder::parseDataFromKeyValue(const char* key, const char* value) {
    // convert key to int
    int keyInt = atoi(key);
    
    // compare the key with 0
    if (keyInt == temprature) {
        // convert the value to uint8_t
        _data[0] = (uint8_t)atoi(value);
    } else if (keyInt == switch1) {
        // convert the value to uint8_t
        _data[1] = (uint8_t)atoi(value);
    } else {
        Serial.println("Unknown key "+String(key));
    }
}