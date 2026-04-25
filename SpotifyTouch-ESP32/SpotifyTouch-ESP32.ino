#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <XPT2046_Touchscreen.h>
#include <BleKeyboard.h>
#include <Preferences.h>
#include "logo.h"

// --- PINY ---
#define TFT_CS    5
#define TFT_DC    2
#define TFT_RST   4
#define TFT_MOSI  23
#define TFT_SCLK  18
#define TFT_MISO  19

#define TOUCH_CS  27
#define TOUCH_CLK 25
#define TOUCH_DIN 32
#define TOUCH_DO  34

// --- KOLORY ---
#define SPOTIFY_GREEN 0x03FF
#define SOFT_WHITE    0xFFFF
#define ERROR_RED     0xF800
#define VIVID_BLUE    0x03FF
#define BT_OFF_COLOR  0x0841

// --- OBIEKTY ---
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
SPIClass touchSPI(HSPI);
XPT2046_Touchscreen ts(TOUCH_CS);
BleKeyboard bleKeyboard("Spotify BT", "ESP32");
Preferences prefs;

// --- ZMIENNE ---
int xMin, xMax, yMin, yMax;
bool lastConnectStatus = false;
unsigned long lastBlinkMillis = 0;
bool blinkState = true;

// --- PŁYNNE ODŚWIEŻANIE PASKA (Z OBSŁUGĄ FLOAT) ---
void drawSmoothBar(float progress) {
    static int lastWidth = 0;
    int barW = 240;
    int barH = 20;
    int barX = 40;
    int barY = 170;

    // Precyzyjne mapowanie dla płynniejszego ruchu 📈
    int currentWidth = map(progress * 10, 0, 1000, 0, barW - 6);

    if (currentWidth > lastWidth) {
        tft.fillRect(barX + 3 + lastWidth, barY + 3, currentWidth - lastWidth, barH - 6, SPOTIFY_GREEN);
        lastWidth = currentWidth;
    }
}

// --- RYSOWANIE PRZYCISKU LAUNCH ---
void drawLaunchButton(bool active) {
    if (active) {
        tft.fillRoundRect(210, 10, 95, 30, 8, SPOTIFY_GREEN);
        tft.setTextColor(0x0000);
    } else {
        tft.fillRoundRect(210, 10, 95, 30, 8, BT_OFF_COLOR);
        tft.setTextColor(SPOTIFY_GREEN);
    }
    tft.setTextSize(1);
    tft.setCursor(232, 21);
    tft.print("LAUNCH");
}

// --- GŁÓWNY INTERFEJS ---
void drawMainInterface() {
    tft.fillScreen(0x0000);
    tft.fillRect(0, 0, 320, 50, 0x10A2); 
    tft.drawFastHLine(0, 50, 320, SPOTIFY_GREEN);
    tft.setTextColor(SPOTIFY_GREEN);
    tft.setTextSize(2);
    tft.setCursor(20, 15);
    tft.print("SPOTIFY");
    tft.drawRoundRect(20, 70, 80, 80, 10, SOFT_WHITE);    
    tft.drawRoundRect(120, 70, 80, 80, 10, SPOTIFY_GREEN); 
    tft.drawRoundRect(220, 70, 80, 80, 10, SOFT_WHITE);    
    tft.setTextColor(SOFT_WHITE); tft.setTextSize(2);
    tft.setCursor(50, 105); tft.print("<<");
    tft.setCursor(250, 105); tft.print(">>");
    tft.setTextColor(SPOTIFY_GREEN);
    tft.setCursor(150, 105); tft.print("||");
    tft.drawRoundRect(20, 180, 130, 45, 10, VIVID_BLUE);
    tft.drawRoundRect(170, 180, 130, 45, 10, VIVID_BLUE);
    tft.setTextColor(VIVID_BLUE);
    tft.setCursor(75, 195); tft.print("-");
    tft.setCursor(225, 195); tft.print("+");
}

