rock_library(pressure_velki
    SOURCES Dummy.cpp
    HEADERS Dummy.hpp)

rock_executable(pressure_velki_bin Main.cpp
    DEPS pressure_velki)

