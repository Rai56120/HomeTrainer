export interface SimulationState {
  speedKmh: number;
  powerWatts: number;
  gradientPercent: number;
  distanceKm: number;
  routeDistanceKm: number;
  running: boolean;
  finished: boolean;
}
