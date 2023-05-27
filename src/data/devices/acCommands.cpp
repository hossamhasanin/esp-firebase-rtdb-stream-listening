#include "acCommands.h"
#include <string.h>

AcCommands::AcCommands(uint8_t key) : Device(key , AC , false){
    isOn = false;
    lowerTempEventCount = 0;
    riseTempEventCount = 0;
    currentCommand = AC_NONE;
}

bool AcCommands::updatedDeviceState(DeviceStateHolder stateHolder){
    // check if field name is equal to "isOn"
    if(strcmp(stateHolder.fieldName , "isOn") == 0){
        isOn = stateHolder.boolValue;
        currentCommand = isOn ? AC_ON : AC_OFF;
    } else if (strcmp(stateHolder.fieldName , "lowerTempEventCount") == 0){
        lowerTempEventCount = stateHolder.intValue;
        currentCommand = AC_LOWER_TEMP;
    } else if (strcmp(stateHolder.fieldName , "riseTempEventCount") == 0){
        riseTempEventCount = stateHolder.intValue;
        currentCommand = AC_RISE_TEMP;
    }
    return true;
}

void AcCommands::resetCommands(){
    lowerTempEventCount = 0;
    riseTempEventCount = 0;
    isOn = false;
    currentCommand = AC_NONE;
}

bool AcCommands::getIsOn(){
    return isOn;
}

uint8_t AcCommands::getLowerTempEventCount(){
    return lowerTempEventCount;
}

uint8_t AcCommands::getRiseTempEventCount(){
    return riseTempEventCount;
}

AcCommnadsToExecute AcCommands::getCurrentCommand(){
    return currentCommand;
}