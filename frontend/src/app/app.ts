import { Component, inject } from '@angular/core';
import { RouterLink, RouterLinkActive, RouterOutlet } from '@angular/router';
import { SimulationApiService } from './services/simulation-api.service';

@Component({
  selector: 'app-root',
  imports: [RouterLink, RouterLinkActive, RouterOutlet],
  templateUrl: './app.html',
  styleUrl: './app.scss',
})
export class App {
  readonly api = inject(SimulationApiService);

  get connectionLabel(): string {
    switch (this.api.connection()) {
      case 'connected':
        return 'API CONNECTED';
      case 'offline':
        return 'API OFFLINE';
      default:
        return 'API NOT CHECKED';
    }
  }
}
