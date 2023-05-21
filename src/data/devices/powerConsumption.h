#include "device.h"

class PowerConsumption : public Device
{
    double powerConsumption;

    public:
        PowerConsumption(uint8_t key);

        double getPowerConsumption();
        bool updatedDeviceState(DeviceStateHolder stateHolder);
};