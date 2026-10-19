import { HttpClient } from '@angular/common/http';
import { Injectable, inject, signal } from '@angular/core';
import { Observable, catchError, tap, throwError } from 'rxjs';
import { SimulationState } from '../models/simulation-state';

export type ApiConnection = 'unknown' | 'connected' | 'offline';
export type SimulationControl = 'start' | 'pause' | 'stop' | 'reset';

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
