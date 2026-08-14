#include <HardwareSerial.h>

HardwareSerial loraSerial(2);

int counter = 0;

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

  Serial.println("Sensor node starting...");

  sendCommand("AT");
  sendCommand("AT+ADDRESS=2");
  sendCommand("AT+BAND=915000000");
  //sendCommand("AT+PARAMETER=10,7,1,7");
}

void loop() {
  String msg = "NODE2,COUNT:" + String(counter);
  String cmd = "AT+SEND=1," + String(msg.length()) + "," + msg;

  Serial.println("Sending: " + msg);
  sendCommand(cmd);

  counter++;
  delay(3000);
}
