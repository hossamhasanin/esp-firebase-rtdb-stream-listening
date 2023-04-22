#pragma once

#include "switch.h"
#include "tempratureSensor.h"
#include "powerConsumption.h"
#include "peopleCounter.h"
#include "rgbLight.h"

#define IS_ON_FEATURE "isOn"
#define COLOR_ID_FEATURE "colorId"
#define TEMPRATURE_FEATURE "temprature"
#define PEOPLE_COUNTER_FEATURE "peopleCounter"

class UpdateDevicesCallback {
    public:
        void updateDevice(Device *device){
            if (device->getType() == SWITCH){
                updateSwitch((Switch *)device);
            } else if (device->getType() == TEMPRATURE_SENSOR){
                updateTempratureSensor((TempratureSensor *)device);
            } else if (device->getType() == POWER_CONSUMPTION){
                updatePowerConsumption((PowerConsumption *)device);
            } else if (device->getType() == PEOPLE_COUNTER){
                updatePeopleCounter((PeopleCounter *)device);
            } else if (device->getType() == RGB_LIGHT){
                updateRgbLight((RgbLight *)device);
            } else {
                throw "Device type not supported";
            }
        }
        virtual void updateSwitch(Switch *device);
        virtual void updateTempratureSensor(TempratureSensor *device);
        virtual void updatePowerConsumption(PowerConsumption *device);
        virtual void updatePeopleCounter(PeopleCounter *device);
        virtual void updateRgbLight(RgbLight *device);
};