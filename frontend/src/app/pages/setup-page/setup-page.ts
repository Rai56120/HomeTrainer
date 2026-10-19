import { HttpErrorResponse } from '@angular/common/http';
import { DecimalPipe } from '@angular/common';
import { Component, inject, signal } from '@angular/core';
import { FormBuilder, ReactiveFormsModule, Validators } from '@angular/forms';
import { RouterLink } from '@angular/router';
import { SimulationApiService } from '../../services/simulation-api.service';

@Component({
  selector: 'app-setup-page',
  imports: [DecimalPipe, ReactiveFormsModule, RouterLink],
  templateUrl: './setup-page.html',
})
export class SetupPage {
  private readonly formBuilder = inject(FormBuilder);
  private readonly api = inject(SimulationApiService);

  readonly form = this.formBuilder.nonNullable.group({
    userName: ['Rider', [Validators.required, Validators.maxLength(80)]],
    userWeightKg: [60, [Validators.min(1), Validators.max(300)]],
    userHeightCm: [170, [Validators.min(50), Validators.max(250)]],
    userGender: ['MALE'],
    frontalAreaM2: [0.4, [Validators.min(0), Validators.max(2)]],
    powerWatts: [180, [Validators.min(0), Validators.max(500.5)]],
    bikeWeightKg: [8, [Validators.min(0), Validators.max(20)]],
    dragCoefficient: [0.32, [Validators.min(0), Validators.max(1.5)]],
    drivetrainLossPercent: [3.5, [Validators.min(0), Validators.max(100)]],
    rollingResistance: [0.004, [Validators.min(0), Validators.max(0.5)]],
    airDensity: [1.225, [Validators.min(0), Validators.max(10)]],
    headwindMs: [0],
  });

  readonly routeFileName = signal<string | null>(null);
  readonly routeError = signal<string | null>(null);
  readonly powerFeedback = signal<string | null>(null);
  readonly applyingPower = signal(false);

  onRouteFileSelected(event: Event): void {
    const input = event.target as HTMLInputElement;
    const file = input.files?.[0];
    this.routeError.set(null);
    this.routeFileName.set(null);

    if (!file) {
      return;
    }

    if (!file.name.toLowerCase().endsWith('.gpx')) {
      this.routeError.set('Choose a GPX file with a .gpx extension.');
      input.value = '';
      return;
    }

    this.routeFileName.set(file.name);
  }

  applyPower(): void {
    this.powerFeedback.set(null);
    const powerControl = this.form.controls.powerWatts;
    if (powerControl.invalid || this.applyingPower()) {
      powerControl.markAsTouched();
      return;
    }

    this.applyingPower.set(true);
    this.api.setPower(powerControl.value).subscribe({
      next: (state) => {
        this.applyingPower.set(false);
        this.powerFeedback.set(`Power set to ${state.powerWatts.toFixed(1)} W.`);
      },
      error: (error: unknown) => {
        this.applyingPower.set(false);
        this.powerFeedback.set(this.describeApiError(error));
      },
    });
  }

  private describeApiError(error: unknown): string {
    if (error instanceof HttpErrorResponse && error.status === 0) {
      return 'Could not reach the HomeTrainer API. Check that the backend is running.';
    }
    if (error instanceof HttpErrorResponse) {
      return `The API rejected the power update (HTTP ${error.status}).`;
    }
    return 'An unexpected error occurred while updating power.';
  }
}
