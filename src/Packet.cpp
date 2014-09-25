#include <pressure_velki/Packet.hpp>
#include <boost/crc.hpp>
#include <pressure_velki/Errors.hpp>
#include <string.h>

using namespace pressure_velki;
using namespace std;

// I've tried to use boost's CRC implementation, but could not manage to
// configure it to match the CRC expected by Velki. This is the CRC calculation
// from the Velki docs.
static boost::uint16_t crc(byte const* begin, byte const* end)
{
    unsigned int crc = 0xFFFF;
    
    for (byte const* it = begin; it != end; ++it)
    {
        crc ^= *it;
        for (int n = 0; n < 8; ++n)
        {
            if (crc & 0x1)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }

    return crc;
}


Packet::Packet()
    : address(0)
    , function(0)
    , payload_size(0)
{
}

Packet::Packet(byte address, byte function)
    : address(address)
    , function(function)
    , payload_size(0)
{
}

byte Packet::getAddress() const
{
    return address;
}

byte Packet::getFunction() const
{
    return function;
}

byte const* Packet::getPayload() const
{
    return payload;
}

int Packet::getPayloadSize() const
{
    return payload_size;
}

byte const& Packet::operator [](int i) const
{
    return payload[i];
}

bool Packet::hasError() const
{
    return error;
}

Error::ERROR_CODE Packet::getErrorCode() const
{
    return static_cast<Error::ERROR_CODE>(payload[0]);
}

void Packet::addByte(byte b)
{
    return addBytes(&b, 1);
}

void Packet::addBytes(byte const* begin, byte const* end)
{
    return addBytes(begin, end - begin);
}

void Packet::addBytes(byte const* buffer, int size)
{
    if (payload_size + size > MAXIMUM_PAYLOAD_SIZE)
        throw std::logic_error("adding bytes to an already full packet");
    memcpy(payload + payload_size, buffer, size);
    payload_size += size;
}

void Packet::marshal(vector<byte>& buffer) const
{
    buffer.push_back(address);
    buffer.push_back(function);
    buffer.insert(buffer.end(), payload, payload + payload_size);
    byte* crc_begin = &buffer[buffer.size() - 2 - payload_size];
    
    boost::uint16_t checksum = crc(crc_begin, &buffer[buffer.size()]);
    buffer.push_back(checksum >> 8);
    buffer.push_back(checksum & 0xFF);
}

bool Packet::isChecksumValid(byte const* begin, byte const* end)
{
    boost::uint16_t expected_checksum = crc(begin, end - 2);
    boost::uint16_t actual_checksum =
        static_cast<boost::uint16_t>(*(end - 2)) << 8 |
        static_cast<boost::uint16_t>(*(end - 1));
    return actual_checksum == expected_checksum;
}

bool Packet::unmarshal(byte const* buffer, int size)
{
    if (!Packet::isChecksumValid(buffer, buffer + size))
        throw InvalidCRC();

    address  = buffer[0];
    function = buffer[1] & 0x7F;
    error    = (buffer[1] >> 15) == 1;

    payload_size = size - 4;
    memcpy(payload, buffer + 2, payload_size);
    return error;
}

