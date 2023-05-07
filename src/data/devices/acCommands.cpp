#include "acCommands.h"

AcCommands::AcCommands(uint8_t key) : Device(key , AC){
    isOn = false;
    lowerTempEventCount = 0;
    riseTempEventCount = 0;
    currentCommand = AC_OFF;
}

void AcCommands::updatedDeviceState(DeviceStateHolder stateHolder){
    // check if field name is equal to "isOn"
    if(stateHolder.fieldName == "isOn"){
        isOn = stateHolder.boolValue;
        currentCommand = isOn ? AC_ON : AC_OFF;
    } else if (stateHolder.fieldName == "lowerTempEventCount"){
        lowerTempEventCount = stateHolder.intValue;
        currentCommand = AC_LOWER_TEMP;
    } else if (stateHolder.fieldName == "riseTempEventCount"){
        riseTempEventCount = stateHolder.intValue;
        currentCommand = AC_RISE_TEMP;
    }
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