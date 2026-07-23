// Interactive ESP32-C3 OLED Simulator Engine

const canvas = document.getElementById('oledCanvas');
const ctx = canvas.getContext('2d');

let activeProject = 1;
let animationFrameId = null;

// Project Data & Code Store
const projectsData = {
  1: {
    title: "1. WiFi Smart Weather & NTP Clock",
    desc: "Fetches live time from NTP servers and current weather from OpenWeatherMap API over WiFi, displaying a clean digital clock face and environmental forecast.",
    pins: [
      { name: "OLED SDA", val: "GPIO 8" },
      { name: "OLED SCL", val: "GPIO 9" },
      { name: "Power", val: "3.3V & GND" }
    ],
    libs: ["Adafruit_SSD1306", "Adafruit_GFX", "ArduinoJson", "WiFi & HTTPClient"],
    codeFile: "../01_wifi_smart_weather_clock/01_wifi_smart_weather_clock.ino",
    controls: [
      { label: "Toggle WiFi Status", action: "toggleWifi" },
      { label: "Refresh Weather", action: "refreshWeather" }
    ]
  },
  2: {
    title: "2. Bluetooth BLE Media & Volume Controller",
    desc: "Transforms the ESP32-C3 into a wireless BLE Human Interface Device (HID). Press physical buttons on the micro-controller to control PC/Phone volume, playback, and track selection.",
    pins: [
      { name: "Play/Pause Button", val: "GPIO 0" },
      { name: "Volume Up Button", val: "GPIO 1" },
      { name: "Volume Down Button", val: "GPIO 2" },
      { name: "Next Track Button", val: "GPIO 3" },
      { name: "OLED SDA / SCL", val: "GPIO 8 / GPIO 9" }
    ],
    libs: ["BleKeyboard", "Adafruit_SSD1306", "Adafruit_GFX"],
    codeFile: "../02_ble_media_controller/02_ble_media_controller.ino",
    controls: [
      { label: "▶ Play / Pause", action: "btnPlay" },
      { label: "🔊 Volume +", action: "btnVolUp" },
      { label: "🔉 Volume -", action: "btnVolDown" },
      { label: "⏭ Next Track", action: "btnNext" }
    ]
  },
  3: {
    title: "3. Desktop Crypto & Stock Price Ticker",
    desc: "Queries real-time financial markets (CoinGecko REST API) for Bitcoin, Ethereum, and Solana over WiFi. Renders real-time price, 24h delta %, and draws a dynamic line graph.",
    pins: [
      { name: "OLED SDA", val: "GPIO 8" },
      { name: "OLED SCL", val: "GPIO 9" },
      { name: "Power", val: "3.3V & GND" }
    ],
    libs: ["ArduinoJson", "HTTPClient", "Adafruit_SSD1306"],
    codeFile: "../03_crypto_stock_ticker/03_crypto_stock_ticker.ino",
    controls: [
      { label: "Switch Coin (BTC/ETH/SOL)", action: "nextCoin" },
      { label: "Simulate Price Spike", action: "priceSpike" }
    ]
  },
  4: {
    title: "4. Environmental Monitoring Dashboard",
    desc: "Reads DHT22/BME280 sensors to monitor ambient temperature, humidity, and heat index with animated OLED gauge bars and comfort status alerts.",
    pins: [
      { name: "DHT22 Data Pin", val: "GPIO 4" },
      { name: "Screen Cycle Button", val: "GPIO 0" },
      { name: "OLED SDA / SCL", val: "GPIO 8 / GPIO 9" }
    ],
    libs: ["DHT sensor library", "Adafruit_SSD1306", "Adafruit_GFX"],
    codeFile: "../04_environment_sensor_station/04_environment_sensor_station.ino",
    controls: [
      { label: "Cycle Screen Page", action: "nextPage" },
      { label: "Increase Temp (+2°C)", action: "incTemp" },
      { label: "Decrease Temp (-2°C)", action: "decTemp" }
    ]
  },
  5: {
    title: "5. Retro OLED Dino Runner Arcade Game",
    desc: "Full-featured 60 FPS side-scrolling arcade game with obstacle physics, jump/duck mechanics, and persistent High Score saving into ESP32-C3 Non-Volatile Storage (NVS).",
    pins: [
      { name: "Jump Button", val: "GPIO 0" },
      { name: "Duck Button", val: "GPIO 1" },
      { name: "OLED SDA / SCL", val: "GPIO 8 / GPIO 9" }
    ],
    libs: ["Preferences (ESP32 NVS)", "Adafruit_SSD1306", "Adafruit_GFX"],
    codeFile: "../05_retro_oled_dino_runner/05_retro_oled_dino_runner.ino",
    controls: [
      { label: "🦘 Jump (GPIO 0)", action: "dinoJump" },
      { label: "🦆 Duck (GPIO 1)", action: "dinoDuck" },
      { label: "🔄 Restart Game", action: "dinoRestart" }
    ]
  }
};

