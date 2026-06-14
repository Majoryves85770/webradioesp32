//=====================
// code de la radio web wifi , les liens des stations qui y figurent sont donnés à titre d'exemple , 
// vous les insérez en lieu et place de ceux présents. 
// attention chaque nouvelle station nécessite l'insertion et la conversion du logo dans le fichier logo.h
// réalisé par MAJORYves - 
//=======================


#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include "vs1053_ext.h"
#include "logos.h"

// ============================================================
// EQUALIZER
// ============================================================
#define EQ_MAX_HEIGHT 40
#define EQ_WIDTH  (SCREEN_W - 20)

// ============================================================
// BROCHES VS1053 (HSPI)
// ============================================================
#define VS1053_CS    16
#define VS1053_DCS   4
#define VS1053_DREQ  26
#define VS1053_MOSI  13
#define VS1053_MISO  12
#define VS1053_SCK   14


// ============================================================
// COULEURS
// ============================================================
#define COLOR_BG         0x0861
#define COLOR_ACCENT     0x07E0
#define COLOR_ACCENT2    0x3666
#define COLOR_TEXT       0xFFFF
#define COLOR_SUBTEXT    0xAD75
#define COLOR_BTN_BG     0x2124
#define COLOR_BTN_BDR    0x07E0
#define COLOR_PLAY       0x07E0
#define COLOR_PAUSE      0xFD20
#define COLOR_RED        0xF800
#define COLOR_YELLOW     0xFFE0
#define COLOR_GREEN      0x07E0
#define COLOR_SEPARATOR  0x2945

// ============================================================
// LAYOUT (320 x 240)
// ============================================================
#define SCREEN_W   320
#define SCREEN_H   240

#define BTN_ZONE_Y 0
#define BTN_H      46
#define BTN_W      90
#define BTN_RADIUS 9

#define BTN_PREV_X  8
#define BTN_PREV_Y  BTN_ZONE_Y
#define BTN_PLAY_X  (SCREEN_W/2 - BTN_W/2)
#define BTN_PLAY_Y  BTN_ZONE_Y
#define BTN_NEXT_X  (SCREEN_W - BTN_W - 8)
#define BTN_NEXT_Y  BTN_ZONE_Y

#define INFO_Y     (BTN_ZONE_Y + BTN_H + 4)
#define INFO_H     (SCREEN_H - INFO_Y)

#define TOUCH_DEBOUNCE 300

// ============================================================
// WiFi
// ============================================================
const char* WIFI_SSID = ""; // modifiez et indiquez ici votre SSID 
const char* WIFI_PASS = "X"; // remplacez par votre mot de passe 

// ============================================================
// STATIONS
// ============================================================
struct Station {
  const char* name;
  const char* url;
};

const Station stations[] = {
  { "RTL",        "http://streaming.radio.rtl.fr/rtl-1-44-128" }, // attention assurez vous que les liens n'ont pas changé, ici à titre d'exemple ..
  { "ICI POITOU", "http://direct.francebleu.fr/live/fbpoitou-midfi.mp3" },
  { "ALOUETTE",   "https://alouette-larochesuryon.ice.infomaniak.ch/alouette-larochesuryon-128.mp3" },
  { "COLLINES",   "http://collineslaradio.ice.infomaniak.ch/collineslaradio-128.mp3" },
 
};
const int NUM_STATIONS = sizeof(stations) / sizeof(stations[0]);

// ============================================================
// LOGOS
// ============================================================
const uint16_t* stationLogos[] = {
  RTLlogo_RTL,
  ICIlogo_ICI,
  ALOUETTElogo_ALOUETTE,
  COLLINESlogo_COLLINES,
  
};

// ============================================================
// OBJETS
// ============================================================
TFT_eSPI tft = TFT_eSPI();
VS1053 mp3(VS1053_CS, VS1053_DCS, VS1053_DREQ, HSPI, VS1053_MOSI, VS1053_MISO, VS1053_SCK);

// ============================================================
// ÉTAT
// ============================================================
int  currentStation  = 0;
bool isPlaying       = false;
bool isPaused        = false;
unsigned long lastTouchTime = 0;

