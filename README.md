# HomeTrainer

HomeTrainer is a cycling simulation with a C++ HTTP API and an Angular web
frontend.

## Run the full application with Docker

Install Docker with the Compose plugin, then run the following from the
repository root:

```sh
docker compose up --build
```

Open [http://127.0.0.1:4200](http://127.0.0.1:4200). The Angular app is served
by Nginx, which also proxies `/api` requests to the C++ backend over the
Compose network. The backend API is also available directly from the host at
[http://127.0.0.1:8080](http://127.0.0.1:8080), for example:

```sh
curl http://127.0.0.1:8080/api/health
```

Both services are published only on the host's loopback interface, so they
are available to local applications but not directly to other machines on
your network. The backend port is configurable with `BACKEND_PORT`:

```sh
BACKEND_PORT=8081 docker compose up --build
```

On PowerShell:

```powershell
$env:BACKEND_PORT = "8081"
docker compose up --build
```

The frontend port is configurable:

```sh
FRONTEND_PORT=4300 docker compose up --build
```

On PowerShell:

```powershell
$env:FRONTEND_PORT = "4300"
docker compose up --build
```

Stop the services with `docker compose down`.

## Run services individually

To run only the backend in a container, build its image and publish its API
port:

```sh
docker build -t hometrainer-backend ./backend
docker run --rm -p 127.0.0.1:8080:8080 hometrainer-backend
```

Its API is available at `http://127.0.0.1:8080`. The root Compose file remains
the way to launch the frontend and backend together.

For local development, start the backend first and then the Angular dev server.
See [frontend/README.md](frontend/README.md) for frontend setup and API details.

## Build the backend locally

From the `backend` directory, configure and build with CMake:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Run the backend test suite with:

```sh
ctest --test-dir build --output-on-failure
```

See [backend/docs/API-ICD.md](backend/docs/API-ICD.md) for the HTTP API
contract and [backend/docs/TESTING.md](backend/docs/TESTING.md) for more about
the backend test suite. To generate the API reference with Doxygen, run this
from the repository root when Doxygen is installed:

```
cd backend
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target docs
```

## Sources

[gpx documentation](https://www.topografix.com/GPX/1/1/gpx.xsd)

[GPX Studio](https://gpx.studio/app#15.23/47.998664/-2.489531)

[Bike power to speed calculation](https://www.gribble.org/cycling/power_v_speed.html)