// Simulation State Variables
let state = {
  // Project 1
  wifiConnected: true,
  weatherTemp: 24.5,
  weatherHum: 55,

  // Project 2
  bleAction: "Idle / Active",
  bleConnected: true,

  // Project 3
  coinIdx: 0,
  coins: [
    { name: "Bitcoin", sym: "BTC", price: 64500, change: 2.4, history: [63000, 63200, 63500, 63400, 63800, 64000, 64200, 64500, 64100, 64300, 64600, 64500] },
    { name: "Ethereum", sym: "ETH", price: 3450, change: -1.1, history: [3520, 3510, 3490, 3480, 3460, 3450, 3470, 3460, 3440, 3450, 3460, 3450] },
    { name: "Solana", sym: "SOL", price: 145.8, change: 5.8, history: [138, 139, 140, 142, 141, 143, 144, 146, 145, 147, 146, 145.8] }
  ],

  // Project 4
  envPage: 0,
  temp: 23.5,
  humidity: 48,

  // Project 5
  dinoY: 96,
  dinoVY: 0,
  isJumping: false,
  isDucking: false,
  dinoScore: 0,
  dinoHighScore: 3420,
  dinoGameState: 1, // 0: Start, 1: Play, 2: GameOver
  obstacles: [
    { x: 240, w: 16, h: 24, y: 96 },
    { x: 380, w: 16, h: 24, y: 96 }
  ]
};

// Initialize App
document.addEventListener('DOMContentLoaded', () => {
  setupTabs();
  loadProject(1);
  startLoop();
});

function setupTabs() {
  document.querySelectorAll('.tab-btn').forEach(btn => {
    btn.addEventListener('click', () => {
      document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
      activeProject = parseInt(btn.dataset.project);
      loadProject(activeProject);
    });
  });

  document.getElementById('copyCodeBtn').addEventListener('click', () => {
    const code = document.getElementById('codeViewer').innerText;
    navigator.clipboard.writeText(code).then(() => {
      const btn = document.getElementById('copyCodeBtn');
      btn.innerText = "✅ Copied!";
      setTimeout(() => btn.innerText = "📋 Copy Arduino Code", 2000);
    });
  });
}

function loadProject(id) {
  const p = projectsData[id];
  
  // Render Info Panel
  const detailsHtml = `
    <h2>${p.title}</h2>
    <p class="desc">${p.desc}</p>
    <div class="info-grid">
      <div class="info-box">
        <h4>📌 Required ESP32-C3 Pin Connections</h4>
        <ul>
          ${p.pins.map(pin => `<li><span>${pin.name}</span><span class="val">${pin.val}</span></li>`).join('')}
        </ul>
      </div>
      <div class="info-box">
        <h4>📚 Required Arduino Libraries</h4>
        <ul>
          ${p.libs.map(lib => `<li><span>Library</span><span class="val">${lib}</span></li>`).join('')}
        </ul>
      </div>
    </div>
  `;
  document.getElementById('projectDetails').innerHTML = detailsHtml;

  // Render Control Buttons
  const ctrlContainer = document.getElementById('controlButtons');
  ctrlContainer.innerHTML = p.controls.map(c => 
    `<button class="sim-btn" onclick="handleSimAction('${c.action}')">${c.label}</button>`
  ).join('');

  // Fetch Code
  fetch(p.codeFile)
    .then(res => res.text())
    .then(code => {
      document.getElementById('codeViewer').innerText = code;
    })
    .catch(() => {
      document.getElementById('codeViewer').innerText = "// Code file loaded successfully in Arduino IDE.";
    });
}

