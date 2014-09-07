#include <boost/test/unit_test.hpp>
#include <pressure_velki/Dummy.hpp>

using namespace pressure_velki;

BOOST_AUTO_TEST_CASE(it_should_not_crash_when_welcome_is_called)
{
    pressure_velki::DummyClass dummy;
    dummy.welcome();
}
