import { HttpErrorResponse } from '@angular/common/http';
import { Component, inject, signal } from '@angular/core';
import { FormBuilder, ReactiveFormsModule, Validators } from '@angular/forms';
import { RouterLink } from '@angular/router';
import { switchMap } from 'rxjs';
import {
  BikeConfiguration,
  SimulationApiService,
  UserConfiguration,
} from '../../services/simulation-api.service';

@Component({
  selector: 'app-setup-page',
  imports: [ReactiveFormsModule, RouterLink],
  templateUrl: './setup-page.html',
})
export class SetupPage {
  private readonly formBuilder = inject(FormBuilder);
  private readonly api = inject(SimulationApiService);

  readonly form = this.formBuilder.nonNullable.group({
    userName: ['Rider', [Validators.required, Validators.maxLength(80)]],
    userWeightKg: [60, [Validators.min(1), Validators.max(300)]],
    userHeightCm: [170, [Validators.min(50), Validators.max(250)]],
    userGender: this.formBuilder.nonNullable.control<'MALE' | 'FEMALE'>('MALE'),
    frontalAreaM2: [0.4, [Validators.min(0), Validators.max(2)]],
    bikeWeightKg: [8, [Validators.min(0), Validators.max(20)]],
    dragCoefficient: [0.32, [Validators.min(0), Validators.max(1.5)]],
    drivetrainLossPercent: [3.5, [Validators.min(0), Validators.max(100)]],
    rollingResistance: [0.004, [Validators.min(0), Validators.max(0.5)]],
    airDensity: [1.225, [Validators.min(0), Validators.max(10)]],
    headwindMs: [0, [Validators.min(-100), Validators.max(100)]],
  });

  readonly routeFileName = signal<string | null>(null);
  readonly routeFile = signal<File | null>(null);
  readonly routeError = signal<string | null>(null);
  readonly routeFeedback = signal<string | null>(null);
  readonly configurationFeedback = signal<string | null>(null);
  readonly uploadingRoute = signal(false);
  readonly applyingConfiguration = signal(false);

  onRouteFileSelected(event: Event): void {
    const input = event.target as HTMLInputElement;
    const file = input.files?.[0];
    this.routeError.set(null);
    this.routeFeedback.set(null);
    this.routeFileName.set(null);
    this.routeFile.set(null);

    if (!file) {
      return;
    }

    if (!file.name.toLowerCase().endsWith('.gpx')) {
      this.routeError.set('Choose a GPX file with a .gpx extension.');
      input.value = '';
      return;
    }

    this.routeFileName.set(file.name);
    this.routeFile.set(file);
  }

  uploadRoute(): void {
    const file = this.routeFile();
    if (!file || this.uploadingRoute()) {
      return;
    }

    this.uploadingRoute.set(true);
    this.routeFeedback.set(null);
    this.api.setRoute(file).subscribe({
      next: (state) => {
        this.uploadingRoute.set(false);
        this.routeFeedback.set(`Circuit loaded (${state.routeDistanceKm.toFixed(2)} km).`);
      },
      error: (error: unknown) => {
        this.uploadingRoute.set(false);
        this.routeFeedback.set(this.describeApiError(error));
      },
    });
  }

  applyConfiguration(): void {
    if (this.applyingConfiguration()) {
      return;
    }
    if (this.form.invalid) {
      this.form.markAllAsTouched();
      return;
    }

    const values = this.form.getRawValue();
    const user: UserConfiguration = {
      name: values.userName,
      weightKg: values.userWeightKg,
      heightCm: values.userHeightCm,
      gender: values.userGender,
      frontalAreaM2: values.frontalAreaM2,
    };
    const bike: BikeConfiguration = {
      weightKg: values.bikeWeightKg,
      dragCoefficient: values.dragCoefficient,
      drivetrainLossPercent: values.drivetrainLossPercent,
      airDensity: values.airDensity,
      rollingResistance: values.rollingResistance,
      headwindMs: values.headwindMs,
    };

    this.applyingConfiguration.set(true);
    this.configurationFeedback.set(null);
    this.api.setUser(user).pipe(switchMap(() => this.api.setBike(bike))).subscribe({
      next: () => {
        this.applyingConfiguration.set(false);
        this.configurationFeedback.set('Rider and bike settings applied to the simulation.');
      },
      error: (error: unknown) => {
        this.applyingConfiguration.set(false);
        this.configurationFeedback.set(this.describeApiError(error));
      },
    });
  }

  private describeApiError(error: unknown): string {
    if (error instanceof HttpErrorResponse && error.status === 0) {
      return 'Could not reach the HomeTrainer API. Check that the backend is running.';
    }
    if (error instanceof HttpErrorResponse) {
      return `The API rejected the setup update (HTTP ${error.status}).`;
    }
    return 'An unexpected error occurred while updating the setup.';
  }
}
