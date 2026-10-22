# Backend

## Run with Docker

From this directory, build the backend image and start a container:

```sh
docker build -t hometrainer-backend .
docker run --rm -p 127.0.0.1:8080:8080 hometrainer-backend
```

The API is available locally at `http://127.0.0.1:8080`. Check it with:

```sh
curl http://127.0.0.1:8080/api/health
```

To use a different host port, change the first port in the `-p` option. For
example, publish the API on port `8081`:

```sh
docker run --rm -p 127.0.0.1:8081:8080 hometrainer-backend
```

The container listens on port `8080` on its network interface. The executable
defaults to binding to `127.0.0.1` when run outside Docker; the image sets
`HOME_TRAINER_HOST=0.0.0.0` so its API is reachable through the published
container port.
