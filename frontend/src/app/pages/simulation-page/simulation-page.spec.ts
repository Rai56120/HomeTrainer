import { HttpErrorResponse } from '@angular/common/http';
import { TestBed } from '@angular/core/testing';
import { signal } from '@angular/core';
import { provideRouter } from '@angular/router';
import { of, throwError } from 'rxjs';
import { SimulationState } from '../../models/simulation-state';
import { SimulationControl, SimulationApiService } from '../../services/simulation-api.service';
import { SimulationPage } from './simulation-page';

const initialState: SimulationState = {
  speedKmh: 26.4,
  powerWatts: 210,
  gradientPercent: 3.2,
  distanceKm: 2.5,
  routeDistanceKm: 10,
  running: false,
  finished: false,
};

function createApiMock(initial: SimulationState | null = initialState) {
  const state = signal<SimulationState | null>(initial);
  return {
    state,
    getState: vi.fn(() => {
      const current = state();
      return current ? of(current) : throwError(() => new HttpErrorResponse({ status: 0 }));
    }),
    control: vi.fn((operation: SimulationControl) => {
      const current = state();
      if (!current) {
        return throwError(() => new HttpErrorResponse({ status: 0 }));
      }
      const next: SimulationState = {
        ...current,
        distanceKm: operation === 'reset' ? 0 : current.distanceKm,
        powerWatts: operation === 'reset' ? 0 : current.powerWatts,
        running: operation === 'start',
        finished: operation === 'reset' ? false : current.finished,
      };
      state.set(next);
      return of(next);
    }),
  };
}

describe('SimulationPage', () => {
  it('shows live state, a bounded progress marker, and remaining distance', async () => {
    const api = createApiMock();
    await TestBed.configureTestingModule({
      imports: [SimulationPage],
      providers: [provideRouter([]), { provide: SimulationApiService, useValue: api }],
    }).compileComponents();
    const fixture = TestBed.createComponent(SimulationPage);
    fixture.detectChanges();

    const compiled = fixture.nativeElement as HTMLElement;
    expect(api.getState).toHaveBeenCalled();
    expect(compiled.querySelector('[role="progressbar"]')?.getAttribute('aria-valuenow')).toBe(
      '25',
    );
    expect(compiled.textContent).toContain('26.4');
    expect(compiled.textContent).toContain('3.2');
    expect(compiled.textContent).toContain('2.50');
    expect(compiled.textContent).toContain('7.50');
    expect(compiled.textContent).toContain('210');
    fixture.destroy();
  });

  it('wires start, pause, stop, and reset to the documented controls', async () => {
    const api = createApiMock();
    await TestBed.configureTestingModule({
      imports: [SimulationPage],
      providers: [provideRouter([]), { provide: SimulationApiService, useValue: api }],
    }).compileComponents();
    const fixture = TestBed.createComponent(SimulationPage);
    fixture.detectChanges();
    const button = (selector: string) =>
      fixture.nativeElement.querySelector(selector) as HTMLButtonElement;

    button('.button-start').click();
    expect(api.control).toHaveBeenLastCalledWith('start');
    fixture.detectChanges();
    button('.button-quiet').click();
    expect(api.control).toHaveBeenLastCalledWith('pause');
    fixture.detectChanges();
    button('.control-buttons button:nth-child(3)').click();
    expect(api.control).toHaveBeenLastCalledWith('stop');
    fixture.detectChanges();
    expect(button('.button-start').disabled).toBe(true);
    expect(fixture.nativeElement.textContent).toContain('The API stop operation exits its worker.');
    button('.button-reset').click();
    expect(api.control).toHaveBeenLastCalledWith('reset');
    fixture.detectChanges();
    expect(button('.button-start').disabled).toBe(true);
    expect(fixture.nativeElement.textContent).toContain('restart the backend');
    fixture.destroy();
  });

  it('allows another start after resetting a paused ride', async () => {
    const api = createApiMock();
    await TestBed.configureTestingModule({
      imports: [SimulationPage],
      providers: [provideRouter([]), { provide: SimulationApiService, useValue: api }],
    }).compileComponents();
    const fixture = TestBed.createComponent(SimulationPage);
    fixture.detectChanges();
    const button = (selector: string) =>
      fixture.nativeElement.querySelector(selector) as HTMLButtonElement;

    button('.button-reset').click();
    fixture.detectChanges();

    expect(button('.button-start').disabled).toBe(false);
    fixture.destroy();
  });

  it('displays connection errors instead of substituting fake state', async () => {
    const api = createApiMock(null);
    await TestBed.configureTestingModule({
      imports: [SimulationPage],
      providers: [provideRouter([]), { provide: SimulationApiService, useValue: api }],
    }).compileComponents();
    const fixture = TestBed.createComponent(SimulationPage);
    fixture.detectChanges();

    expect(fixture.nativeElement.querySelector('[role="alert"]')?.textContent).toContain(
      'Cannot reach the HomeTrainer API',
    );
    expect(fixture.nativeElement.textContent).toContain('—');
    fixture.destroy();
  });
});
