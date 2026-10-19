# HomeTrainer frontend

An Angular 21 standalone application for configuring and monitoring a HomeTrainer
ride.

## Run locally

Start the C++ API from the `backend` project so it listens on
`http://127.0.0.1:8080`, then run the frontend:

```powershell
cd frontend
npm ci
npm start
```

Open `http://localhost:4200`. The Angular development server proxies `/api`
requests to the local C++ API; no separate CORS configuration is needed.

## API integration

The frontend calls only endpoints documented in
[`backend/docs/API-ICD.md`](../backend/docs/API-ICD.md):

| UI action or data                                     | API request                                     |
| ----------------------------------------------------- | ----------------------------------------------- |
| Upload and activate a GPX route                      | `POST /api/route`                               |
| Apply rider profile                                  | `POST /api/config/user`                         |
| Apply bike and environment parameters                | `POST /api/config/bike`                         |
| Apply ride power                                     | `POST /api/control/power`                       |
| Read live speed, gradient, distance, power and status | `GET /api/state`                                |
| Display active ride elapsed time                     | Measured locally while API state is running    |
| Start, pause, stop or reset the ride                  | `POST /api/simulation/{start,pause,stop,reset}` |

The simulation remains paused until the frontend sends the `start` request.
GPX uploads are limited to 5 MiB. Rider and bike configuration changes are
applied immediately and persist through a simulation reset.

## Build and tests

```powershell
npm run build
npm test -- --watch=false
```
