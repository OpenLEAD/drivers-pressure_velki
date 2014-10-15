#include <boost/test/unit_test.hpp>
#include <pressure_velki/Packet.hpp>
#include <iodrivers_base/Driver.hpp>

using namespace std;
using namespace pressure_velki;

namespace boost {
    namespace test_tools {
        template<>
        struct print_log_value< vector<byte> > {
            void operator()( std::ostream& out, vector<byte> const& buffer)
            {
                out << iodrivers_base::Driver::binary_com(&buffer[0], buffer.size());
            }
        };
    }
}

BOOST_AUTO_TEST_CASE(Packet_addByte)
{
    pressure_velki::Packet packet(23, 32);
    packet.addByte(42);
    BOOST_REQUIRE_EQUAL(1, packet.getPayloadSize());
    packet.addByte(24);
    BOOST_REQUIRE_EQUAL(2, packet.getPayloadSize());
    byte const expected[] = { 42, 24 };
    BOOST_REQUIRE_EQUAL(vector<byte>(expected, expected + 2),
            vector<byte>(packet.getPayload(), packet.getPayload() + 2));
}

BOOST_AUTO_TEST_CASE(Packet_addBytes_with_pointer_and_size)
{
    pressure_velki::Packet packet(23, 32);
    byte const expected[] = { 42, 24 };
    packet.addBytes(expected, 1);
    packet.addBytes(expected + 1, 1);
    BOOST_REQUIRE_EQUAL(2, packet.getPayloadSize());
    BOOST_REQUIRE_EQUAL(vector<byte>(expected, expected + 2),
            vector<byte>(packet.getPayload(), packet.getPayload() + 2));
}

BOOST_AUTO_TEST_CASE(Packet_addBytes_with_pointer_range)
{
    pressure_velki::Packet packet(23, 32);
    byte const expected[] = { 42, 24 };
    packet.addBytes(expected, expected + 1);
    packet.addBytes(expected + 1, expected + 2);
    BOOST_REQUIRE_EQUAL(2, packet.getPayloadSize());
    BOOST_REQUIRE_EQUAL(vector<byte>(expected, expected + 2),
            vector<byte>(packet.getPayload(), packet.getPayload() + 2));
}

BOOST_AUTO_TEST_CASE(Packet_bracket_operator)
{
    pressure_velki::Packet packet(23, 32);
    byte const expected[] = { 42, 24 };
    packet.addBytes(expected, 2);
    BOOST_REQUIRE_EQUAL(42, packet[0]);
    BOOST_REQUIRE_EQUAL(24, packet[1]);
}

BOOST_AUTO_TEST_CASE(Packet_getPayloadSize_on_empty_packet)
{
    pressure_velki::Packet packet(23, 32);
    BOOST_REQUIRE_EQUAL(0, packet.getPayloadSize());
}


BOOST_AUTO_TEST_CASE(marshal_and_unmarshal_should_be_identity)
{
    pressure_velki::Packet packet(23, 32);
    vector<byte> buffer;
    packet.marshal(buffer);

    Packet result(0, 0);
    result.unmarshal(&buffer[0], buffer.size());
}

BOOST_AUTO_TEST_CASE(Packet_crc_calculation)
{
    // Velki provides expected CRC values for the initialize packet. Verify that
    // we match
    pressure_velki::Packet packet(250, 48);
    vector<byte> buffer;
    packet.marshal(buffer);

    BOOST_CHECK_EQUAL(4, buffer[buffer.size() - 2]);
    BOOST_CHECK_EQUAL(67, buffer[buffer.size() - 1]);
}

BOOST_AUTO_TEST_CASE(Packet_isChecksumValid)
{
    pressure_velki::Packet packet(250, 48);
    vector<byte> buffer;
    packet.marshal(buffer);
    BOOST_REQUIRE( Packet::isChecksumValid(&buffer[0], &buffer[buffer.size()]) );
}

BOOST_AUTO_TEST_CASE(Packet_parseFloat)
{
    {
        byte marshalled[4] = { 63, 109, 186, 172 };
        float result = Packet::parseFloat(marshalled);
        BOOST_CHECK_CLOSE(0.9286296f, result, 0.0001);
    }
    {
        byte marshalled[4] = { 63, 109, 177, 83 };
        float result = Packet::parseFloat(marshalled);
        BOOST_CHECK_CLOSE(0.9284870f, result, 0.0001);
    }
    {
        byte marshalled[4] = { 63, 109, 178, 242 };
        float result = Packet::parseFloat(marshalled);
        BOOST_CHECK_CLOSE(0.9285117f, result, 0.0001);
    }
    {
        byte marshalled[4] = { 65, 201, 184, 0 };
        float result = Packet::parseFloat(marshalled);
        BOOST_CHECK_CLOSE(25.21484f, result, 0.0001);
    }
    {
        byte marshalled[4] = { 65, 202, 81, 128 };
        float result = Packet::parseFloat(marshalled);
        BOOST_CHECK_CLOSE(25.28979f, result, 0.0001);
    }

}
