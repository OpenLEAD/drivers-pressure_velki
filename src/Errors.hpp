#ifndef PRESSURE_VELKI_ERRORS_HPP
#define PRESSURE_VELKI_ERRORS_HPP

#include <stdexcept>

namespace pressure_velki
{
    /** Representation of exception responses */
    struct Error : public std::runtime_error
    {
        enum ERROR_CODE
        {
            ERROR_NOT_IMPLEMENTED = 1,
            ERROR_INCORRECT_PARAMETERS = 2,
            ERROR_BAD_DATA = 3,
            ERROR_DEVICE_NOT_INITIALIZED = 32
        };

        int device;
        int function;
        ERROR_CODE error;

        Error(int device, int function, ERROR_CODE error)
            : std::runtime_error(Error::errorMessage(function, error))
            , device(device)
            , function(function)
            , error(error)
        {}

        static char const* errorMessage(int function, ERROR_CODE error);
    };

    struct DeviceNotInitialized : public Error
    {
        DeviceNotInitialized(int device, int function)
            : Error(device, function, ERROR_DEVICE_NOT_INITIALIZED) {}
    };

    struct InvalidCRC : public std::runtime_error
    {
        InvalidCRC()
            : std::runtime_error("invalid CRC in received message") {}
    };

    struct PoweringUp : public std::runtime_error
    {
        int device;

        PoweringUp(int device)
            : std::runtime_error("attempted to use the device while it was powering up")
            , device(device)
        {}
    };
}

#endif

