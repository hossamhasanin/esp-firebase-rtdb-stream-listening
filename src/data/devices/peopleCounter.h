#include "device.h"

class PeopleCounter : public Device
{
    uint8_t peopleCount;

    public:
        PeopleCounter(uint8_t key);

        uint8_t getPeopleCount();
        bool updatedDeviceState(DeviceStateHolder stateHolder);
};