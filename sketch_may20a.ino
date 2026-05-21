#include <HardwareSerial.h>

HardwareSerial loraSerial(2);

void sendCommand(String cmd) {
  loraSerial.print(cmd + "\r\n");
  delay(200);

  while (loraSerial.available()) {
    Serial.write(loraSerial.read());
  }
}

void setup() {
  Serial.begin(115200);
  loraSerial.begin(115200, SERIAL_8N1, 16, 17);

  delay(1000);

  Serial.println("Base station starting...");

  sendCommand("AT");
  sendCommand("AT+ADDRESS=1");
  sendCommand("AT+BAND=915000000");
  //sendCommand("AT+PARAMETER=10,7,1,7");

  Serial.println("Listening for LoRa packets...");
}

void loop() {
  while (loraSerial.available()) {
    String incoming = loraSerial.readStringUntil('\n');
    incoming.trim();

    if (incoming.length() > 0) {
      Serial.println("RX: " + incoming);
    }
  }
}