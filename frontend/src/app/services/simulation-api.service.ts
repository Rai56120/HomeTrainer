import { HttpClient } from '@angular/common/http';
import { Injectable, inject, signal } from '@angular/core';
import { Observable, catchError, tap, throwError } from 'rxjs';
import { SimulationState } from '../models/simulation-state';

export type ApiConnection = 'unknown' | 'connected' | 'offline';
export type SimulationControl = 'start' | 'pause' | 'stop' | 'reset';

export interface UserConfiguration {
  name: string;
  weightKg: number;
  heightCm: number;
  gender: 'MALE' | 'FEMALE';
  frontalAreaM2: number;
}

export interface BikeConfiguration {
  weightKg: number;
  dragCoefficient: number;
  drivetrainLossPercent: number;
  airDensity: number;
  rollingResistance: number;
  headwindMs: number;
}

@Injectable({ providedIn: 'root' })
export class SimulationApiService {
  private readonly http = inject(HttpClient);

  readonly connection = signal<ApiConnection>('unknown');
  readonly state = signal<SimulationState | null>(null);

  getState(): Observable<SimulationState> {
    return this.withState(this.http.get<SimulationState>('/api/state'));
  }

  setPower(watts: number): Observable<SimulationState> {
    return this.withState(this.http.post<SimulationState>('/api/control/power', { watts }));
  }

  setRoute(file: File): Observable<SimulationState> {
    const body = new FormData();
    body.append('file', file, file.name);
    return this.withState(this.http.post<SimulationState>('/api/route', body));
  }

  setUser(configuration: UserConfiguration): Observable<SimulationState> {
    return this.withState(this.http.post<SimulationState>('/api/config/user', configuration));
  }

  setBike(configuration: BikeConfiguration): Observable<SimulationState> {
    return this.withState(this.http.post<SimulationState>('/api/config/bike', configuration));
  }

  control(operation: SimulationControl): Observable<SimulationState> {
    return this.withState(this.http.post<SimulationState>(`/api/simulation/${operation}`, null));
  }

  private withState(request: Observable<SimulationState>): Observable<SimulationState> {
    return request.pipe(
      tap((state) => {
        this.state.set(state);
        this.connection.set('connected');
      }),
      catchError((error: unknown) => {
        this.connection.set('offline');
        return throwError(() => error);
      }),
    );
  }
}
