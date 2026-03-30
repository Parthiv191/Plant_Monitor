# Plant Monitor (ESP32)

Real-time soil moisture, temperature, humidity, and light level
monitoring using an ESP32 with OLED display, Wi-Fi connectivity, and
live web interface.

------------------------------------------------------------------------

## ✨ Current Features

-   **Sensors**
    -   DHT11 for temperature and humidity\
    -   Capacitive soil moisture sensor (ADC)\
    -   Photoresistor for ambient light (ADC)\
-   **Continuous ADC Sampling**
    -   Uses ESP-IDF continuous ADC driver for higher accuracy\
    -   Averaged readings for stable light & soil values\
-   **Smart Light Tracking**
    -   Stores 24-hour rolling light data (48 samples)\
    -   Computes daily average light exposure\
-   **Display**
    -   Live readings + status indicators on SSD1306 OLED\
-   **Wi-Fi + Web Server**
    -   Connects to Wi-Fi (station mode)\
    -   Hosts HTTP server for remote monitoring\
    -   mDNS enabled for easy local access\
-   **Status Analysis System**
    -   Each parameter labeled (Good / Bad)\
    -   Drives alert logic + UI\
-   **LED Indicators**
    -   Status LED (heartbeat blink)\
    -   Alert LED when any condition is not "Good"

------------------------------------------------------------------------

## 🧭 Removed / Changed Features

-   Removed manual calibration routine (handled via normalization)
-   Sampling now handled via FreeRTOS task instead of fixed loop timing
-   Sensor averaging improved (especially light)

------------------------------------------------------------------------

## 📸 Demo

Pending hardware completion (OLED link below):
https://www.amazon.com/HiLetgo-Serial-128X64-Display-Color/dp/B06XRBTBTB

------------------------------------------------------------------------

## 🧱 System Overview

``` mermaid
flowchart LR
  DHT[DHT11 Sensor] --> MCU[ESP32]
  PHOTO[Photoresistor] --> MCU
  SOIL[Soil Sensor] --> MCU
  MCU --> OLED[OLED Display]
  MCU --> LEDS[Status & Alert LEDs]
  MCU --> WIFI[Wi-Fi + HTTP Server]
```

------------------------------------------------------------------------

## 🔩 Hardware

  Component        Interface           Notes
  ---------------- ------------------- ------------------------------
  ESP32            Main MCU            Wi-Fi enabled
  DHT11            GPIO26              Temp & humidity
  Soil Sensor      GPIO39 (ADC1 CH3)   Inverted moisture %
  Light Sensor     GPIO36 (ADC1 CH0)   \% brightness
  OLED (SSD1306)   I²C                 Live display
  LED (Status)     GPIO2               Blinks every cycle
  LED (Alert)      GPIO4               Turns on when issue detected

------------------------------------------------------------------------

## ⚙️ Firmware Architecture

-   **FreeRTOS Task (`sensor_task`)**
    -   Reads sensors
    -   Processes ADC buffer
    -   Updates rolling averages
    -   Runs analysis
    -   Updates OLED + LEDs
    -   Sends data to web server
-   **Modules**
    -   `analysis.c` → Determines status strings\
    -   `plant_data.c` → Stores global plant state\
    -   `wifi_sta.c` → Wi-Fi connection\
    -   `http_server.c` → Web dashboard backend\
    -   `display_oled.c` → OLED rendering

------------------------------------------------------------------------

## 🌐 Web Interface

-   Runs automatically when Wi-Fi connects\
-   Accessible via:
    -   IP address\
    -   mDNS hostname (e.g., `plant.local`)\
-   Displays:
    -   Live sensor values\
    -   Status indicators

------------------------------------------------------------------------

## 📤 Example Data (Internal Representation)

``` json
{
  "temp_c": 24.3,
  "humidity": 48.1,
  "light_avg": 67.5,
  "soil": 42.0,
  "status": {
    "temp": "Good",
    "humidity": "Good",
    "light": "Low",
    "soil": "Dry"
  }
}
```

------------------------------------------------------------------------

## 🧰 Repo Layout

    plant-monitor/
    ├─ components/
    ├─ main/
    │  └─ main.c
    ├─ CMakeLists.txt
    └─ README.md

------------------------------------------------------------------------

## 🧪 Testing

-   OLED confirms real-time updates\
-   LED behavior validates alert logic\
-   Serial logs show sensor + error states\
-   Web UI verifies remote access

------------------------------------------------------------------------

## 🗺️ Roadmap

-   [ ] Data logging (SPIFFS / SD)
-   [ ] MQTT integration
-   [ ] OTA updates
-   [ ] Mobile-friendly dashboard
-   [ ] Auto-watering system

------------------------------------------------------------------------

## 📚 References

-   ESP-IDF ADC Continuous Mode docs\
-   DHT11 datasheet\
-   SSD1306 datasheet
