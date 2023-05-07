#include "device.h"

enum AcCommnadsToExecute {
    AC_NONE = 0,
    AC_ON = 1,
    AC_OFF = 2,
    AC_LOWER_TEMP = 3,
    AC_RISE_TEMP = 4
};

class AcCommands : public Device{
    bool isOn;
    uint8_t lowerTempEventCount;
    uint8_t riseTempEventCount;
    AcCommnadsToExecute currentCommand; 

    public:
        AcCommands(uint8_t key);

        AcCommnadsToExecute getCurrentCommand();
        bool getIsOn();
        uint8_t getLowerTempEventCount();
        uint8_t getRiseTempEventCount();
        void resetCommands();
        void updatedDeviceState(DeviceStateHolder stateHolder);
};