// ============================================================
// PROTOTYPES
// ============================================================
void wifiSetup();
void startStation(int index);
void nextStation();
void prevStation();
void togglePlayPause();
void handleTouch();
bool isTouched(int tx, int ty, int bx, int by, int bw, int bh);
void drawAll();
void drawStationInfo();
void drawButtons();
void drawButton(int x, int y, int w, int h, const char* label, uint16_t bg);
void drawEqualizer();
void drawLogo(const uint16_t* logo);
void updateUI();

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
 
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  wifiSetup();

  mp3.begin();
  mp3.setVolume(100);

  drawAll();
  startStation(currentStation);
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  mp3.loop();
  handleTouch();
  updateUI();
  vTaskDelay(1);
}

// ============================================================
// WIFI
// ============================================================
void wifiSetup() {
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startAttemptTime > 15000) ESP.restart();
    vTaskDelay(1);
  }
}

// ============================================================
// UI UPDATE
// ============================================================
void updateUI() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 50) return;
  lastUpdate = millis();

  drawEqualizer();
}

// ============================================================
// RADIO
// ============================================================
void startStation(int index) {
  currentStation = index;
  isPaused = false;
  isPlaying = false;

  drawAll();

  if (mp3.connecttohost(stations[currentStation].url)) {
    isPlaying = true;
  }

  drawStationInfo();
  drawButtons();
}

void togglePlayPause() {
  if (!isPlaying) { startStation(currentStation); return; }

  isPaused = !isPaused;
  mp3.setVolume(isPaused ? 0 : 100);

  drawButtons();
  drawStationInfo();
}

void nextStation() { startStation((currentStation + 1) % NUM_STATIONS); }
void prevStation() { startStation((currentStation - 1 + NUM_STATIONS) % NUM_STATIONS); }

// ============================================================
// TACTILE
// ============================================================
void handleTouch() {
  if (millis() - lastTouchTime < TOUCH_DEBOUNCE) return;

  uint16_t tx, ty;
  if (!tft.getTouch(&tx, &ty, 600)) return;

  ty = SCREEN_H - ty;
  lastTouchTime = millis();

  if      (isTouched(tx, ty, BTN_PREV_X, BTN_PREV_Y, BTN_W, BTN_H)) prevStation();
  else if (isTouched(tx, ty, BTN_PLAY_X, BTN_PLAY_Y, BTN_W, BTN_H)) togglePlayPause();
  else if (isTouched(tx, ty, BTN_NEXT_X, BTN_NEXT_Y, BTN_W, BTN_H)) nextStation();
}

bool isTouched(int tx, int ty, int bx, int by, int bw, int bh) {
  return (tx >= bx && tx <= bx + bw && ty >= by && ty <= by + bh);
}

// ============================================================
// AFFICHAGE
// ============================================================
void drawAll() {
  tft.fillScreen(COLOR_BG);
  drawStationInfo();
  drawButtons();
}

// ============================================================
// LOGO — Option B : hauteur forcée 100 px, bandes supprimées
// ============================================================
bool isAlmostWhite(uint16_t c) {
  uint8_t r = (c >> 11) & 0x1F;
  uint8_t g = (c >> 5)  & 0x3F;
  uint8_t b =  c        & 0x1F;
  return (r > 28 && g > 55 && b > 28);
}

void drawLogo(const uint16_t* logo) {
  static uint16_t buffer[100*100];

  for (int i = 0; i < 100*100; i++) {
    uint16_t c = pgm_read_word(&logo[i]);
    buffer[i] = (c >> 8) | (c << 8);
  }

  int lastGoodLine = 99;

  for (int y = 99; y >= 0; y--) {
    bool allWhite = true;

    for (int x = 0; x < 100; x++) {
      uint16_t c = buffer[y * 100 + x];
      if (!isAlmostWhite(c)) {
        allWhite = false;
        break;
      }
    }

    if (!allWhite) {
      lastGoodLine = y;
      break;
    }
  }

  for (int y = lastGoodLine + 1; y < 100; y++) {
    memcpy(&buffer[y * 100], &buffer[lastGoodLine * 100], 100 * sizeof(uint16_t));
  }

  int x = (SCREEN_W - 100) / 2;
  int y = INFO_Y + 20; // descente de 15 px

  tft.pushImage(x, y, 100, 100, buffer);
}

