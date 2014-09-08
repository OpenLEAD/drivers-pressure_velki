#ifndef PRESSURE_VELKI_DEVICE_INFO_HPP
#define PRESSURE_VELKI_DEVICE_INFO_HPP

namespace pressure_velki
{
    struct DeviceInfo
    {
        int deviceClass;
        int deviceGroup;
        int firmwareYear;
        int firmwareWeek;
        int internalBufferSize;
    };
}

#endif

