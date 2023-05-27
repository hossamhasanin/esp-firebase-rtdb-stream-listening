#include "devicesManager.h"
#include "switch.h"
#include "tempratureSensor.h"
#include "powerConsumption.h"
#include "peopleCounter.h"
#include "rgbLight.h"
#include "acCommands.h"

Device* DevicesManager::getDevice(uint8_t key){
    return devices[key];
}

DevicesManager::~DevicesManager(){
    for (auto const& x : devices)
    {
        delete x.second;
    }
}

DevicesManager::DevicesManager(){

    devices[doorStateId] = new Switch(doorStateId , false , false);
    devices[tempId] = new TempratureSensor(tempId);
    devices[led1Id] = new Switch(led1Id);
    devices[electriId] = new Switch(electriId);
    devices[rgblStateId] = new RgbLight(rgblStateId);
    devices[gasLeakAlarmId] = new Switch(gasLeakAlarmId , false , false);
    devices[numOfPeopleId] = new PeopleCounter(numOfPeopleId);
    devices[acCommands] = new AcCommands(acCommands);
    devices[passwordWrongAlarmId] = new Switch(passwordWrongAlarmId , false , false);
    devices[powerConsumptionId] = new PowerConsumption(powerConsumptionId);
}

std::map<uint8_t , Device*>* DevicesManager::getDevices(){
    return &devices;
}