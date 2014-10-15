#include <pressure_velki/Packet.hpp>
#include <pressure_velki/DriverClass5_20.hpp>
#include <pressure_velki/Errors.hpp>
#include <base/Logging.hpp>
#include <base/Float.hpp>

using namespace pressure_velki;
using namespace std;

DriverClass5_20::DriverClass5_20()
    : iodrivers_base::Driver(Packet::MAXIMUM_PACKET_SIZE)
{
    setWriteTimeout(base::Time::fromSeconds(1));
    setReadTimeout(base::Time::fromSeconds(1));
}

DeviceInfo DriverClass5_20::initialize(int device)
{
    Packet packet(device, FUNCTION_INITIALIZE);
    writePacket(packet);
    Packet response = readResponse(device, FUNCTION_INITIALIZE, 6);
    
    DeviceInfo info;
    info.deviceClass = response[0];
    info.deviceGroup = response[1];
    info.firmwareYear  = response[2];
    info.firmwareWeek  = response[3];
    info.internalBufferSize  = response[4];
    return info;
}

int DriverClass5_20::getSerialNumber(int device)
{
    Packet packet(device, FUNCTION_SERIAL_NUMBER);
    writePacket(packet);
    Packet response = readResponse(device, FUNCTION_SERIAL_NUMBER, 4);

    return static_cast<int>(response[0]) << 24 |
        static_cast<int>(response[1]) << 16 |
        static_cast<int>(response[2]) << 8 |
        static_cast<int>(response[3]);
}

base::Pressure DriverClass5_20::readPressure(int id, int device)
{
    if (id < 0 || id > 1)
        throw std::runtime_error("invalid pressure channel selected");

    float bar = readChannel(static_cast<CHANNEL_ID>(CHANNEL_PRESSURE0 + id), device);
    return base::Pressure::fromBar(bar);
}

float DriverClass5_20::readTemperatureOfPressureSensor(int id, int device)
{
    if (id < 0 || id > 1)
        throw std::runtime_error("invalid pressure channel selected");

    return readChannel(static_cast<CHANNEL_ID>(CHANNEL_TEMPERATURE_OF_PRESSURE0 + id), device);
}


float DriverClass5_20::readChannel(CHANNEL_ID id, int device)
{
    Packet packet(device, FUNCTION_READ_CHANNEL);
    packet.addByte(id);
    writePacket(packet);
    Packet response = readResponse(device, FUNCTION_READ_CHANNEL, 5);

    float value = Packet::parseFloat(&response[0]);
    int stat = response[4];
    if (stat & 0x8) // not ready
        throw PoweringUp(device);
    else if (stat & 0x7)
    {
        LOG_WARN_S << "saturated analog input on device " << device;
        return base::unknown<float>();
    }
    else if (stat & id)
    {
        LOG_WARN_S << "measure or computation error on channel " << id << " on device " << device;
        return base::unknown<float>();
    }

    return value;
}

bool DriverClass5_20::isAbsolute(int device)
{
    Packet packet(device, FUNCTION_CONFIGURATION_READ);
    packet.addByte(14);
    writePacket(packet);
    Packet response = readResponse(device, FUNCTION_CONFIGURATION_READ, 1);
    return (response[0] != 0);
}

void DriverClass5_20::echo(int device)
{
    uint8_t payload[4] = { 0, 0, 0xE, 0x8 };
    Packet packet(device, FUNCTION_ECHO);
    packet.addBytes(payload, payload + 4);
    writePacket(packet);
    Packet response = readResponse(device, FUNCTION_ECHO, 4);

    for (int i = 0; i < 4; ++i)
    {
        if (response[i] != payload[i])
            throw std::runtime_error("communication error while performing echo");
    }
}

void DriverClass5_20::writePacket(Packet const& packet)
{
    writeBuffer.clear();
    packet.marshal(writeBuffer);
    LOG_DEBUG_S << "sending " << binary_com(&writeBuffer[0], writeBuffer.size());
    iodrivers_base::Driver::writePacket(&writeBuffer[0], writeBuffer.size());
}

Packet DriverClass5_20::readPacket()
{
    byte buffer[Packet::MAXIMUM_PACKET_SIZE];
    int packet_size = iodrivers_base::Driver::readPacket(buffer, Packet::MAXIMUM_PACKET_SIZE);
    // The Velki docs specify a silence of 1ms between reception of data and
    // sending again
    usleep(1000);
    Packet packet;
    packet.unmarshal(buffer, packet_size);
    return packet;
}

Packet DriverClass5_20::readResponse(int address, int function, int expectedSize)
{
    expectedAddress = address;
    expectedFunction = function;
    expectedPayloadSize = expectedSize;
    Packet packet = readPacket();

    if (packet.hasError())
        throw Error(address, function, packet.getErrorCode());
    return packet;
}

int DriverClass5_20::extractPacket(uint8_t const* buffer, size_t buffer_size) const
{
    LOG_DEBUG_S << "parsing " << buffer_size << " bytes: " << binary_com(buffer, buffer_size);
    // All packets have address, function and CRC on top of payload
    size_t expectedPacketSize = expectedPayloadSize + 4;
    if (buffer_size < expectedPacketSize)
        return 0;
    if (buffer[0] != expectedAddress)
    {
        LOG_DEBUG_S << "  unexpected address";
        return -1;
    }
    if ((buffer[1] & 0x7F) != expectedFunction)
    {
        LOG_DEBUG_S << "  unexpected function";
        return -1;
    }
    if (!Packet::isChecksumValid(buffer, buffer + expectedPacketSize))
    {
        LOG_DEBUG_S << "  invalid checksum";
        return -1;
    }
    return expectedPacketSize;
}

