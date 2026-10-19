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
| Apply rider power                                     | `POST /api/control/power`                       |
| Read live speed, gradient, distance, power and status | `GET /api/state`                                |
| Start, pause, stop or reset the ride                  | `POST /api/simulation/{start,pause,stop,reset}` |

The current API does not accept a GPX upload or bike/environment configuration.
The GPX picker only displays the chosen local filename, and those parameter
fields remain local to the setup form; neither is sent to a placeholder route
or applied to the simulation. The backend continues to use its configured
route and simulation parameters until the API contract adds support.

## Build and tests

```powershell
npm run build
npm test -- --watch=false
```
