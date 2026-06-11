# 🐟 Water Quality Monitoring and Automated Fish Feeder System

![Platform](https://img.shields.io/badge/Platform-ESP32-blue)
![Language](https://img.shields.io/badge/Language-C%2B%2B%20%28Arduino%29-orange)
![IoT](https://img.shields.io/badge/IoT-Blynk-green)
![Sensors](https://img.shields.io/badge/Sensors-DS18B20%20%7C%20pH-yellow)
![Status](https://img.shields.io/badge/Status-Completed-brightgreen)

## 📽️ Demo Video

▶️ [Watch the full project demonstration on YouTube](https://youtu.be/rWLZThYqyv8)

---

## Abstract

This project is an aquarium monitoring and automated fish feeding system built on the **ESP32 microcontroller**. It integrates a **DS18B20 temperature sensor** and an **analog pH sensor** to continuously monitor water quality. Readings are shown locally on an **OLED display** and transmitted to the user's smartphone in real time via the **Blynk IoT platform**. A **servo-controlled mechanical feeder** (fabricated from 3D-printed parts) dispenses a fixed quantity of food at scheduled intervals. The system features a **power-failure failsafe**: using non-volatile memory (ESP32 Preferences) and NTP-based time synchronization, it preserves the feeding schedule across reboots and automatically compensates for any feedings missed during a power outage. The result is a low-cost, integrated solution for maintaining healthy aquatic conditions in aquariums.

---

## Features

- 🌡️ **Real-time temperature monitoring** using DS18B20 (±0.5 °C accuracy)
- 🧪 **Real-time pH monitoring** with moving-average filtering for stable readings
- 📱 **Remote monitoring** via the Blynk mobile app (live gauges + historical graphs)
- 🔔 **Automatic alerts** — push notification to smartphone when pH or temperature exceeds safe thresholds
- 🕐 **Scheduled automated feeding** — time synchronized via NTP (GMT+6), no internal RTC required
- 🔁 **Missed-feeding recovery** — detects and compensates for feedings missed during a power outage
- 💾 **Non-volatile state persistence** — feeding schedule survives reboots with zero data loss


---

## System Overview

The system is divided into four operational phases: **Initialization → Data Processing → Operation → Failsafe**.

![image alt]([https://github.com/joyontadebnath/Water-Quality-Monitoring-and-Automated-Fish-Feeder-System/blob/87aff5d93a407e934373e8091ce9356f6a66e133/images/methodology_diagram.png](https://github.com/joyontadebnath/Water-Quality-Monitoring-and-Automated-Fish-Feeder-System/blob/ff33aee1f220c343e2a3631c102326babab767d8/images/methodology.png))

### Phase 1 – Initialization
- Feeding schedule is set (user-configurable interval).
- Safe thresholds for temperature and pH are defined in firmware.
- DS18B20 and pH sensors are initialized and calibrated.
- OLED display and servo motor are set up; servo is held at the stopped position (90°).
- System connects to Wi-Fi and syncs local time from the NTP server.

### Phase 2 – Data Processing
- pH sensor outputs a 12-bit ADC value → converted to voltage → mapped to pH scale (0–14) via a two-segment linear calibration equation.
- A 20-sample moving-average filter smooths pH fluctuations.
- Temperature is read digitally from the DS18B20 via the One-Wire bus.
- Both values are displayed on the OLED and pushed to Blynk virtual pins V0 (temperature) and V1 (pH) every second.

### Phase 3 – Operation
- If pH falls outside **5.5–8.5** or temperature falls outside **28–33 °C**, the system logs a Blynk event and sends a push notification to the user's phone.
- When the current local time matches the programmed feeding minute, the servo rotates from 90° to 0° for **5 seconds** (releasing food), then returns to 90° (stopped).

### Phase 4 – Failsafe
- The last feeding timestamp (hour, minute, second) and initialization flag are stored in flash.
- On every loop iteration, the system checks whether the elapsed time since the last stored feeding exceeds the scheduled interval.
- If a missed cycle is detected (e.g., after a reboot or power cut), the feeder activates immediately and a notification is sent to the user reporting the number of missed periods.


## Hardware

### Components

| Component | Quantity |
|---|---|
| ESP32 Wi-Fi Module | 1 |
| pH Sensor Module | 1 |
| DS18B20 Temperature Sensor | 1 |
| OLED Display (128×64, I2C) | 1 |
| Breadboard | 1 | 
| Continuous Rotation Servo Motor | 1 |
| Resistors & Wires | — |
| 3D-Printed Feeder Parts | 1 set |

### Circuit

The system wiring is as follows:

| Component | ESP32 Pin |
|---|---|
| DS18B20 Temperature Sensor (Data) | GPIO 4 |
| DS18B20 Pull-up Resistor (1 kΩ) | Between GPIO 4 and 3.3 V |
| pH Sensor (Analog Output – Po pin) | GPIO 34 |
| OLED Display (SDA) | GPIO 21 |
| OLED Display (SCL) | GPIO 22 |
| Servo Motor (Signal) | GPIO 27 |


![image alt](https://github.com/joyontadebnath/Water-Quality-Monitoring-and-Automated-Fish-Feeder-System/blob/ff33aee1f220c343e2a3631c102326babab767d8/images/circuit_diagram.png)

![image alt](https://github.com/joyontadebnath/Water-Quality-Monitoring-and-Automated-Fish-Feeder-System/blob/ff33aee1f220c343e2a3631c102326babab767d8/images/practical_circuit.png)


### Mechanical Feeder

The feeder is built from 3D-printed parts based on an open-source auger design. It consists of three main parts:

- **Auger (spiral screw)** — attached to the servo shaft; rotates to push food forward.
- **Motor holder** — clamps the servo and aligns it with the auger tube.
- **Auger tube** — channel through which food travels from the storage container into the water.

The feeder attaches to a food container (e.g., a bottle) positioned above the aquarium. When the servo rotates, the auger pushes a fixed volume of food down the tube and into the water.

![image alt](https://github.com/joyontadebnath/Water-Quality-Monitoring-and-Automated-Fish-Feeder-System/blob/ff33aee1f220c343e2a3631c102326babab767d8/images/feeder_3d_parts.png)

![image alt](https://github.com/joyontadebnath/Water-Quality-Monitoring-and-Automated-Fish-Feeder-System/blob/ff33aee1f220c343e2a3631c102326babab767d8/images/mechanical_feeder.png)

---

### Hardware Build


![image alt](https://github.com/joyontadebnath/Water-Quality-Monitoring-and-Automated-Fish-Feeder-System/blob/ff33aee1f220c343e2a3631c102326babab767d8/images/hardware_overview.png)


![image alt](https://github.com/joyontadebnath/Water-Quality-Monitoring-and-Automated-Fish-Feeder-System/blob/ff33aee1f220c343e2a3631c102326babab767d8/images/demo_setup.png)

---

## Firmware

### Libraries

Install the following libraries via the Arduino IDE Library Manager or PlatformIO before compiling:

| Library | Purpose |
|---|---|
| `BlynkSimpleEsp32` | Blynk IoT platform integration |
| `OneWire` | One-wire bus communication |
| `DallasTemperature` | DS18B20 sensor driver |
| `ESP32Servo` | Servo motor control on ESP32 |
| `Adafruit_GFX` | Graphics primitives for display |
| `Adafruit_SSD1306` | OLED display driver |
| `Preferences` (built-in) | Non-volatile flash storage |
| `time.h` (built-in) | NTP time synchronization |
| `WiFi.h` (built-in) | Wi-Fi connectivity |

---

### Pin Assignments

```cpp
#define ONE_WIRE_BUS  4    // DS18B20 data line
#define PH_PIN        34   // pH sensor analog output
#define SERVO_PIN     27   // Servo motor PWM signal
// OLED: SDA → GPIO 21, SCL → GPIO 22 (I2C default)
```

---

### How the Code Works

#### pH Reading & Filtering

The pH sensor outputs an analog voltage. The ADC reads a 12-bit value (0–4095), which is scaled to the 0–3.3 V range. A circular buffer of 20 samples is maintained; the average voltage is computed and then converted to a pH value using a two-segment piecewise linear equation derived from buffer-solution calibration:

```cpp
float voltage_to_pH(float voltage) {
  if (voltage > 2.555)
    return -5.505316423 * voltage + 20.93293045;
  else
    return -5.550239234 * voltage + 21.03611085;
}
```

#### Feeding Schedule & Missed-Feed Recovery

The `checkFeedTime()` function runs every loop iteration. It reads the last stored feeding timestamp from flash memory (`Preferences`), compares it with the current NTP time, and decides one of three outcomes:

1. **First boot** — initializes the stored timestamp to now.
2. **Normal feed** — current time = last time + 1 minute → activate servo for 5 s, update stored timestamp.
3. **Missed feed** — current time > last time + 1 minute → immediately activate servo, log a Blynk event with the count of missed periods, update stored timestamp.

#### Blynk Alerts

Alerts fire if readings fall outside safe bounds:

```cpp
if (PHValue > 8.5 || PHValue < 5.5)
  Blynk.logEvent("ph_limit", "Warning: pH out of range!");

if (tempC > 33.0 || tempC < 28.0)
  Blynk.logEvent("temp_limit", "Warning: Temperature out of range!");
```

---

### Setup & Flash Instructions

1. **Install Arduino IDE** and add the ESP32 board package via Boards Manager (`https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`).
2. **Install all libraries** listed in the [Libraries](#libraries) section.
3. **Create a Blynk project:**
   - Sign up at [blynk.cloud](https://blynk.cloud)
   - Create a new template named `fishfeeder micro`
   - Add two Gauge widgets: V0 (Temperature), V1 (pH)
   - Copy your `BLYNK_TEMPLATE_ID` and `BLYNK_AUTH_TOKEN`
4. **Edit the firmware** (`Final_Project_2.ino`) — update the following lines with your own credentials:

```cpp
#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN    "YOUR_AUTH_TOKEN"

char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";
```
5. **Select board:** `ESP32 Dev Module` in Tools → Board.
6. **Select port** and click **Upload**.
7. Place the pH sensor and temperature sensor in the water. Connect a power source. The OLED will display `System Starting...` on boot, then begin showing live readings.

---

## Blynk App Integration

The Blynk mobile app serves as the remote user interface. It provides:

- **Live gauges** for temperature (V0) and pH (V1), updating every second.
- **Historical graphs** to visualize trends over minutes, hours, or days.
- **Push notifications** via Blynk Events:
  - `ph_limit` → pH out of 5.5–8.5 range
  - `temp_limit` → Temperature out of 28–33 °C range
  - `feeding_time` → Missed feeding cycle detected and compensated


![image alt](https://github.com/joyontadebnath/Water-Quality-Monitoring-and-Automated-Fish-Feeder-System/blob/ff33aee1f220c343e2a3631c102326babab767d8/images/blynk_ui.png)

![image alt](https://github.com/joyontadebnath/Water-Quality-Monitoring-and-Automated-Fish-Feeder-System/blob/ff33aee1f220c343e2a3631c102326babab767d8/images/blynk_notifications.jpg)

![image alt](https://github.com/joyontadebnath/Water-Quality-Monitoring-and-Automated-Fish-Feeder-System/blob/ff33aee1f220c343e2a3631c102326babab767d8/images/oled_output.png)

---

## Sensor Calibration

### pH Sensor

The pH sensor was calibrated using **standard buffer solutions** at pH 4.01, 6.86, and 9.18 (at 25 °C). For each known pH solution, the output voltage was measured. A piecewise linear equation was then fitted to map voltage → pH. The calibration curve was split at 2.555 V to improve accuracy across the full range.


![image alt](https://github.com/joyontadebnath/Water-Quality-Monitoring-and-Automated-Fish-Feeder-System/blob/ff33aee1f220c343e2a3631c102326babab767d8/images/ph_buffer_solutions.png)

![image alt](https://github.com/joyontadebnath/Water-Quality-Monitoring-and-Automated-Fish-Feeder-System/blob/ff33aee1f220c343e2a3631c102326babab767d8/images/ph_calibration_curve.png)

After calibration, measured pH values were approximately **2–3% off** from reference values. A 20-sample moving average filter is applied in firmware to further reduce noise.

### Temperature Sensor (DS18B20)

The DS18B20 was verified against a reference laboratory thermometer. Readings matched closely with no significant offset, so no software correction factor was needed.

| Parameter | Value |
|---|---|
| Temperature Range | −55°C to +125°C |
| Accuracy | ±0.5°C (from −10°C to +85°C) |

---

## Failsafe Mechanism

The failsafe is designed for environments with frequent power outages (common in Bangladesh). It works as follows:

1. **Every feeding event**, the timestamp (hour, minute, second) is written to ESP32 flash.
2. On **every boot**, the stored timestamp is loaded back from flash.
3. On **every loop iteration**, the elapsed time since the last stored feed is compared against the scheduled interval.
4. If the elapsed time exceeds one interval, the system treats it as a **missed feeding**, immediately activates the feeder, and sends a Blynk notification: *"Missed a feeding cycle for N periods."*

This design means the system does **not** rely on an internal RTC or battery-backed clock — it uses NTP time from the internet after reconnecting to Wi-Fi.

**Current limitation:** Only one missed cycle can be fully compensated per recovery event. Extended outages spanning many cycles are partially handled (one compensatory feed is given), but full multi-cycle recovery is reserved for a future hardware revision with battery backup.

---

## Results & Evaluation

| Test | Outcome |
|---|---|
| pH sensor accuracy | Readings within **2–3%** of reference buffer values |
| Temperature accuracy | Matched reference thermometer; no correction needed |
| Blynk data transmission | Temperature and pH displayed correctly in real time |
| Push notifications | Fired correctly when thresholds were exceeded |
| Automated feeding | Correct food amount dispensed at scheduled intervals; no over/under-feeding |
| Power failure recovery | System successfully resumed feeding and restored all settings after simulated outage |

---

## Limitations

- **Wi-Fi dependency:** The system requires an active internet connection to fetch local time from the NTP server. Without Wi-Fi, time-based scheduling stops working.
- **Single missed-cycle recovery:** The current failsafe reliably handles one missed feeding per outage. Extended blackouts may cause additional missed cycles.
- **Sensor sensitivity:** Physical disturbances to the pH or temperature sensors can produce incorrect transient readings. The moving average filter mitigates this but does not eliminate it.
- **No offline clock:** There is no battery-backed RTC. If Wi-Fi is unavailable on reboot, the system cannot schedule feedings until connectivity is restored.

---

## Team

Ahamad Abtahi, Md Nazmus Shakib Tushar, Md. Maksudur Rahman Turzo and Joyonta Debnath


## References

1. G. Ignisha Rajathi, L. R. Priya, R. Vedhapriyavadhana, and K. Deepthyka, "Aqua Optimize Transformation of Water Management With Cloud-Powered Efficiency," *2024 International Conference on Modeling, Simulation & Intelligent Computing (MoSICom)*, pp. 501–506, Dec. 2024. [DOI: 10.1109/mosicom63082.2024.10882034](https://doi.org/10.1109/mosicom63082.2024.10882034)

2. "IoT Sensors for Water Quality Monitoring in Aquaculture Systems: A Systematic Review," *ProQuest*, 2025. [Link](https://www.proquest.com/openview/8c1d4316138b0a5b338d0e402061d187/1)

3. Shareef, Z.; Reddy, S.R.N. Design and Wireless Sensor Network Analysis of Water Quality Monitoring System for Aquaculture.

4. Kumar, M.P.; Monisha, J.; Pravenisha, R.; Praiselin, V.; Devi, K.S. The Real Time Monitoring of Water Quality in IoT Environment.

5. Lee, P.G. A Review of Automated Control Systems for Aquaculture and Design Criteria for Their Implementation. *Aquac. Eng.* 1995, 14, 205–227.

6. Mechanical Fish Feeder 3D Model — [Thingiverse Thing:301532](https://www.thingiverse.com/thing:301532)

---

*EEE 416 Final Project · Bangladesh University of Engineering and Technology · January 2025*
