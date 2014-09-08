#include <pressure_velki/Errors.hpp>

using namespace pressure_velki;

char const* Error::errorMessage(int function, Error::ERROR_CODE error)
{
    switch(error)
    {
        case Error::ERROR_NOT_IMPLEMENTED: return "function not implemented";
        case Error::ERROR_INCORRECT_PARAMETERS: return "incorrect parameters provided";
        case Error::ERROR_BAD_DATA: return "bad data provided";
        case Error::ERROR_DEVICE_NOT_INITIALIZED: return "device not yet initialized";
        default: return "error code without string description";
    }
}