void drawStationInfo() {
  tft.fillRect(0, INFO_Y, SCREEN_W, INFO_H, COLOR_BG);

  drawLogo(stationLogos[currentStation]);

  int dotY = INFO_Y + 110;
  int dotGap = 11;
  int dotX = (SCREEN_W - NUM_STATIONS * dotGap) / 2;

  for (int i = 0; i < NUM_STATIONS; i++) {
    if (i == currentStation)
      tft.fillCircle(dotX + i * dotGap, dotY, 4, COLOR_ACCENT);
    else
      tft.drawCircle(dotX + i * dotGap, dotY, 3, COLOR_SEPARATOR);
  }
}

// ============================================================
// BOUTONS
// ============================================================
void drawButtons() {
  uint16_t playBg = isPlaying ? (isPaused ? 0x8000 : 0x0340) : COLOR_BTN_BG;
  const char* playLabel = (isPlaying && !isPaused) ? "PAUSE" : " PLAY";

  drawButton(BTN_PREV_X, BTN_PREV_Y, BTN_W, BTN_H, "  <<  ", COLOR_BTN_BG);
  drawButton(BTN_PLAY_X, BTN_PLAY_Y, BTN_W, BTN_H, playLabel, playBg);
  drawButton(BTN_NEXT_X, BTN_NEXT_Y, BTN_W, BTN_H, "  >>  ", COLOR_BTN_BG);
}

void drawButton(int x, int y, int w, int h, const char* label, uint16_t bg) {

  // Couleurs Glassmorphism Pulse
  uint16_t haloColor   = tft.color565(120, 0, 150);   // halo violet doux
  uint16_t borderColor = tft.color565(180, 0, 200);   // contour violet clair
  uint16_t glassColor  = tft.color565(30, 30, 30);    // verre fumé

  // Halo subtil
  tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, BTN_RADIUS + 2, haloColor);

  // Contour fin
  tft.drawRoundRect(x, y, w, h, BTN_RADIUS, borderColor);

  // Fond "verre fumé"
  tft.fillRoundRect(x + 1, y + 1, w - 2, h - 2, BTN_RADIUS - 1, glassColor);

  // Texte blanc centré
  tft.setTextColor(TFT_WHITE, glassColor);
  tft.setTextSize(2);
  tft.setCursor(x + (w - strlen(label) * 12) / 2, y + (h - 16) / 2);
  tft.print(label);
}



// ============================================================
// EQUALIZER SPECTRUM FLUIDE
// ============================================================
void drawEqualizer() {
  static float levels[32];
  int eqY = INFO_Y + INFO_H - EQ_MAX_HEIGHT - 10;

  // Efface la zone
  tft.fillRect(10, eqY, SCREEN_W - 20, EQ_MAX_HEIGHT, COLOR_BG);

  for (int i = 0; i < 32; i++) {

    // --- Simulation FFT réaliste ---
    float zoneFactor =
      (i < 8)  ? 0.6 :      // basses
      (i < 20) ? 1.0 :      // médiums
                 1.4;       // aigus

    float target = random(5, EQ_MAX_HEIGHT * zoneFactor);
    levels[i] = (levels[i] * 0.75f) + (target * 0.25f);

    int h = constrain(levels[i], 2, EQ_MAX_HEIGHT);
    int x = 80 + i * 5;     // <-- barres fines + espace
    int y = eqY + (EQ_MAX_HEIGHT - h);

    // --- Couleur dynamique Pulse (bleu → violet → rose) ---
    float t = (float)i / 31.0f;  // 0 → 1 selon la barre

    // interpolation RGB
    uint8_t r = (uint8_t)( (1.0f - t) * 0   + t * 255 );   // 0 → 255
    uint8_t g = (uint8_t)( (1.0f - t) * 80  + t * 0   );   // 80 → 0
    uint8_t b = (uint8_t)( (1.0f - t) * 255 + t * 120 );   // 255 → 120

    uint16_t color = tft.color565(r, g, b);

    // --- Barres fines ---
    tft.fillRect(x, y, 3, h, color);
  }
}

