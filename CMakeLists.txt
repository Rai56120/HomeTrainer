cmake_minimum_required(VERSION 3.10)
project(HomeTrainer VERSION 1.0 DESCRIPTION "Virtual Cycling Engine")

find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating HomeTrainer API documentation"
        VERBATIM
    )
else()
    message(STATUS "Doxygen not found; the docs target will not be available")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

enable_testing()

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
include(FetchContent)
FetchContent_Declare(
    doctest
    URL https://github.com/doctest/doctest/archive/refs/tags/v2.4.11.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_Declare(
    httplib
    URL https://github.com/yhirose/cpp-httplib/archive/refs/tags/v0.15.3.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(doctest httplib nlohmann_json)

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

set(CORE_SOURCES
    lib/Bike.cpp
    lib/Edge.cpp
    lib/Vertex.cpp
    lib/Route.cpp
    lib/User.cpp
    src/Simulation.cpp
)

add_library(HomeTrainerCore ${CORE_SOURCES})
target_include_directories(HomeTrainerCore PUBLIC
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/src
)

add_executable(HomeTrainer src/main.cpp)
target_sources(HomeTrainer PRIVATE src/ApiServer.cpp)

target_include_directories(HomeTrainer PRIVATE
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(HomeTrainer PRIVATE
    HomeTrainerCore
    Threads::Threads
    httplib::httplib
    nlohmann_json::nlohmann_json
)
target_compile_options(HomeTrainer PRIVATE -Wno-strict-overflow)
target_compile_definitions(HomeTrainer PRIVATE
    HOME_TRAINER_RESOURCE_DIR="${CMAKE_SOURCE_DIR}/resources"
)

add_executable(HomeTrainerPhysicsTests
    tests/PhysicsTests.cpp
    tests/BikeTests.cpp
    tests/VertexTests.cpp
    tests/EdgeTests.cpp
    tests/RouteTests.cpp
    tests/SimulationTests.cpp
    tests/ApiTests.cpp
    src/ApiServer.cpp
)

target_include_directories(HomeTrainerPhysicsTests PRIVATE
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(HomeTrainerPhysicsTests PRIVATE
    HomeTrainerCore
    doctest::doctest
    httplib::httplib
    nlohmann_json::nlohmann_json
)
target_compile_options(HomeTrainerPhysicsTests PRIVATE -Wno-strict-overflow)
target_compile_definitions(HomeTrainerPhysicsTests PRIVATE
    HOME_TRAINER_RESOURCE_DIR="${CMAKE_SOURCE_DIR}/resources"
    HOME_TRAINER_TEST_FIXTURE_DIR="${CMAKE_SOURCE_DIR}/tests/fixtures"
)

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(HomeTrainerPhysicsTests)