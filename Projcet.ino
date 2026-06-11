#define BLYNK_TEMPLATE_ID "**********"
#define BLYNK_TEMPLATE_NAME "*******************"
#define BLYNK_AUTH_TOKEN "*******************"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "time.h"
#include <Preferences.h>


Preferences preferences;

#define ONE_WIRE_BUS 4
#define PH_PIN 34
#define SERVO_PIN 27

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

char ssid[] = "*******************";
char pass[] = "*******************";

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
BlynkTimer timer;
Servo feederServo;

const int Number_Of_Sample = 20;
float Array_Of_PH[Number_Of_Sample] = {0};
int phIndex = 0;
bool Array_is_Full = false;

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 6 * 3600;  // GMT+6
const int daylightOffset_sec = 0;

void samplePH() {
  int PH_Binary = analogRead(PH_PIN);
  float PH_Voltage = PH_Binary * (3.3 / 4095.0);
  Array_Of_PH[phIndex] = PH_Voltage;
  phIndex = (phIndex + 1) % Number_Of_Sample;
  if (phIndex == 0) Array_is_Full = true;
}

float Get_Average_PH_Voltage() {
  samplePH();
  float sum = 0;
  int count = Array_is_Full ? Number_Of_Sample : phIndex;
  for (int i = 0; i < count; i++) sum += Array_Of_PH[i];
  return (count > 0) ? sum / count : 0;
}

float voltage_to_pH(float voltage) {
  if (voltage > 2.555) return -5.505316423 * voltage + 20.93293045;
  else return -5.550239234 * voltage + 21.03611085;
}

float Get_Average_PH() {
  float avgVoltage = Get_Average_PH_Voltage();
  return voltage_to_pH(avgVoltage);
}

void updateOLED(float tempC, float PHValue) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Water Monitor");

  // Time Display
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeStr[16];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
    display.setCursor(0, 16);
    display.print("Time: ");
    display.println(timeStr);
  }

  display.setCursor(0, 32);
  display.print("Temp: ");
  display.print(tempC, 2);
  display.println(" C");

  display.setCursor(0, 48);
  display.print("pH: ");
  display.print(PHValue, 2);
  display.display();
}

void sendSensorData() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  float PHValue = Get_Average_PH();

 Blynk.virtualWrite(V0, tempC);
 Blynk.virtualWrite(V1, PHValue);

  updateOLED(tempC, PHValue);

  if (PHValue > 8.5 || PHValue < 5.5)
 Blynk.logEvent("ph_limit", "Warning: pH out of range!");
  if (tempC > 33.0 || tempC < 28.0)
 Blynk.logEvent("temp_limit", "Warning: Temperature out of range!");

  Serial.print("Temp: "); Serial.print(tempC);
  Serial.print(" C | pH: "); Serial.print(PHValue);
  Serial.println();
}
/*
void checkFeedTime() {
  static int lastMin = -1;

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    int currentMin = timeinfo.tm_min;
    int currentSec = timeinfo.tm_sec;

    if (currentSec == 0 && currentMin != lastMin) {
      if (lastMin != -1) {  // skip this check on the first run
        int diff = (currentMin - lastMin + 60) % 60;
        if (diff > 0) {
          Serial.println("Missed feeding time detected!");
          Blynk.logEvent("feeding_time", "Missed a feeding cycle.");
        }
      }

      lastMin = currentMin;

      Serial.println("Feeding fish...");
      feederServo.write(0);
      delay(5000);
      feederServo.write(90);
    }
  }
}


*/


void checkFeedTime() {
  static int mark = -1;
  static int LastHour;
  static int LastMin;
  static int LastSec;

  preferences.begin("random2", false);
  mark = preferences.getInt("num1", 0);
  LastHour = preferences.getInt("num2", 0);
  LastMin = preferences.getInt("num3", 0);
  LastSec = preferences.getInt("num4", 0);
  preferences.end();

  struct tm timeinfo;

  if (getLocalTime(&timeinfo)) {
    int currentHour = timeinfo.tm_hour;
    int currentMin = timeinfo.tm_min;
    int currentSec = timeinfo.tm_sec;

    if (mark == -1) {
      // First time initialization
      LastHour = currentHour;
      LastMin = currentMin;
      LastSec = currentSec;
      mark = 0;
      preferences.begin("random2", false);
    preferences.putInt("num1", mark);
    preferences.putInt("num2", LastHour);
    preferences.putInt("num3", LastMin);
    preferences.putInt("num4", LastSec);
    preferences.end();

      
    } else if ((currentHour == LastHour) && (currentMin == LastMin + 1) && (currentSec == 0)) {
      // Feed normally at start of every minute
      LastHour = currentHour;
      LastMin = currentMin;
      LastSec = currentSec;

      Serial.println("Feeding fish...");
      feederServo.write(0);
      delay(5000);
      feederServo.write(90);

      preferences.begin("random2", false);
      preferences.putInt("num1", mark);
      preferences.putInt("num2", LastHour);
      preferences.putInt("num3", LastMin);
      preferences.putInt("num4", LastSec);
      preferences.end();

      
    } else if ((currentHour > LastHour) || (currentMin > LastMin + 1)) {
      // Missed one or more feeding cycles
      int missed = (currentHour - LastHour) * 60 + (currentMin - LastMin);
      if (missed < 0) missed += 1440;  // handle wrap-around at midnight

      char msg[64];
      sprintf(msg, "Missed a feeding cycle for %d periods.", missed);
      Blynk.logEvent("feeding_time", msg);
      
      Serial.printf("Missed %d cycles. Compensating now...\n", missed);

        Serial.println("Feeding fish...");
        feederServo.write(0);
        delay(5000);
        feederServo.write(90);
        delay(1000);  // Small delay between feedings

      LastHour = currentHour;
      LastMin = currentMin;
      LastSec = currentSec;
      mark = 0;
      preferences.begin("random2", false);
      preferences.putInt("num1", mark);
      preferences.putInt("num2", LastHour);
      preferences.putInt("num3", LastMin);
      preferences.putInt("num4", LastSec);
      preferences.end();

      
    }
  }
}


// This will execute once
void setup() {

  // Begining the stuffs.
  Serial.begin(115200);
  sensors.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Servo OFF 
  feederServo.attach(SERVO_PIN);
  feederServo.write(90);

  // Time Configure
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Display initialize
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (true);
  }

  // Display Setup
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("System Starting...");
  display.display();
  delay(1000);

  timer.setInterval(1000L, samplePH);
  timer.setInterval(5000L, sendSensorData);
}

void loop() {
  Blynk.run();  
  timer.run();
  checkFeedTime();
}
