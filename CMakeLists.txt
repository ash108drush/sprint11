cmake_minimum_required(VERSION 3.16)

project(sprint11 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sprint11 main.cpp
    domain.cpp domain.h geo.cpp geo.h json.cpp json.h json_reader.cpp json_reader.h map_renderer.cpp map_renderer.h request_handler.cpp request_handler.h svg.cpp svg.h transport_catalogue.cpp transport_catalogue.h)

include(GNUInstallDirs)
install(TARGETS sprint11
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
