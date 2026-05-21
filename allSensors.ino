#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define SDA_PIN 21
#define SCL_PIN 22
#define MOIST_PIN 34


  

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_BME280 bme;

unsigned long lastPageChange = 0;
const unsigned long pageInterval = 5000; // 5 seconds
int page = 0;
const int totalPages = 4;
const unsigned long sensorInterval = 500;     // sensor smoothing update every 0.5 sec

float alpha = 0.10;

float smoothMoisture = 0;
float smoothHumidity = 0;
float smoothTemp = 0;
float smoothPressure = 0;

bool firstRead = true;

// Replace with your real calibration values
int dryValue = 3200;
int wetValue = 1400;

int moisturePercent() {
  int raw = analogRead(MOIST_PIN);
  int pct = map(raw, dryValue, wetValue, 0, 100);
  return constrain(pct, 0, 100);
}

void showPage(String title, String value, String unit) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("AutoGarden v1");

  display.setTextSize(2);
  display.setCursor(0, 18);
  display.println(title);

  display.setTextSize(2);
  display.setCursor(0, 42);
  display.print(value);
  display.print(" ");
  display.print(unit);

  display.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed");
    while (true);
  }

  if (!bme.begin(0x76)) { // try 0x77 if needed
    Serial.println("BME280 failed");
    while (true);
  }

  display.clearDisplay();
  display.display();
}

void updateSmoothedValues() {
  int moisture = moisturePercent();
  float humidity = bme.readHumidity();
  float temp = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F;

  if (firstRead) {
    smoothMoisture = moisture;
    smoothHumidity = humidity;
    smoothTemp = temp;
    smoothPressure = pressure;
    firstRead = false;
  } else {
    smoothMoisture = alpha * moisture + (1 - alpha) * smoothMoisture;
    smoothHumidity = alpha * humidity + (1 - alpha) * smoothHumidity;
    smoothTemp = alpha * temp + (1 - alpha) * smoothTemp;
    smoothPressure = alpha * pressure + (1 - alpha) * smoothPressure;
  }
}

void drawCurrentPage() {
  if (page == 0) {
    showPage("Moisture", String((int)round(smoothMoisture)), "%");
  } else if (page == 1) {
    showPage("Temp", String(smoothTemp, 1), "C");
  } else if (page == 2) {
    showPage("Humidity", String(smoothHumidity, 0), "%");
  } else if (page == 3) {
    showPage("Pressure", String(smoothPressure, 0), "hPa");
  }
}

void loop() {
// smoothing factor: lower = smoother/slower


updateSmoothedValues();



if (millis() - lastPageChange >= pageInterval) {
  lastPageChange = millis();

  display.dim(true);
  delay(3000);

  page = (page + 1) % totalPages;
  drawCurrentPage();
  display.dim(false);
  delay(100);
  display.dim(true);
  delay(1000);
  display.clearDisplay();
  display.display();
}
}