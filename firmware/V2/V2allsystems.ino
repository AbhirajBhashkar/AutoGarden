#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// -------------------- Pins --------------------
#define LED_PIN   4

#define SDA_PIN   9
#define SCL_PIN   8

// IMPORTANT: Change this to the GPIO connected to the soil sensor signal.
#define SOIL_PIN  6

#define PUMP_PIN 5

// -------------------- OLED --------------------
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1



// -------------------- Watering Settings --------------------
const int WATER_THRESHOLD = 50;  // Water when soil moisture < 50%
const int PUMP_TIME = 2000;      // Pump runs for 2 seconds
const int SOAK_TIME = 5000;      // Wait 5 seconds after watering



Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// -------------------- Sensors --------------------
Adafruit_BME280 bme;

bool bmeFound = false;
bool oledFound = false;

uint8_t bmeAddress = 0;
uint8_t oledAddress = 0;

// ------------------------------------------------

void blinkLED(int onTime, int offTime, int count)
{
  for (int i = 0; i < count; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(onTime);

    digitalWrite(LED_PIN, LOW);
    delay(offTime);
  }
}

bool i2cDeviceExists(uint8_t address)
{
  Wire.beginTransmission(address);
  return Wire.endTransmission() == 0;
}

bool initializeBME280()
{
  if (i2cDeviceExists(0x76) && bme.begin(0x76))
  {
    bmeAddress = 0x76;
    return true;
  }

  if (i2cDeviceExists(0x77) && bme.begin(0x77))
  {
    bmeAddress = 0x77;
    return true;
  }

  return false;
}

bool initializeOLED()
{
  if (i2cDeviceExists(0x3C))
  {
    oledAddress = 0x3C;

    if (display.begin(
          SSD1306_SWITCHCAPVCC,
          oledAddress
        ))
    {
      return true;
    }
  }

  if (i2cDeviceExists(0x3D))
  {
    oledAddress = 0x3D;

    if (display.begin(
          SSD1306_SWITCHCAPVCC,
          oledAddress
        ))
    {
      return true;
    }
  }

  return false;
}

int readSoilSensor()
{
  const int sampleCount = 20;
  long total = 0;

  for (int i = 0; i < sampleCount; i++)
  {
    total += analogRead(SOIL_PIN);
    delay(5);
  }

  return total / sampleCount;
}


int getSoilMoisturePercent()
{
  const int samples = 20;
  long total = 0;

  for (int i = 0; i < samples; i++)
  {
    total += analogRead(SOIL_PIN);
    delay(5);
  }

  int raw = total / samples;

  // Replace these with YOUR measured values
  const int DRY_VALUE = 3000;
  const int WET_VALUE = 1300;

  int moisture = map(raw, DRY_VALUE, WET_VALUE, 0, 100);

  // Prevent values below 0% or above 100%
  moisture = constrain(moisture, 0, 100);

  return moisture;
}


void printI2CDevices()
{
  Serial.println("Scanning I2C bus...");

  int devicesFound = 0;

  for (uint8_t address = 1; address < 127; address++)
  {
    if (i2cDeviceExists(address))
    {
      Serial.print("I2C device found at 0x");

      if (address < 16)
      {
        Serial.print("0");
      }

      Serial.println(address, HEX);
      devicesFound++;
    }
  }

  if (devicesFound == 0)
  {
    Serial.println("No I2C devices found.");
  }

  Serial.println();
}

void showStartupScreen()
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("AutoGarden V2");
  display.println();

  display.print("BME280: ");
  display.println(bmeFound ? "OK" : "FAIL");

  display.print("Soil ADC: GPIO ");
  display.println(SOIL_PIN);

  display.println();
  display.println("Starting...");

  display.display();
}

void pumpOn()
{
  digitalWrite(PUMP_PIN, HIGH);
  Serial.println("PUMP ON");

  if (oledFound)
  {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);

    display.println("AutoGarden V2");
    display.println();
    display.println("WATERING...");
    display.display();
  }
}

void pumpOff()
{
  digitalWrite(PUMP_PIN, LOW);
  Serial.println("PUMP OFF");
}

