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
  let api: { setPower: ReturnType<typeof vi.fn> };

  beforeEach(async () => {
    api = { setPower: vi.fn().mockReturnValue(of(sampleState)) };
    await TestBed.configureTestingModule({
      imports: [SetupPage],
      providers: [provideRouter([]), { provide: SimulationApiService, useValue: api }],
    }).compileComponents();
  });

  it('shows all requested setup fields and their specified bounds', () => {
    const fixture = TestBed.createComponent(SetupPage);
    fixture.detectChanges();
    const compiled = fixture.nativeElement as HTMLElement;

    expect(compiled.querySelector('#power-slider')?.getAttribute('max')).toBe('500.5');
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

  it('shows local-only rider profile fields without sending them to the API', () => {
    const fixture = TestBed.createComponent(SetupPage);
    fixture.detectChanges();
    const compiled = fixture.nativeElement as HTMLElement;

    expect(compiled.querySelector('[formControlName="userName"]')).not.toBeNull();
    expect(compiled.querySelector('[formControlName="userWeightKg"]')).not.toBeNull();
    expect(compiled.querySelector('[formControlName="userHeightCm"]')).not.toBeNull();
    expect(compiled.querySelector('[formControlName="userGender"]')).not.toBeNull();
    expect(compiled.querySelector('[formControlName="frontalAreaM2"]')).not.toBeNull();
    expect(compiled.textContent).toContain('not sent to the current API');
    expect(api.setPower).not.toHaveBeenCalled();
  });

  it('keeps a selected GPX local and does not send unsupported setup to the API', () => {
    const fixture = TestBed.createComponent(SetupPage);
    fixture.detectChanges();
    const input = fixture.nativeElement.querySelector('#route-file') as HTMLInputElement;
    const file = new File(['<gpx/>'], 'training-loop.gpx', { type: 'application/gpx+xml' });
    Object.defineProperty(input, 'files', { configurable: true, value: [file] });
    input.dispatchEvent(new Event('change'));
    fixture.detectChanges();

    expect(fixture.nativeElement.textContent).toContain('training-loop.gpx');
    expect(fixture.nativeElement.textContent).toContain('not uploaded or used by the simulation');
    expect(api.setPower).not.toHaveBeenCalled();
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

  it('applies slider power through the documented API action', () => {
    const fixture = TestBed.createComponent(SetupPage);
    fixture.detectChanges();
    const component = fixture.componentInstance;
    component.form.controls.powerWatts.setValue(222.5);
    component.applyPower();
    fixture.detectChanges();

    expect(api.setPower).toHaveBeenCalledWith(222.5);
    expect(fixture.nativeElement.querySelector('[role="status"]')?.textContent).toContain(
      'Power set to 222.5 W.',
    );
  });
});
