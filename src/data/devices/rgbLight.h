#include "device.h"

class RgbLight : public Device{
    bool isOn;
    uint8_t colorId;

    public:
        RgbLight(uint8_t key);

        bool getIsOn();
        uint8_t getColorId();
        bool updatedDeviceState(DeviceStateHolder stateHolder);
};