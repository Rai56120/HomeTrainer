import { provideHttpClient } from '@angular/common/http';
import { HttpTestingController, provideHttpClientTesting } from '@angular/common/http/testing';
import { TestBed } from '@angular/core/testing';
import { SimulationState } from '../models/simulation-state';
import { SimulationApiService } from './simulation-api.service';

const sampleState: SimulationState = {
  speedKmh: 24.5,
  powerWatts: 215,
  gradientPercent: 2.25,
  distanceKm: 1.5,
  routeDistanceKm: 12,
  running: false,
  finished: false,
};

describe('SimulationApiService', () => {
  let api: SimulationApiService;
  let http: HttpTestingController;

  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [provideHttpClient(), provideHttpClientTesting()],
    });
    api = TestBed.inject(SimulationApiService);
    http = TestBed.inject(HttpTestingController);
  });

  afterEach(() => http.verify());

  it('reads and stores the documented simulation state', () => {
    api.getState().subscribe();

    const request = http.expectOne('/api/state');
    expect(request.request.method).toBe('GET');
    request.flush(sampleState);

    expect(api.state()).toEqual(sampleState);
    expect(api.connection()).toBe('connected');
  });

  it('sends power only to the documented power-control endpoint', () => {
    api.setPower(250.5).subscribe();

    const request = http.expectOne('/api/control/power');
    expect(request.request.method).toBe('POST');
    expect(request.request.body).toEqual({ watts: 250.5 });
    request.flush({ ...sampleState, powerWatts: 250.5 });

    expect(api.state()?.powerWatts).toBe(250.5);
  });

  it.each(['start', 'pause', 'stop', 'reset'] as const)(
    'sends the %s control to its documented endpoint without a payload',
    (operation) => {
      api.control(operation).subscribe();

      const request = http.expectOne(`/api/simulation/${operation}`);
      expect(request.request.method).toBe('POST');
      expect(request.request.body).toBeNull();
      request.flush(sampleState);
    },
  );

  it('marks the API offline and propagates HTTP errors', () => {
    let receivedError = false;
    api.getState().subscribe({ error: () => (receivedError = true) });

    http.expectOne('/api/state').flush('unavailable', {
      status: 503,
      statusText: 'Service Unavailable',
    });

    expect(receivedError).toBe(true);
    expect(api.connection()).toBe('offline');
  });
});