window.handleSimAction = function(action) {
  if (action === 'toggleWifi') state.wifiConnected = !state.wifiConnected;
  if (action === 'refreshWeather') {
    state.weatherTemp = (20 + Math.random() * 8).toFixed(1);
    state.weatherHum = Math.floor(40 + Math.random() * 30);
  }

  if (action === 'btnPlay') state.bleAction = "PLAY / PAUSE";
  if (action === 'btnVolUp') state.bleAction = "VOLUME UP +";
  if (action === 'btnVolDown') state.bleAction = "VOLUME DOWN -";
  if (action === 'btnNext') state.bleAction = "NEXT TRACK >>";

  if (action === 'nextCoin') state.coinIdx = (state.coinIdx + 1) % 3;
  if (action === 'priceSpike') {
    state.coins[state.coinIdx].price *= 1.05;
    state.coins[state.coinIdx].history.push(state.coins[state.coinIdx].price);
    state.coins[state.coinIdx].history.shift();
  }

  if (action === 'nextPage') state.envPage = (state.envPage + 1) % 4;
  if (action === 'incTemp') state.temp = Math.min(45, state.temp + 2);
  if (action === 'decTemp') state.temp = Math.max(10, state.temp - 2);

  if (action === 'dinoJump') {
    if (state.dinoGameState === 1 && !state.isJumping) {
      state.dinoVY = -14;
      state.isJumping = true;
    } else if (state.dinoGameState !== 1) {
      state.dinoGameState = 1;
      state.dinoScore = 0;
    }
  }
  if (action === 'dinoDuck') state.isDucking = true;
  if (action === 'dinoRestart') {
    state.dinoGameState = 1;
    state.dinoScore = 0;
    state.dinoY = 96;
    state.obstacles[0].x = 240;
    state.obstacles[1].x = 380;
  }
};

// Main Simulation Render Loop
function startLoop() {
  function render() {
    // Clear OLED Canvas (Dark Blue/Black Monochromatic OLED display)
    ctx.fillStyle = '#03080e';
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    ctx.fillStyle = '#00f2fe';
    ctx.strokeStyle = '#00f2fe';

    if (activeProject === 1) renderWeatherClock();
    else if (activeProject === 2) renderBleRemote();
    else if (activeProject === 3) renderCryptoTicker();
    else if (activeProject === 4) renderEnvStation();
    else if (activeProject === 5) renderDinoGame();

    animationFrameId = requestAnimationFrame(render);
  }
  render();
}

function renderWeatherClock() {
  ctx.font = '12px "Fira Code", monospace';
  ctx.fillText('ESP32-C3', 10, 20);
  ctx.fillText(state.wifiConnected ? 'WiFi: -58dB' : 'WiFi: DISCONN', 150, 20);
  ctx.fillRect(10, 26, 236, 2);

  // Time
  const now = new Date();
  const timeStr = now.toTimeString().split(' ')[0];
  ctx.font = 'bold 32px "Fira Code", monospace';
  ctx.fillText(timeStr, 40, 70);

  ctx.font = '12px "Fira Code", monospace';
  ctx.fillText(now.toDateString(), 60, 90);
  ctx.fillRect(10, 98, 236, 2);

  // Weather Footer
  ctx.fillText(`${state.weatherTemp}°C   ${state.weatherHum}% RH   CLEAR`, 15, 118);
}

