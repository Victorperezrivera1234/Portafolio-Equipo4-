#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <time.h>
#include <esp_wifi.h>

// Network selection: false = home Wi-Fi, true = campus Wi-Fi (enterprise login)
bool redCampus = true;

char* ssidCasa = "";
char* passwordCasa = "";

char* ssidCampus = "";
char* identidad = "";
char* usuario = "";
char* clave = "";
char*zonaHoraria = "CST6";

// Feeding schedule (editable from the Settings tab, saved in flash)
const int MAX_HORARIOS = 10;
int horas[MAX_HORARIOS];
int minutos[MAX_HORARIOS];
int numeroHorarios = 0;

// How long the motor spins, in milliseconds (editable from the Settings tab)
unsigned long duracionMotor = 10000;

// Last minute that triggered a feed, so a schedule never fires twice in the same minute
int ultimaClave = -1;

int ENA = 25;
int IN1 = 26;
int IN2 = 27;

// Motor state (non-blocking, so the web server keeps responding while it runs)
bool motorActivo = false;
unsigned long motorInicio = 0;
unsigned long motorDuracion = 0;

// Time check runs once per second
unsigned long ultimaRevision = 0;

WebServer server(80);
Preferences prefs;

const char PAGINA[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Dashboard</title>
<style>
  :root {
    --sidebar-bg: #1a2230;
    --sidebar-text: #aab4c3;
    --sidebar-active-bg: #263247;
    --sidebar-active-text: #ffffff;
    --accent: #3fb8a5;
    --page-bg: #eceff3;
    --card-bg: #ffffff;
    --text: #1d2533;
    --text-muted: #5d6a7d;
    --border: #d9dee6;
  }

  * { box-sizing: border-box; }

  html, body { height: 100%; }

  body {
    margin: 0;
    display: flex;
    font-family: system-ui, -apple-system, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
    color: var(--text);
    background: var(--page-bg);
    line-height: 1.5;
  }

  .sidebar {
    width: 230px;
    flex-shrink: 0;
    background: var(--sidebar-bg);
    padding: 1.5rem 1rem;
    display: flex;
    flex-direction: column;
    gap: 1.75rem;
  }

  .brand {
    margin: 0;
    padding: 0 0.75rem;
    font-size: 1.125rem;
    font-weight: 600;
    color: var(--sidebar-active-text);
    letter-spacing: 0.01em;
  }

  .nav {
    display: flex;
    flex-direction: column;
    gap: 0.25rem;
  }

  .nav a {
    display: block;
    padding: 0.6rem 0.75rem;
    border-left: 3px solid transparent;
    border-radius: 0 6px 6px 0;
    color: var(--sidebar-text);
    text-decoration: none;
    font-size: 0.9375rem;
    transition: background-color 0.15s, color 0.15s;
  }

  .nav a:hover {
    background: var(--sidebar-active-bg);
    color: var(--sidebar-active-text);
  }

  .nav a[aria-current="page"] {
    background: var(--sidebar-active-bg);
    border-left-color: var(--accent);
    color: var(--sidebar-active-text);
  }

  .nav a:focus-visible {
    outline: 2px solid var(--accent);
    outline-offset: -2px;
  }

  .main {
    flex: 1;
    min-width: 0;
    padding: 2.5rem;
  }

  .view[hidden] { display: none; }

  .main h1 {
    margin: 0 0 1.5rem;
    font-size: 1.75rem;
    font-weight: 650;
    letter-spacing: -0.01em;
  }

  .card {
    max-width: 34rem;
    padding: 1.5rem;
    background: var(--card-bg);
    border: 1px solid var(--border);
    border-radius: 10px;
  }

  .card h2 {
    margin: 0 0 0.5rem;
    font-size: 1.125rem;
    font-weight: 600;
  }

  .card p {
    margin: 0;
    color: var(--text-muted);
    max-width: 62ch;
  }

  .feed-btn {
    margin-top: 1.25rem;
    padding: 0.7rem 1.5rem;
    border: 0;
    border-radius: 8px;
    background: var(--accent);
    color: #0d2a26;
    font: inherit;
    font-weight: 600;
    cursor: pointer;
  }

  .feed-btn:hover { filter: brightness(0.95); }
  .feed-btn:disabled { opacity: 0.6; cursor: wait; }
  .feed-btn:focus-visible { outline: 2px solid var(--sidebar-bg); outline-offset: 2px; }

  .status {
    margin-top: 0.75rem;
    min-height: 1.5em;
    color: var(--text-muted);
    font-size: 0.9375rem;
  }

  .field { margin-top: 1.25rem; }

  .field-title {
    display: block;
    margin-bottom: 0.4rem;
    font-weight: 600;
  }

  .hint {
    margin-top: 0.35rem;
    color: var(--text-muted);
    font-size: 0.875rem;
  }

  input[type="number"], select {
    padding: 0.45rem 0.6rem;
    border: 1px solid var(--border);
    border-radius: 6px;
    background: #fff;
    color: var(--text);
    font: inherit;
  }

  input[type="number"] { width: 6rem; }

  input:focus-visible, select:focus-visible, .sec-btn:focus-visible, .link-btn:focus-visible {
    outline: 2px solid var(--accent);
    outline-offset: 2px;
  }

  .time-row {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    margin-bottom: 0.5rem;
  }

  .link-btn {
    padding: 0.25rem 0.5rem;
    border: 0;
    background: none;
    color: var(--text-muted);
    font: inherit;
    text-decoration: underline;
    cursor: pointer;
  }

  .sec-btn {
    margin-top: 0.25rem;
    padding: 0.5rem 1rem;
    border: 1px solid var(--border);
    border-radius: 8px;
    background: #fff;
    color: var(--text);
    font: inherit;
    cursor: pointer;
  }

  .sec-btn:disabled { opacity: 0.5; cursor: not-allowed; }

  @media (max-width: 640px) {
    body { flex-direction: column; }

    .sidebar {
      width: 100%;
      padding: 1rem;
      gap: 0.75rem;
    }

    .nav { flex-direction: row; gap: 0.25rem; overflow-x: auto; }
    .nav a { white-space: nowrap; border-left: 0; border-bottom: 3px solid transparent; border-radius: 6px 6px 0 0; }
    .nav a[aria-current="page"] { border-bottom-color: var(--accent); }

    .main { padding: 1.5rem 1rem; }
  }
</style>
</head>
<body>
  <aside class="sidebar">
    <p class="brand">Dashboard</p>
    <nav class="nav" aria-label="Main">
      <a href="#overview" aria-current="page">Overview</a>
      <a href="#reports">Reports</a>
      <a href="#settings">Settings</a>
    </nav>
  </aside>

  <main class="main">
    <section class="view" id="overview">
      <h1>Overview</h1>
      <div class="card">
        <h2>Welcome back</h2>
        <p>This is your main content area. Replace this card with the numbers, charts, or notes you want to see first.</p>
        <button id="feed" class="feed-btn" type="button">Feed</button>
        <div id="feed-status" class="status" role="status" aria-live="polite"></div>
      </div>
    </section>

    <section class="view" id="reports" hidden>
      <h1>Reports</h1>
      <div class="card">
        <h2>Nothing here yet</h2>
        <p>Feeding history and other reports can go here.</p>
      </div>
    </section>

    <section class="view" id="settings" hidden>
      <h1>Settings</h1>
      <div class="card">
        <h2>Feeder</h2>

        <div class="field">
          <label class="field-title" for="duration">Motor on time (seconds)</label>
          <input id="duration" type="number" min="1" max="120" step="1" value="10">
          <div class="hint">How long the motor spins each time, for scheduled and manual feeds. From 1 to 120.</div>
        </div>

        <div class="field">
          <span class="field-title" id="times-title">Feeding times (24-hour)</span>
          <div id="times" role="group" aria-labelledby="times-title"></div>
          <button id="add-time" class="sec-btn" type="button">Add time</button>
          <div id="times-hint" class="hint"></div>
        </div>

        <button id="save" class="feed-btn" type="button">Save settings</button>
        <div id="save-status" class="status" role="status" aria-live="polite"></div>
      </div>
    </section>
  </main>

  <script>
    // ----- Tabs (driven by the #hash in the address) -----
    const views = document.querySelectorAll('.view');
    const links = document.querySelectorAll('.nav a');

    function route() {
      const wanted = (location.hash || '#overview').slice(1);
      const known = Array.from(views).some(v => v.id === wanted) ? wanted : 'overview';
      views.forEach(v => { v.hidden = v.id !== known; });
      links.forEach(a => {
        if (a.getAttribute('href') === '#' + known) a.setAttribute('aria-current', 'page');
        else a.removeAttribute('aria-current');
      });
    }
    window.addEventListener('hashchange', route);
    route();

    // ----- Feed button -----
    const feedBtn = document.getElementById('feed');
    const feedStatus = document.getElementById('feed-status');

    feedBtn.addEventListener('click', async () => {
      feedBtn.disabled = true;
      feedStatus.textContent = 'Sending...';
      try {
        const res = await fetch('/feed', { method: 'POST' });
        feedStatus.textContent = await res.text();
      } catch (err) {
        feedStatus.textContent = 'Could not reach the feeder.';
      }
      setTimeout(() => { feedBtn.disabled = false; }, 1000);
    });

    // ----- Settings -----
    const MAX = 10;
    const rows = document.getElementById('times');
    const addBtn = document.getElementById('add-time');
    const hint = document.getElementById('times-hint');
    const durationInput = document.getElementById('duration');
    const saveBtn = document.getElementById('save');
    const saveStatus = document.getElementById('save-status');

    function pad(n) { return String(n).padStart(2, '0'); }

    function makeSelect(count, value, label) {
      const s = document.createElement('select');
      s.setAttribute('aria-label', label);
      for (let i = 0; i < count; i++) {
        const o = document.createElement('option');
        o.value = i;
        o.textContent = pad(i);
        if (i === value) o.selected = true;
        s.appendChild(o);
      }
      return s;
    }

    function refreshHint() {
      const n = rows.children.length;
      addBtn.disabled = n >= MAX;
      hint.textContent = n + ' of ' + MAX + ' times' + (n === 0 ? ' (no automatic feeding)' : '');
    }

    function addRow(h, m) {
      if (rows.children.length >= MAX) return;
      const row = document.createElement('div');
      row.className = 'time-row';

      const sep = document.createElement('span');
      sep.textContent = ':';

      const del = document.createElement('button');
      del.type = 'button';
      del.className = 'link-btn';
      del.textContent = 'Remove';
      del.addEventListener('click', () => { row.remove(); refreshHint(); });

      row.append(makeSelect(24, h, 'Hour'), sep, makeSelect(60, m, 'Minute'), del);
      rows.appendChild(row);
      refreshHint();
    }

    addBtn.addEventListener('click', () => addRow(12, 0));

    function currentTimes() {
      const list = Array.from(rows.children).map(row => {
        const s = row.querySelectorAll('select');
        return pad(s[0].value) + ':' + pad(s[1].value);
      });
      return Array.from(new Set(list)).sort();
    }

    async function loadConfig() {
      try {
        const res = await fetch('/config', { cache: 'no-store' });
        const cfg = await res.json();
        durationInput.value = cfg.duration;
        rows.innerHTML = '';
        cfg.times.forEach(t => {
          const parts = t.split(':');
          addRow(parseInt(parts[0], 10), parseInt(parts[1], 10));
        });
        refreshHint();
      } catch (err) {
        saveStatus.textContent = 'Could not load the current settings.';
      }
    }

    saveBtn.addEventListener('click', async () => {
      const seconds = parseInt(durationInput.value, 10);
      if (!(seconds >= 1 && seconds <= 120)) {
        saveStatus.textContent = 'Motor on time must be between 1 and 120 seconds.';
        return;
      }
      saveBtn.disabled = true;
      saveStatus.textContent = 'Saving...';
      try {
        const body = new URLSearchParams({
          duration: String(seconds),
          times: currentTimes().join(',')
        });
        const res = await fetch('/config', { method: 'POST', body: body });
        saveStatus.textContent = await res.text();
        if (res.ok) loadConfig();
      } catch (err) {
        saveStatus.textContent = 'Could not reach the feeder.';
      }
      saveBtn.disabled = false;
    });

    loadConfig();
  </script>
</body>
</html>
)rawliteral";


