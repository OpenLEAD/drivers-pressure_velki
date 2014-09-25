#ifndef PRESSURE_VELKI_PACKET_HPP
#define PRESSURE_VELKI_PACKET_HPP

#include <boost/cstdint.hpp>
#include <vector>
#include <stdexcept>
#include <pressure_velki/Errors.hpp>

namespace pressure_velki
{
    typedef boost::uint8_t byte;

    /** Implementation of the base packet
     *
     * This is Velki's protocol, so packets have the format:
     *
     * - Byte 1: device address (with special values, see below) 
     * - Byte 2: function
     * - N Bytes: data
     * - 2 Bytes: CRC16 checksum, high-order byte then low-order byte
     */
    class Packet
    {
    public:
        static const int ADDRESS_BROADCAST = 0;
        static const int ADDRESS_POINT_TO_POINT = 250;
        static const int MAXIMUM_PAYLOAD_SIZE = 64;
        static const int MAXIMUM_PACKET_SIZE = MAXIMUM_PAYLOAD_SIZE + 4;

    private:
        bool error;
        byte address;
        byte function;
        byte payload_size;
        byte payload[MAXIMUM_PAYLOAD_SIZE];

    public:
        /** The function number for exceptions */
        static const int RESPONSE_EXCEPTION_FUNCTION = 1;

        Packet();
        Packet(byte address, byte function);

        /** Return the device address */
        byte getAddress() const;

        /** Return the requested function */
        byte getFunction() const;

        /** Returns true if the error flag is set
         */
        bool hasError() const;

        /** If this packet is an error response, returns the error code
         */
        Error::ERROR_CODE getErrorCode() const;

        /** Add a single byte to the payload */
        void addByte(byte b);

        /** Add bytes to the payload */
        void addBytes(byte const* buffer, int size);

        /** Add bytes to the payload */
        void addBytes(byte const* begin, byte const* end);

        /** Return the number of bytes in payload */
        int getPayloadSize() const;

        /** Get the n-th payload byte */
        byte const& operator [](int i) const;

        /** Get the payload data */
        byte const* getPayload() const;

        /** Marshals this packet into a ready-to-send buffer
         *
         * The marshalled packet is appended to the buffer. You can use this to
         * send multiple packets at once. Otherwise, don't forget to clear()
         * your buffer first.
         */
        void marshal(std::vector<byte>& buffer) const;

        /** Unmarshals this packet from a buffer filled with received data
         *
         * @param buffer the buffer containing the data
         * @return true if the packet was an error, false otherwise
         * @throw InvalidCRC if the buffer's CRC is invalid
         */
        bool unmarshal(byte const* buffer, int size);

        /** Tests if the checksum in the buffer is valid.
         *
         * The buffer is assumed to contain exactly one packet, whose last two
         * bytes are the checksum
         */
        static bool isChecksumValid(byte const* begin, byte const* end);
    };
}

#endif