void waterPlant()
{

  Serial.println("Soil is dry - watering!");
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);

  display.println("AutoGarden V2");
  display.println();
  display.println("Watering...");
  display.display();

  blinkLED(60, 30, 5);

  // delay(PUMP_TIME);
  pumpOn();
  delay(PUMP_TIME);


  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("AutoGarden V2");
  display.println();
  display.println("Soaking...");
  display.display();

  // delay(SOAK_TIME);  
  pumpOff();

  Serial.println("Waiting for water to soak in...");
  delay(SOAK_TIME);
}


void setup()
{
  Serial.begin(115200);
  delay(2000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(SOIL_PIN, INPUT);

  pinMode(PUMP_PIN, OUTPUT);

// VERY important: make sure pump starts OFF
  digitalWrite(PUMP_PIN, LOW);


  analogReadResolution(12);

  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println();
  Serial.println("========================");
  Serial.println("AutoGarden V2 Test");
  Serial.println("========================");

  blinkLED(100, 100, 5);

  printI2CDevices();

  bmeFound = initializeBME280();
  oledFound = initializeOLED();

  Serial.print("BME280: ");
  Serial.println(bmeFound ? "FOUND" : "NOT FOUND");

  if (bmeFound)
  {
    Serial.print("BME280 address: 0x");
    Serial.println(bmeAddress, HEX);
  }

  Serial.print("OLED: ");
  Serial.println(oledFound ? "FOUND" : "NOT FOUND");

  if (oledFound)
  {
    Serial.print("OLED address: 0x");
    Serial.println(oledAddress, HEX);

    showStartupScreen();
  }

  if (bmeFound && oledFound)
  {
    // Everything initialized
    blinkLED(80, 80, 2);
  }
  else
  {
    // Something failed
    blinkLED(500, 300, 3);
  }
}

void loop()
{
  int soilRaw = readSoilSensor();

  float soilVoltage =
    soilRaw * (3.3 / 4095.0);

  float temperature = NAN;
  float humidity = NAN;
  float pressure = NAN;

  if (bmeFound)
  {
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    pressure = bme.readPressure() / 100.0F;
  }

  // ---------------- Serial output ----------------

  Serial.println("------------------------");

  if (bmeFound)
  {
    Serial.print("Temperature: ");
    Serial.print(temperature, 2);
    Serial.println(" C");

    Serial.print("Humidity: ");
    Serial.print(humidity, 2);
    Serial.println(" %");

    Serial.print("Pressure: ");
    Serial.print(pressure, 2);
    Serial.println(" hPa");
  }
  else
  {
    Serial.println("BME280 unavailable");
  }

  Serial.print("Soil raw ADC: ");
  Serial.println(soilRaw);

  Serial.print("Soil voltage: ");
  Serial.print(soilVoltage, 3);
  Serial.println(" V");

  int soilMoisture = getSoilMoisturePercent();

  Serial.print("Soil Moisture: ");
  Serial.print(soilMoisture);
  Serial.println("%");

  // if (soilMoisture < WATER_THRESHOLD)
  // {
  //   waterPlant();
  // }
  // ---------------- OLED output ----------------

  if (oledFound)
  {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(0, 0);
    display.println("AutoGarden V2");

    if (bmeFound)
    {
      display.print("Temp: ");
      display.print(temperature, 1);
      display.println(" C");

      display.print("Humidity: ");
      display.print(humidity, 1);
      display.println(" %");

      display.print("Pressure: ");
      display.print(pressure, 0);
      display.println(" hPa");
    }
    else
    {
      display.println("BME280: ERROR");
    }

    display.println();

    // display.print("Soil raw: ");
    // display.println(soilRaw);

    // display.print("Voltage: ");
    // display.print(soilVoltage, 2);
    // display.println(" V");

    display.print("Soil: ");
    display.print(soilMoisture);
    display.println("%");

    display.display();

    if (soilMoisture < WATER_THRESHOLD)
    {
      // display.clearDisplay();
      // display.setTextColor(SSD1306_WHITE);
      // display.setTextSize(1);
      // display.setCursor(0, 0);

      // display.println("AutoGarden V2");
      // display.println();
      // display.println("Watering...");
      // display.display();

      // blinkLED(60, 30, 5);

      // delay(PUMP_TIME);

      // display.clearDisplay();
      // display.setCursor(0, 0);
      // display.println("AutoGarden V2");
      // display.println();
      // display.println("Soaking...");
      // display.display();

      // delay(SOAK_TIME);
      delay(5000);
      waterPlant();

    }

  }

  

  // Two quick flashes = main loop working
  blinkLED(60, 60, 2);

  delay(1800);
}
