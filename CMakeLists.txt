cmake_minimum_required(VERSION 3.10)
project(HomeTrainer VERSION 1.0 DESCRIPTION "Virtual Cycling Engine")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused -02")

find_package(Threads REQUIRED)

set(SOURCES
    src/main.cpp
    lib/Bike.cpp
    lib/Edge.cpp
    lib/Vertex.cpp
    lib/Route.cpp
    lib/User.cpp
)

add_executable(HomeTrainer.exe ${SOURCES})

target_include_directories(HomeTrainer PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_link_libraries(HomeTrainer PRIVATE Threads::Threads)