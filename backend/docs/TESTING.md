# Test Guide

## Overview

The project uses doctest. CMake builds the test executable
`HomeTrainerPhysicsTests` and registers its individual doctest cases with
CTest. Although the executable name says "Physics", it contains model, route,
simulation, and HTTP API tests as well. The current suite contains 19 test
cases across seven source files.

## Build and Run

From the repository root, configure, build, and run the suite with:

```sh
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

The first configure may need network access to fetch doctest, cpp-httplib, and
nlohmann/json if they are not already available in the build directory.

To list the registered cases without running them:

```sh
ctest --test-dir build -N
```

The test executable can also be run directly to use doctest filters:

```sh
build/HomeTrainerPhysicsTests.exe --test-case="Edge *"
```

## Test Coverage

| Source file | What it verifies |
| --- | --- |
| `tests/PhysicsTests.cpp` | Finite speed at representative power/gradient combinations; uphill speed is lower than flat speed; zero-power descent and level-ground behavior; no-bike behavior; bundled GPX route geometry and distance consistency. |
| `tests/BikeTests.cpp` | Bike physical parameters are retained, and headwind defaults to zero. |
| `tests/VertexTests.cpp` | Vertex ID and geographic coordinates are exposed as stored. |
| `tests/EdgeTests.cpp` | Geographic segment length, ascent/descent gradient signs, and zero-length edge handling. |
| `tests/RouteTests.cpp` | Missing, empty, and one-point GPX inputs yield empty routes; a two-point GPX creates one finite, flat segment with consistent total distance. |
| `tests/SimulationTests.cpp` | Fixed-time ticks advance distance; route completion clamps traveled distance and marks the simulation finished. |
| `tests/ApiTests.cpp` | Stable state JSON fields; power command parsing and validation; HTTP health and state requests; successful and invalid power-control requests. |

## Test Data and Runtime Details

Route and simulation tests use files under `tests/fixtures/`:

- `empty.gpx` and `one-point.gpx` exercise degenerate route inputs.
- `flat-two-points.gpx` provides a short, flat route for route, simulation,
  and API tests.
- The physics suite also loads `resources/ninian_bourg.gpx` to validate the
  bundled route.

CMake supplies the fixture and resource directory paths to the test target as
compile definitions. The API integration test starts a local HTTP server on
`127.0.0.1:18080`; ensure that port is available when running the suite.

## Coverage Boundaries

The current API tests do not exercise the simulation `start`, `pause`, `stop`,
or `reset` HTTP endpoints. The suite also has no dedicated `UserTests.cpp`;
user speed calculations are covered indirectly by `PhysicsTests.cpp`.