// --- KALIBRACJA DOTYKU ---
void runCalibration() {
    TS_Point p[4];
    int pts[4][2] = {{30, 30}, {290, 30}, {30, 210}, {290, 210}};
    for (int i = 0; i < 4; i++) {
        tft.fillScreen(0x0000);
        tft.drawCircle(pts[i][0], pts[i][1], 15, SPOTIFY_GREEN);
        tft.setTextColor(SOFT_WHITE);
        tft.setCursor(50, 110);
        tft.print("KALIBRACJA PKT: "); tft.print(i + 1);
        while (!ts.touched()) delay(10);
        p[i] = ts.getPoint();
        delay(500);
        while (ts.touched()) delay(10);
    }
    xMin = (p[0].x + p[2].x) / 2; xMax = (p[1].x + p[3].x) / 2;
    yMin = (p[0].y + p[1].y) / 2; yMax = (p[2].y + p[3].y) / 2;
    prefs.begin("spotify", false);
    prefs.putInt("xmin", xMin); prefs.putInt("xmax", xMax);
    prefs.putInt("ymin", yMin); prefs.putInt("ymax", yMax);
    prefs.end();
}

void setup() {
    Serial.begin(115200);
    tft.init(240, 320);
    tft.setRotation(1);
    tft.invertDisplay(false);
    tft.fillScreen(0x0000);
    touchSPI.begin(TOUCH_CLK, TOUCH_DO, TOUCH_DIN, TOUCH_CS);
    ts.begin(touchSPI);
    ts.setRotation(1);

    tft.drawBitmap(100, 30, myLogo, 120, 120, SPOTIFY_GREEN);
    tft.drawRoundRect(40, 170, 240, 20, 10, SOFT_WHITE); 

    bleKeyboard.begin();

    // --- NOWA ANIMACJA ŁADOWANIA (300 KROKÓW + EASING) ⏳ ---
    for (int i = 0; i <= 300; i++) {
        float progress = (i / 300.0) * 100.0;
        drawSmoothBar(progress); 
        
        int wait = 10;
        if (i > 220) wait = 20; // Delikatne zwolnienie
        if (i > 280) wait = 45; // Mocne zwolnienie na finiszu
        
        delay(wait); 
    }

    prefs.begin("spotify", true);
    xMin = prefs.getInt("xmin", 0);
    if (xMin == 0) {
        prefs.end();
        runCalibration();
    } else {
        xMax = prefs.getInt("xmax", 3800);
        yMin = prefs.getInt("ymin", 200);
        yMax = prefs.getInt("ymax", 3800);
        prefs.end();
    }
    drawMainInterface();
    drawLaunchButton(false);
}

void loop() {
    bool connected = bleKeyboard.isConnected();

    if (connected != lastConnectStatus) {
        tft.fillCircle(195, 25, 4, connected ? SPOTIFY_GREEN : ERROR_RED);
        drawLaunchButton(connected);
        lastConnectStatus = connected;
    }

    if (!connected) {
        if (millis() - lastBlinkMillis > 800) {
            lastBlinkMillis = millis();
            blinkState = !blinkState;
            drawLaunchButton(blinkState);
        }
    }

    if (ts.touched()) {
        TS_Point p = ts.getPoint();
        int px = map(p.x, xMin, xMax, 0, 320);
        int py = map(p.y, yMin, yMax, 0, 240);

        if (connected) {
            if (py < 60 && px > 200) {
                bleKeyboard.press(KEY_LEFT_GUI); 
                bleKeyboard.press('r');
                delay(500); 
                bleKeyboard.releaseAll(); 
                delay(1000); 
                bleKeyboard.print("spotify:"); 
                delay(500); 
                bleKeyboard.write(KEY_RETURN);
            }
            else if (py > 65 && py < 165) {
                if (px < 110) bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
                else if (px > 110 && px < 210) bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
                else if (px > 210) bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);
            }
            else if (py > 170) {
                if (px < 160) bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
                else bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
            }
            delay(300);
        }
        while (ts.touched()) delay(10);
    }
}