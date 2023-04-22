#include "device.h"

class Switch : public Device
{
    bool state;
    
    public:
        Switch(uint8_t key);

        bool getState();
        void updatedDeviceState(DeviceStateHolder stateHolder);
};