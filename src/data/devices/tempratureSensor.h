#include "device.h"

class TempratureSensor : public Device
{
    uint8_t temprature;

    public:
        TempratureSensor(uint8_t key);

        uint8_t getTemprature();
        bool updatedDeviceState(DeviceStateHolder stateHolder);
};