void iniciarMotor(unsigned long duracion){
  Serial.println("Iniciando motores");

  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  analogWrite(ENA,255);

  motorActivo = true;
  motorInicio = millis();
  motorDuracion = duracion;
}

void detenerMotor(){
  analogWrite(ENA,0);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);

  motorActivo = false;
  Serial.println("Motor detenido");
}

// Reads text like "22:36,07:30" into the arrays. Returns false if anything is invalid.
bool analizarHorarios(const String& texto, int* h, int* m, int& n){
  n = 0;
  int inicio = 0;
  int largo = texto.length();

  while(inicio < largo){
    int coma = texto.indexOf(',', inicio);
    if(coma < 0) coma = largo;

    String parte = texto.substring(inicio, coma);
    int hh, mm;
    if(sscanf(parte.c_str(), "%d:%d", &hh, &mm) != 2) return false;
    if(hh < 0 || hh > 23 || mm < 0 || mm > 59) return false;
    if(n >= MAX_HORARIOS) return false;

    h[n] = hh;
    m[n] = mm;
    n++;
    inicio = coma + 1;
  }
  return true;
}

String horariosComoTexto(){
  String texto = "";
  for(int i = 0; i < numeroHorarios; i++){
    char buf[8];
    snprintf(buf, sizeof(buf), "%02d:%02d", horas[i], minutos[i]);
    if(i > 0) texto += ",";
    texto += buf;
  }
  return texto;
}

