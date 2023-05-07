#pragma once
#include <stdint.h>

struct DeviceStateHolder
{
    const char* fieldName;
    uint8_t intValue;
    double doubleValue;
    bool boolValue;
};
