#pragma once

#include <map>
#include "device.h"

class DevicesManager
{
    std::map<uint8_t , Device*> devices;

    public:

        Device* getDevice(uint8_t key);
        std::map<uint8_t , Device*>* getDevices();

        ~DevicesManager();
        DevicesManager();
};
