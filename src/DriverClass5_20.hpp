#ifndef PRESSURE_VELKI_DRIVER_CLASS5_20_HPP
#define PRESSURE_VELKI_DRIVER_CLASS5_20_HPP

#include <boost/cstdint.hpp>
#include <iodrivers_base/Driver.hpp>
#include <pressure_velki/Packet.hpp>
#include <pressure_velki/DeviceInfo.hpp>

namespace pressure_velki
{
    class DriverClass5_20 : public iodrivers_base::Driver
    {
        enum FUNCTIONS
        {
            FUNCTION_ECHO = 8,
            FUNCTION_INITIALIZE = 48,
            FUNCTION_SERIAL_NUMBER = 69,
            FUNCTION_READ_CHANNEL = 73
        };

        enum CHANNEL_ID
        {
            CHANNEL_CALCULATED,
            CHANNEL_PRESSURE0,
            CHANNEL_PRESSURE1,
            CHANNEL_TEMPERATURE,
            CHANNEL_TEMPERATURE_OF_PRESSURE0,
            CHANNEL_TEMPERATURE_OF_PRESSURE1
        };

    public:
        DriverClass5_20();

        /** Initialize the given device, and wait for the reply
         *
         * @param device the device ID. You can use the special value
         *   Packet::ADDRESS_POINT_TO_POINT if there is only one device
         *   connected
         * @return true if the device was not initialized already and false
         *   otherwise. Note that it is harmless to initialize a device more
         *   than once
         */
        DeviceInfo initialize(int device = Packet::ADDRESS_POINT_TO_POINT);

        /** Query and return the device's serial number
         *
         * This can be used as some form of diagnostics
         */
        int getSerialNumber(int device = Packet::ADDRESS_POINT_TO_POINT);

        /** Send an echo to the given device and wait for the response */
        void echo(int device = Packet::ADDRESS_POINT_TO_POINT);

        /** Read one pressure channel
         *
         * @param the pressure channel to read (either 0 or 1)
         * @return the pressure in bar
         */
        float readPressure(int id, int device = Packet::ADDRESS_POINT_TO_POINT);

        /** Read the temperature of one pressure sensor
         *
         * @param the pressure sensor whose temperature should be read (either 0 or 1)
         * @return the temperature in celsius
         */
        float readTemperatureOfPressureSensor(int id, int device = Packet::ADDRESS_POINT_TO_POINT);

    protected:
        std::vector<byte> writeBuffer;

        /** Read one channel */
        float readChannel(CHANNEL_ID channel, int device);

        /** Write one packet */
        void writePacket(Packet const& packet);

        /** Reads one packet */
        Packet readPacket();

        /** Information about the packet that should be expected during the next
         * response read.
         *
         * @see readResponse
         */
        int expectedAddress;
        int expectedFunction;
        int expectedPayloadSize;

        /** Read the response to a request
         *
         * @param address the device address. The response will have to come
         *   from the same device
         * @param function the request's function. The response should refer to
         *   the same function
         * @param expectedSize the expected size of the payload
         */
        Packet readResponse(int address, int function, int expectedSize);


        /** Packet extraction routine used by iodrivers_base::Driver */
        int extractPacket(boost::uint8_t const* buffer, size_t buffer_size) const;
    };
}

#endif

