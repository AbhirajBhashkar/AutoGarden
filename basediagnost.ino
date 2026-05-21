#include <HardwareSerial.h>

HardwareSerial loraSerial(2);

void setup() {
  Serial.begin(115200);
  loraSerial.begin(9600, SERIAL_8N1, 16, 17);

  delay(1000);
  Serial.println("RYLR diagnostic ready.");
  Serial.println("Type AT, AT+ADDRESS?, etc.");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    Serial.print("Sending to module: ");
    Serial.println(cmd);

    loraSerial.print(cmd);
    loraSerial.print("\r\n");
  }

  while (loraSerial.available()) {
    Serial.write(loraSerial.read());
  }
}