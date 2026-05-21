# AutoGarden
My AutoGarden IOT project


Here I am documenting the development of AutoGarden. It is an IOT Project I am designing. The idea is that ultimately it will be able to automate the gardening process to help gardeners and others growing plants. The first goal is to be able to sense environmental attributes and warn the user using IOT or radio or some communication method.

UPDATES:

May 16th:

Built first circuit. Able to measure soil moisture. Outputted on OLED screen.

May 17th:
Added BME280 sensor. Measured temperature, humidity, and pressure. Updated OLED screen. Tested dimming for energy saving in future.

May 18th:
Added REYAX RYLR998 Radio module. Tested if it outputted using my SDR, was able to see a pulse at 915MHz, so it was a success.

May 19th:
Attempted to set up ESPHOME on Home Assistant. Experienced issues with connecting, will try again later.

May20th:
Established Radio connection with another ESP32 with the same radio module. Able to send test messages to count. Very few error signals.