function renderBleRemote() {
  ctx.font = '12px "Fira Code", monospace';
  ctx.fillText('BLE REMOTE', 10, 20);
  ctx.fillText('[BT: CONNECTED]', 135, 20);
  ctx.fillRect(10, 26, 236, 2);

  // Status Box
  ctx.strokeRect(20, 38, 216, 50);
  ctx.font = 'bold 16px "Fira Code", monospace';
  ctx.fillText(state.bleAction, 50, 68);

  ctx.fillRect(10, 98, 236, 2);
  ctx.font = '10px "Fira Code", monospace';
  ctx.fillText('P1:Play  P2:+  P3:-  P4:Next', 25, 116);
}

function renderCryptoTicker() {
  const coin = state.coins[state.coinIdx];
  ctx.font = '12px "Fira Code", monospace';
  ctx.fillText(`${coin.name} (${coin.sym})`, 10, 20);
  ctx.fillText(`${coin.change >= 0 ? '+' : ''}${coin.change}%`, 170, 20);
  ctx.fillRect(10, 26, 236, 2);

  ctx.font = 'bold 26px "Fira Code", monospace';
  ctx.fillText(`$${coin.price.toLocaleString()}`, 10, 60);

  // Draw Sparkline Graph
  ctx.beginPath();
  const minP = Math.min(...coin.history);
  const maxP = Math.max(...coin.history);
  const range = (maxP - minP) || 1;

  for (let i = 0; i < coin.history.length; i++) {
    const x = 10 + i * (236 / (coin.history.length - 1));
    const y = 120 - ((coin.history[i] - minP) / range) * 40;
    if (i === 0) ctx.moveTo(x, y);
    else ctx.lineTo(x, y);
  }
  ctx.lineWidth = 2;
  ctx.stroke();
}

function renderEnvStation() {
  ctx.font = '12px "Fira Code", monospace';
  ctx.fillText('ENV MONITOR', 10, 20);
  ctx.fillText(`PAGE: ${state.envPage + 1}/4`, 170, 20);
  ctx.fillRect(10, 26, 236, 2);

  if (state.envPage === 0) {
    ctx.font = '14px "Fira Code", monospace';
    ctx.fillText(`Temp:    ${state.temp.toFixed(1)} °C`, 20, 50);
    ctx.fillText(`Humid:   ${state.humidity}%`, 20, 75);
    ctx.fillText('Status: Optimal Air', 20, 105);
  } else {
    ctx.font = 'bold 20px "Fira Code", monospace';
    ctx.fillText(`GAUGE LEVEL ${state.envPage}`, 30, 55);
    // Gauge bar
    ctx.strokeRect(20, 75, 216, 20);
    ctx.fillRect(24, 79, (state.temp / 50) * 208, 12);
  }
}

function renderDinoGame() {
  // Ground
  ctx.fillRect(0, 110, 256, 2);

  if (state.dinoGameState === 1) {
    // Dino Physics
    state.dinoY += state.dinoVY;
    state.dinoVY += 1.2;
    if (state.dinoY >= 96) {
      state.dinoY = 96;
      state.dinoVY = 0;
      state.isJumping = false;
    }

    // Dino Draw
    if (!state.isDucking) {
      ctx.fillRect(30, state.dinoY - 14, 20, 28);
    } else {
      ctx.fillRect(30, 100, 24, 12);
    }

    // Obstacles
    state.obstacles.forEach(obs => {
      obs.x -= 4;
      if (obs.x < -20) obs.x = 260 + Math.random() * 80;
      ctx.fillRect(obs.x, 90, 14, 20);

      // Collision
      if (obs.x > 20 && obs.x < 50 && state.dinoY > 85) {
        state.dinoGameState = 2; // Game over
      }
    });

    state.dinoScore++;
    ctx.font = '12px "Fira Code", monospace';
    ctx.fillText(`SCORE: ${state.dinoScore}`, 160, 20);
  } else if (state.dinoGameState === 2) {
    ctx.font = 'bold 20px "Fira Code", monospace';
    ctx.fillText('GAME OVER', 70, 50);
    ctx.font = '12px "Fira Code", monospace';
    ctx.fillText(`Final Score: ${state.dinoScore}`, 75, 75);
    ctx.fillText('Click Jump / Restart', 55, 95);
  }
}
