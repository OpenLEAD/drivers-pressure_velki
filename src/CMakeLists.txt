rock_library(pressure_velki
    SOURCES Errors.cpp Packet.cpp DriverClass5_20.cpp
    HEADERS Errors.hpp Packet.hpp DriverClass5_20.hpp DeviceInfo.hpp
    DEPS_PKGCONFIG iodrivers_base base-lib)

rock_executable(pressure_velki_read
    SOURCES Main.cpp
    DEPS pressure_velki)
