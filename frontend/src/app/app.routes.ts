import { Routes } from '@angular/router';
import { SetupPage } from './pages/setup-page/setup-page';
import { SimulationPage } from './pages/simulation-page/simulation-page';

export const routes: Routes = [
  { path: 'setup', component: SetupPage },
  { path: 'simulation', component: SimulationPage },
  { path: '', pathMatch: 'full', redirectTo: 'setup' },
  { path: '**', redirectTo: 'setup' },
];
