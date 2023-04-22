#include "devicesManager.h"
#include "switch.h"
#include "tempratureSensor.h"
#include "powerConsumption.h"
#include "peopleCounter.h"
#include "rgbLight.h"

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

    devices[doorStateId] = new Switch(doorStateId);
    devices[tempId] = new TempratureSensor(tempId);
    devices[led1Id] = new Switch(led1Id);
    devices[electriId] = new Switch(electriId);
    devices[rgblStateId] = new RgbLight(rgblStateId);
    devices[gasLeakAlarmId] = new Switch(gasLeakAlarmId);
    devices[numOfPeopleId] = new PeopleCounter(numOfPeopleId);
    devices[passwordWrongAlarmId] = new Switch(passwordWrongAlarmId);
    devices[powerConsumptionId] = new PowerConsumption(powerConsumptionId);
}

std::map<uint8_t , Device*>* DevicesManager::getDevices(){
    return &devices;
}