void cargarConfig(){
  prefs.begin("alimentador", false);
  unsigned long duracion = prefs.getULong("duracion", 10000);
  String guardado = prefs.getString("horarios", "22:36,22:37");
  prefs.end();

  if(duracion < 1000 || duracion > 120000) duracion = 10000;
  duracionMotor = duracion;

  int h[MAX_HORARIOS], m[MAX_HORARIOS], n;
  if(analizarHorarios(guardado, h, m, n)){
    for(int i = 0; i < n; i++){
      horas[i] = h[i];
      minutos[i] = m[i];
    }
    numeroHorarios = n;
  }

  Serial.printf("Config: motor %lu ms, %d horarios (%s)\n", duracionMotor, numeroHorarios, horariosComoTexto().c_str());
}

void paginaPrincipal(){
  server.send_P(200, "text/html", PAGINA);
}

void manejarFeed(){
  if(motorActivo){
    server.send(409, "text/plain", "The feeder is already running.");
    return;
  }
  iniciarMotor(duracionMotor);
  server.send(200, "text/plain", "Feeding now.");
}

void enviarConfig(){
  String json = "{\"duration\":" + String(duracionMotor / 1000) + ",\"times\":[";
  for(int i = 0; i < numeroHorarios; i++){
    char buf[8];
    snprintf(buf, sizeof(buf), "\"%02d:%02d\"", horas[i], minutos[i]);
    if(i > 0) json += ",";
    json += buf;
  }
  json += "]}";

  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", json);
}

