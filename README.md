# ESP32 MQTT Integration with ThingsBoard

📡 **IoT Project – Dummy Sensor Data via MQTT**  
This project demonstrates how to connect an **ESP32** to the **ThingsBoard IoT platform** over MQTT. The ESP32 publishes **dummy temperature and humidity sensor values** in JSON format, which are visualized on the ThingsBoard dashboard.  

🔗 GitHub Repository: [Things_Board](https://github.com/daniyal-944/Things_Board)

---

## 🚀 Features
- ✅ MQTT connection with **ThingsBoard Cloud** (`mqtt://thingsboard.cloud`)  
- ✅ Authentication using **device access token**  
- ✅ **Dummy sensor data** (temperature & humidity) generation  
- ✅ JSON telemetry publishing to `v1/devices/me/telemetry`  
- ✅ Periodic publishing using **FreeRTOS tasks**  
- ✅ Verified visualization on **ThingsBoard dashboard**  

---

## 🛠️ Technical Details

### MQTT Client
- **Broker:** `mqtt://thingsboard.cloud`  
- **Authentication:** Device access token (no username/password needed)  
- **Topic:** `v1/devices/me/telemetry`  
- **APIs Used:**
  - `esp_mqtt_client_init()` – Initialize client  
  - `esp_mqtt_client_start()` – Connect to broker  
  - `esp_mqtt_client_publish()` – Send telemetry  

### Telemetry Data Format
```json
{
  "temperature": 27,
  "humidity": 65
}
```


## 🛠️ Dummy Sensor Logic
- Temperature & humidity values are **randomly generated** for simulation.  
- Data is sent every **5 seconds** using FreeRTOS `vTaskDelay()`.  

---

## 📂 Project Structure
Things_Board/
│── main/
│ ├── main.c # ESP32 firmware
│── CMakeLists.txt
│── sdkconfig
└── README.md


---

## ⚙️ Getting Started

### 1️⃣ Prerequisites
- Install **ESP-IDF** (v5.x recommended) → [ESP-IDF Setup Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)  
- Create a **ThingsBoard Cloud** account → [ThingsBoard](https://thingsboard.cloud)  
- An ESP32 development board + USB cable  

### 2️⃣ Register Device in ThingsBoard
1. Log in to **ThingsBoard Cloud**.  
2. Navigate to **Devices → Add New Device**.  
3. Copy the **Access Token** from the device settings.  

### 3️⃣ Configure the ESP32 Firmware
Open `main/main.c` and replace the placeholder token with your device access token:
```c
#define THINGSBOARD_TOKEN "YOUR_DEVICE_ACCESS_TOKEN"
```
---
## 4️⃣ Build & Flash
```bash
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```
---
### 5️⃣ Verify

- In the ESP-IDF monitor, you should see successful MQTT connection logs and telemetry publishes.
- On the ThingsBoard dashboard, add temperature & humidity widgets to visualize data.
---
### 📊 Outputs

- ESP-IDF Monitor Logs
- Shows MQTT connection status and telemetry publish logs.
- ThingsBoard Dashboard
- Real-time temperature & humidity widgets.

---
## 📌 Summary

This project demonstrates end-to-end IoT data flow:  
**ESP32 → MQTT → ThingsBoard → Dashboard**

It provides a strong foundation for integrating real sensors in future IoT applications.

---

## 💡 Future Improvements

- 🔹 Replace dummy values with real sensor readings (DHT11/DHT22).  
- 🔹 Enable QoS 1 for reliable delivery.  
- 🔹 Add reconnection logic for MQTT failures.  
- 🔹 Implement bi-directional communication to receive commands from ThingsBoard.  

---

✍️ Developed as part of an IoT learning assignment.
