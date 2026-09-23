cmake_minimum_required(VERSION 3.10)
project(HomeTrainer VERSION 1.0 DESCRIPTION "Virtual Cycling Engine")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Common warnings shared by all configs
set(WARNINGS "-pedantic -Wall -Wextra \
    -Wcast-align -Wcast-qual -Wctor-dtor-privacy \
    -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op \
    -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept \
    -Wold-style-cast -Woverloaded-virtual -Wredundant-decls \
    -Wshadow -Wsign-conversion -Wsign-promo \
    -Wstrict-null-sentinel -Wstrict-overflow=5 \
    -Wswitch-default -Wundef -Wno-unused"
)

# Debug: no -O2, no -Werror, symbols on
set(CMAKE_CXX_FLAGS_DEBUG "${WARNINGS} -g -O0")

# Release: warnings become errors, optimizations on
set(CMAKE_CXX_FLAGS_RELEASE "${WARNINGS} -O2 -Werror")

find_package(Threads REQUIRED)

set(SOURCES
    src/main.cpp
    lib/Bike.cpp
    lib/Edge.cpp
    lib/Vertex.cpp
    lib/Route.cpp
    lib/User.cpp
)

add_executable(HomeTrainer ${SOURCES})

target_include_directories(HomeTrainer PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_link_libraries(HomeTrainer PRIVATE Threads::Threads)