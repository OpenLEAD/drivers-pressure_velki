#include <iostream>
#include <pressure_velki/DriverClass5_20.hpp>
#include <base/Pressure.hpp>

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

    bool absolute = driver.isAbsolute();
    cout << "This device measures " << (absolute ? "absolute" : "relative") << " pressures" << endl;

    while (true)
    {
        cout
            << "P0=" << driver.readPressure(0).toBar() << "bar T0=" << driver.readTemperatureOfPressureSensor(0) << "C"
            << " "
            << "P1=" << driver.readPressure(1).toBar() << "bar T1=" << driver.readTemperatureOfPressureSensor(1) << "C"
            << endl;
    }

    return 0;
}
