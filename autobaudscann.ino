#include <HardwareSerial.h>

HardwareSerial loraSerial(2);

int baudRates[] = {9600, 19200, 38400, 57600, 115200};

void tryBaud(int baud) {
  Serial.println();
  Serial.print("Trying LoRa baud: ");
  Serial.println(baud);

  loraSerial.end();
  delay(200);
  loraSerial.begin(baud, SERIAL_8N1, 16, 17);
  delay(500);

  loraSerial.print("AT\r\n");

  unsigned long start = millis();
  while (millis() - start < 1000) {
    while (loraSerial.available()) {
      Serial.write(loraSerial.read());
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("RYLR baud scan starting...");

  for (int i = 0; i < 5; i++) {
    tryBaud(baudRates[i]);
  }

  Serial.println();
  Serial.println("Scan finished.");
}

void loop() {}