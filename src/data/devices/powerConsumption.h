#include "device.h"

class PowerConsumption : public Device
{
    double powerConsumption;

    public:
        PowerConsumption(uint8_t key);

        double getPowerConsumption();
        void updatedDeviceState(DeviceStateHolder stateHolder);
};