import { HttpErrorResponse } from '@angular/common/http';
import { DecimalPipe } from '@angular/common';
import { Component, DestroyRef, inject, signal } from '@angular/core';
import { takeUntilDestroyed } from '@angular/core/rxjs-interop';
import { FormsModule } from '@angular/forms';
import { RouterLink } from '@angular/router';
import { EMPTY, catchError, interval, startWith, switchMap } from 'rxjs';
import { SimulationState } from '../../models/simulation-state';
import { SimulationApiService, SimulationControl } from '../../services/simulation-api.service';

@Component({
  selector: 'app-simulation-page',
  imports: [DecimalPipe, FormsModule, RouterLink],
  templateUrl: './simulation-page.html',
})
export class SimulationPage {
  readonly api = inject(SimulationApiService);
  private readonly destroyRef = inject(DestroyRef);

  readonly pending = signal(false);
  readonly stopped = signal(false);
  readonly errorMessage = signal<string | null>(null);
  readonly powerInput = signal<number | null>(null);
  readonly elapsedSeconds = signal(0);
  private lastClockUpdate = Date.now();

  constructor() {
    interval(250)
      .pipe(takeUntilDestroyed(this.destroyRef))
      .subscribe(() => {
        const now = Date.now();
        if (this.state?.running) {
          this.elapsedSeconds.update((elapsed) => elapsed + (now - this.lastClockUpdate) / 1000);
        }
        this.lastClockUpdate = now;
      });

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

  get displayedPowerWatts(): number {
    return this.powerInput() ?? this.state?.powerWatts ?? 180;
  }

  get formattedElapsedTime(): string {
    const totalSeconds = Math.floor(this.elapsedSeconds());
    const hours = Math.floor(totalSeconds / 3600);
    const minutes = Math.floor((totalSeconds % 3600) / 60);
    const seconds = totalSeconds % 60;
    return [hours, minutes, seconds].map((value) => String(value).padStart(2, '0')).join(':');
  }

  updatePower(event: Event): void {
    this.powerInput.set(Number((event.target as HTMLInputElement).value));
  }

  applyPower(): void {
    if (this.pending()) {
      return;
    }
    this.pending.set(true);
    this.errorMessage.set(null);
    this.api.setPower(this.displayedPowerWatts).subscribe({
      next: () => this.pending.set(false),
      error: (error: unknown) => {
        this.pending.set(false);
        this.errorMessage.set(this.describeApiError(error));
      },
    });
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
          this.elapsedSeconds.set(0);
        } else if (operation === 'start') {
          this.stopped.set(false);
        }
        if (operation === 'pause' || operation === 'stop' || operation === 'reset') {
          this.powerInput.set(null);
        }
        if (operation === 'reset') {
          this.stopped.set(false);
          this.elapsedSeconds.set(0);
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
