import { HttpErrorResponse } from '@angular/common/http';
import { DecimalPipe } from '@angular/common';
import { Component, DestroyRef, inject, signal } from '@angular/core';
import { takeUntilDestroyed } from '@angular/core/rxjs-interop';
import { RouterLink } from '@angular/router';
import { EMPTY, catchError, interval, startWith, switchMap } from 'rxjs';
import { SimulationState } from '../../models/simulation-state';
import { SimulationApiService, SimulationControl } from '../../services/simulation-api.service';

@Component({
  selector: 'app-simulation-page',
  imports: [DecimalPipe, RouterLink],
  templateUrl: './simulation-page.html',
})
export class SimulationPage {
  readonly api = inject(SimulationApiService);
  private readonly destroyRef = inject(DestroyRef);

  readonly pending = signal(false);
  readonly stopped = signal(false);
  readonly errorMessage = signal<string | null>(null);

  constructor() {
    interval(1000)
      .pipe(
        startWith(0),
        switchMap(() =>
          this.api.getState().pipe(
            catchError((error: unknown) => {
              this.errorMessage.set(this.describeApiError(error));
              return EMPTY;
            }),
          ),
        ),
        takeUntilDestroyed(this.destroyRef),
      )
      .subscribe();
  }

  get state(): SimulationState | null {
    return this.api.state();
  }

  get progressPercent(): number {
    const state = this.state;
    if (!state || state.routeDistanceKm <= 0) {
      return 0;
    }
    return Math.max(0, Math.min(100, (state.distanceKm / state.routeDistanceKm) * 100));
  }

  get distanceRemainingKm(): number {
    const state = this.state;
    return state ? Math.max(0, state.routeDistanceKm - state.distanceKm) : 0;
  }

  control(operation: SimulationControl): void {
    if (this.pending()) {
      return;
    }

    this.pending.set(true);
    this.errorMessage.set(null);
    this.api.control(operation).subscribe({
      next: () => {
        this.pending.set(false);
        if (operation === 'stop') {
          this.stopped.set(true);
        }
      },
      error: (error: unknown) => {
        this.pending.set(false);
        this.errorMessage.set(this.describeApiError(error));
      },
    });
  }

  private describeApiError(error: unknown): string {
    if (error instanceof HttpErrorResponse && error.status === 0) {
      return 'Cannot reach the HomeTrainer API. Start the backend and check the connection.';
    }
    if (error instanceof HttpErrorResponse) {
      return `The API request failed (HTTP ${error.status}).`;
    }
    return 'An unexpected error occurred while communicating with the API.';
  }
}
