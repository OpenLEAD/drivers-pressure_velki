#include <iostream>
#include <pressure_velki/DriverClass5_20.hpp>

using namespace pressure_velki;
using namespace std;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        cerr << "usage: " << argv[0] << " DEVICE" << endl;
        return 1;
    }

    DriverClass5_20 driver;
    driver.openURI(argv[1]);
    DeviceInfo info = driver.initialize();
    cout << "Device: class=" << info.deviceClass << ", group=" << info.deviceGroup << "\n" <<
        "Firmware: year=" << info.firmwareYear << ", week=" << info.firmwareWeek << "\n" <<
        "Internal buffer size: " << info.internalBufferSize << " bytes" << endl;

    while (true)
    {
        cout
            << "P0=" << driver.readPressure(0) << " T0=" << driver.readTemperatureOfPressureSensor(0)
            << "P1=" << driver.readPressure(1) << " T1=" << driver.readTemperatureOfPressureSensor(1)
            << endl;
    }

    return 0;
}
