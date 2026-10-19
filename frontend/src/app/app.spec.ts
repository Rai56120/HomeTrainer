import { TestBed } from '@angular/core/testing';
import { provideRouter } from '@angular/router';
import { signal } from '@angular/core';
import { App } from './app';
import { SimulationApiService } from './services/simulation-api.service';

describe('App', () => {
  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [App],
      providers: [
        provideRouter([]),
        {
          provide: SimulationApiService,
          useValue: { connection: signal('unknown') },
        },
      ],
    }).compileComponents();
  });

  it('renders setup and ride navigation with an unchecked API status', () => {
    const fixture = TestBed.createComponent(App);
    fixture.detectChanges();

    const compiled = fixture.nativeElement as HTMLElement;
    expect(compiled.querySelector('.main-nav a[routerLink="/setup"]')?.textContent).toContain(
      'Setup',
    );
    expect(compiled.querySelector('.main-nav a[routerLink="/simulation"]')?.textContent).toContain(
      'Ride',
    );
    expect(compiled.querySelector('.connection-indicator')?.textContent).toContain(
      'API NOT CHECKED',
    );
  });
});
