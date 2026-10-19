# HomeTrainer Interface Control Document

## 1. Purpose and Scope

This document defines the HTTP interface for reading and controlling the
HomeTrainer cycling simulation. It describes the interface implemented by the
current executable; it does not define the internal simulation model or the
GPX file format.

## 2. Connection

| Property | Value |
| --- | --- |
| Protocol | HTTP over TCP |
| Default address | `http://127.0.0.1:8080` |
| Payload format | JSON, UTF-8 |
| Success response content type | `application/json` |
| Authentication | None |

The executable binds to loopback (`127.0.0.1`) on port `8080`; it is not
reachable from other machines by default. For requests with a body, clients
should send `Content-Type: application/json`.

## 3. Common Response Data

Simulation state is returned by `GET /api/state` and by every successful
simulation-control request. The response is a JSON object with these fields:

| Field | Type | Unit / meaning |
| --- | --- | --- |
| `speedKmh` | number | Current speed in kilometers per hour |
| `powerWatts` | number | Requested rider power in watts |
| `gradientPercent` | number | Current route gradient as a percentage |
| `distanceKm` | number | Distance traveled along the route, in kilometers |
| `routeDistanceKm` | number | Total route length, in kilometers |
| `running` | boolean | True while simulation updates are active |
| `finished` | boolean | True after the route is complete |

Example:

```json
{
  "speedKmh": 25.0,
  "powerWatts": 250.0,
  "gradientPercent": 2.5,
  "distanceKm": 1.2,
  "routeDistanceKm": 10.0,
  "running": true,
  "finished": false
}
```

## 4. Endpoints

### `GET /api/health`

Checks that the HTTP API is responding.

**Success:** `200 OK`

```json
{
  "status": "ok",
  "apiVersion": 1
}
```

### `GET /api/state`

Returns the current simulation state using the common response object in
Section 3.

**Success:** `200 OK`

### `POST /api/control/power`

Sets rider power. The request body must be a JSON object with a finite,
non-negative numeric `watts` property.

**Request:**

```json
{
  "watts": 250
}
```

**Success:** `200 OK`, followed by the updated simulation state.

**Invalid request:** `400 Bad Request`

```json
{
  "error": "watts must be a finite non-negative number"
}
```

The error message is specific to the validation failure. Invalid JSON returns
`{"error":"request body must be valid JSON"}`; a missing, non-numeric, or
non-object `watts` value returns `{"error":"watts must be a number"}`.

### Simulation controls

Each operation is a `POST` request with no required body. Each returns
`200 OK` and the resulting simulation state.

| Endpoint | Effect |
| --- | --- |
| `POST /api/simulation/start` | Starts or resumes simulation updates unless the route is finished. |
| `POST /api/simulation/pause` | Pauses updates; the simulation can be resumed with `start`. |
| `POST /api/simulation/stop` | Requests the simulation worker to exit. This is not equivalent to `pause`; do not rely on `start` resuming after `stop`. |
| `POST /api/simulation/reset` | Restores the initial rider and route position and pauses the simulation. |

## 5. Error Handling

Power-command validation failures use HTTP `400` and a JSON object containing
an `error` string. Successful requests use HTTP `200`. The server does not
define a common JSON error schema for other HTTP failures; clients should
handle non-2xx responses and unknown routes without assuming a response body
shape.

## 6. Request Examples

```sh
curl http://127.0.0.1:8080/api/health
curl http://127.0.0.1:8080/api/state
curl -X POST http://127.0.0.1:8080/api/control/power \
  -H "Content-Type: application/json" \
  -d '{"watts":250}'
curl -X POST http://127.0.0.1:8080/api/simulation/pause
curl -X POST http://127.0.0.1:8080/api/simulation/start
```