void guardarConfig(){
  if(!server.hasArg("duration") || !server.hasArg("times")){
    server.send(400, "text/plain", "Missing settings.");
    return;
  }

  long segundos = server.arg("duration").toInt();
  if(segundos < 1 || segundos > 120){
    server.send(400, "text/plain", "Motor on time must be between 1 and 120 seconds.");
    return;
  }

  int h[MAX_HORARIOS], m[MAX_HORARIOS], n;
  if(!analizarHorarios(server.arg("times"), h, m, n)){
    server.send(400, "text/plain", "Invalid times. Use up to 10 times in HH:MM format.");
    return;
  }

  duracionMotor = (unsigned long)segundos * 1000UL;
  for(int i = 0; i < n; i++){
    horas[i] = h[i];
    minutos[i] = m[i];
  }
  numeroHorarios = n;

  prefs.begin("alimentador", false);
  prefs.putULong("duracion", duracionMotor);
  prefs.putString("horarios", horariosComoTexto());
  prefs.end();

  Serial.println("Configuracion guardada");
  server.send(200, "text/plain", "Settings saved.");
}

void setup() {

  Serial.begin(115200);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  analogWrite(ENA,0);

  cargarConfig();
  
  WiFi.mode(WIFI_STA);
  if(redCampus){
    WiFi.begin(ssidCampus, WPA2_AUTH_PEAP, identidad, usuario, clave);
  } else {
    WiFi.begin(ssidCasa, passwordCasa);
  }
  Serial.print("Conectando al Wi-Fi");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado");
  Serial.print("Abre en tu navegador: http://");
  Serial.println(WiFi.localIP());

  configTzTime(zonaHoraria,"pool.ntp.org","time.nist.gov");
  Serial.println("Esperando la hora...");
  struct tm tiempo;

  while(!getLocalTime(&tiempo)){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nHora sincronizada");

  server.on("/", HTTP_GET, paginaPrincipal);
  server.on("/feed", HTTP_POST, manejarFeed);
  server.on("/config", HTTP_GET, enviarConfig);
  server.on("/config", HTTP_POST, guardarConfig);
  server.begin();
  Serial.println("Servidor web iniciado");
}


void loop() {
  // Keep the web page responsive
  server.handleClient();

  // Stop the motor when its time is up
  if(motorActivo && millis() - motorInicio >= motorDuracion){
    detenerMotor();
  }

  // Check the schedule once per second
  if(millis() - ultimaRevision < 1000){
    return;
  }
  ultimaRevision = millis();

  struct tm tiempo;

  if(!getLocalTime(&tiempo)){
    Serial.println("No se pudo leer la hora");
    return;
  }

  Serial.printf("Hora: %02d:%02d:%02d\n",tiempo.tm_hour,tiempo.tm_min,tiempo.tm_sec);

  int clave_minuto = tiempo.tm_yday * 1440 + tiempo.tm_hour * 60 + tiempo.tm_min;
 
  for(int i = 0;i<numeroHorarios; i++){
    if(tiempo.tm_hour == horas[i] && tiempo.tm_min == minutos[i] && clave_minuto != ultimaClave){
      // If the motor is busy (e.g. manual feed), try again next second
      if(motorActivo){
        continue;
      }
      ultimaClave = clave_minuto;

      iniciarMotor(duracionMotor);
    }
  }
}
