import { TestBed } from '@angular/core/testing';
import { provideRouter } from '@angular/router';
import { of } from 'rxjs';
import { SetupPage } from './setup-page';
import { SimulationApiService } from '../../services/simulation-api.service';
import { SimulationState } from '../../models/simulation-state';

const sampleState: SimulationState = {
  speedKmh: 0,
  powerWatts: 222.5,
  gradientPercent: 0,
  distanceKm: 0,
  routeDistanceKm: 8,
  running: false,
  finished: false,
};

describe('SetupPage', () => {
  let api: {
    setRoute: ReturnType<typeof vi.fn>;
    setUser: ReturnType<typeof vi.fn>;
    setBike: ReturnType<typeof vi.fn>;
  };

  beforeEach(async () => {
    api = {
      setRoute: vi.fn().mockReturnValue(of(sampleState)),
      setUser: vi.fn().mockReturnValue(of(sampleState)),
      setBike: vi.fn().mockReturnValue(of(sampleState)),
    };
    await TestBed.configureTestingModule({
      imports: [SetupPage],
      providers: [provideRouter([]), { provide: SimulationApiService, useValue: api }],
    }).compileComponents();
  });

  it('shows all requested setup fields and their specified bounds', () => {
    const fixture = TestBed.createComponent(SetupPage);
    fixture.detectChanges();
    const compiled = fixture.nativeElement as HTMLElement;

    expect(compiled.querySelector('[formControlName="bikeWeightKg"]')?.getAttribute('max')).toBe(
      '20',
    );
    expect(compiled.querySelector('[formControlName="dragCoefficient"]')?.getAttribute('max')).toBe(
      '1.5',
    );
    expect(
      compiled.querySelector('[formControlName="rollingResistance"]')?.getAttribute('max'),
    ).toBe('0.5');
    expect(compiled.querySelector('[formControlName="airDensity"]')?.getAttribute('max')).toBe(
      '10',
    );
    expect(compiled.querySelector('[formControlName="headwindMs"]')).not.toBeNull();
    expect(compiled.textContent).toContain('Drive train losses');
  });

  it('applies rider and bike profile fields to the API', () => {
    const fixture = TestBed.createComponent(SetupPage);
    fixture.detectChanges();
    const compiled = fixture.nativeElement as HTMLElement;

    expect(compiled.querySelector('[formControlName="userName"]')).not.toBeNull();
    expect(compiled.querySelector('[formControlName="userWeightKg"]')).not.toBeNull();
    expect(compiled.querySelector('[formControlName="userHeightCm"]')).not.toBeNull();
    expect(compiled.querySelector('[formControlName="userGender"]')).not.toBeNull();
    expect(compiled.querySelector('[formControlName="frontalAreaM2"]')).not.toBeNull();
    fixture.componentInstance.applyConfiguration();

    expect(api.setUser).toHaveBeenCalledWith({
      name: 'Rider',
      weightKg: 60,
      heightCm: 170,
      gender: 'MALE',
      frontalAreaM2: 0.4,
    });
    expect(api.setBike).toHaveBeenCalledWith({
      weightKg: 8,
      dragCoefficient: 0.32,
      drivetrainLossPercent: 3.5,
      airDensity: 1.225,
      rollingResistance: 0.004,
      headwindMs: 0,
    });
  });

  it('uploads a selected GPX file to change the active route', () => {
    const fixture = TestBed.createComponent(SetupPage);
    fixture.detectChanges();
    const input = fixture.nativeElement.querySelector('#route-file') as HTMLInputElement;
    const file = new File(['<gpx/>'], 'training-loop.gpx', { type: 'application/gpx+xml' });
    Object.defineProperty(input, 'files', { configurable: true, value: [file] });
    input.dispatchEvent(new Event('change'));
    fixture.detectChanges();

    expect(fixture.nativeElement.textContent).toContain('training-loop.gpx');
    fixture.componentInstance.uploadRoute();
    expect(api.setRoute).toHaveBeenCalledWith(file);
  });

  it('rejects non-GPX files with an accessible message', () => {
    const fixture = TestBed.createComponent(SetupPage);
    fixture.detectChanges();
    const input = fixture.nativeElement.querySelector('#route-file') as HTMLInputElement;
    const file = new File(['not a route'], 'notes.txt', { type: 'text/plain' });
    Object.defineProperty(input, 'files', { configurable: true, value: [file] });
    input.dispatchEvent(new Event('change'));
    fixture.detectChanges();

    expect(fixture.nativeElement.querySelector('[role="alert"]')?.textContent).toContain(
      'Choose a GPX file',
    );
  });

});
