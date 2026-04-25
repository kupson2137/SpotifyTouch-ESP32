# 🎵 Spotify Bluetooth Controller (ESP32)

Stylowy i funkcjonalny pilot do Spotify oparty na mikrokontrolerze ESP32 oraz dotykowym ekranie TFT 2.8". Urządzenie komunikuje się z komputerem lub telefonem przez Bluetooth (udając klawiaturę multimedialną), co pozwala na sterowanie muzyką bez dotykania głównego urządzenia.

## 🚀 Funkcje
- **Płynny start:** Animowany pasek ładowania (300 kroków z efektem easing).
- **Sterowanie:** Play/Pause, Następny/Poprzedni utwór.
- **Głośność:** Przyciski +/- do szybkiej regulacji.
- **Przycisk LAUNCH:** Automatyczne otwieranie aplikacji Spotify na systemie Windows (przez Win+R).
- **Status połączenia:** Dioda na ekranie informująca o sparowaniu przez Bluetooth.
- **Kalibracja:** Wbudowany system kalibracji dotyku (4 punkty) zapisywany w pamięci nieulotnej (Preferences).

## 🛠️ Schemat połączeń (Pinout)

| Komponent | Pin (ESP32 / Zasilanie) |
|-----------|-------------------------|
| **VCC** | 3.3V                    |
| **GND** | GND                     |
| **LCD LED**| GPIO 15 / 3.3v    |
| **LCD CS** | GPIO 5                 |
| **LCD DC** | GPIO 2                 |
| **LCD RST**| GPIO 4                 |
| **LCD MOSI**| GPIO 23                |
| **LCD SCK** | GPIO 18                |
| **TOUCH CS**| GPIO 27                |
| **TOUCH CLK**| GPIO 25                |
| **TOUCH DIN**| GPIO 32                |
| **TOUCH DO** | GPIO 34                |

## 📦 Wymagane biblioteki
Aby skompilować projekt, musisz mieć zainstalowane następujące biblioteki w Arduino IDE:
1. `Adafruit_GFX`
2. `Adafruit_ST7789`
3. `XPT2046_Touchscreen`
4. `ESP32-BLE-Keyboard`

## 📸 Uruchomienie
1. Wgraj kod na swoje ESP32.
2. Połącz się przez Bluetooth z urządzeniem o nazwie **"Spotify BT"**.
3. Przy pierwszym uruchomieniu wykonaj kalibrację dotykając punktów na ekranie.
4. Ciesz się bezprzewodowym sterowaniem!

## ⚠️ Ważne ustawienia kompilacji (Arduino IDE)

Ze względu na rozmiar stosu Bluetooth (biblioteka `BleKeyboard`), projekt wymaga odpowiedniego ustawienia partycji pamięci Flash. Jeśli użyjesz domyślnych ustawień, kod może się nie zmieścić.

### Wymagania:
- **Mikrokontroler:** ESP32 Wroom (Flash 4MB).
- **Partition Scheme:** Musisz ustawić na **Huge APP (3MB No OTA/1MB SPIFFS)**.

### Jak to ustawić?
1. W Arduino IDE wejdź w menu **Narzędzia (Tools)**.
2. Znajdź opcję **Partition Scheme**.
3. Wybierz z listy: **Huge APP (3MB No OTA/1MB SPIFFS)**.
4. Dopiero teraz kliknij **Upload**.
---
*Projekt stworzony z pasją do muzyki i mikrokontrolerów.*
