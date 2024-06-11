# ESP32 Smart Planter
## Hardware info
### Required Hardware:
* ESP32 Devkit (30-pins)
* Soil moisture sensor
* PN532 rfid reader
* Passive buzzer
* LDR
* Ds18b20 Soil temperature sensor
* Water Level Sensor
* Bme280
* 5V 4-channel relay module (a 2-channel module can also be used if you do not plan to add more hardware yourself)
* I2C LCD (min. 4 lines, 20 characters per line)
* Wire
### Optional hardware:
* Custom PCB (gerber files can be found under [`/CustomPCB/Gerber`](https://github.com/VaeluxV/ESP32-Smart-Planter-School-Project/tree/main/CustomPCB/Gerber) and eagle schematics under [`/Eagle/ESP-Planter`](https://github.com/VaeluxV/ESP32-Smart-Planter-School-Project/tree/main/Eagle/ESP-Planter))

## Software info

### Required software:
* Arduino IDE or VS Code (Platformio)
### Required libraries:
* SPI
* Adafruit_PN532
* Wire
* LiquidCrystal_I2C
* Adafruit_Sensor
* Adafruit_BME280
* FastLED
* OneWire
* DallasTemperature
* WiFi
* PubSubClient
* freertos/FreeRTOS
* freertos/task

---

## Other info

This project works using Grafana, InfluxDB & MQTT installed on any device such as a Raspberry Pi. You can remove the Grafana & InfluxDB if you do not need to store any data but it is not recommended because I have not tested the full functionality without these programs. MQTT is required if you do not modify any code.

Make sure to enter your WiFi and MQTT details when uploading this code to your ESP32 or it won't work!

MQTT settings can be found at line 18-21 and WiFi settings can be found at line 32-33.
Other configurable settings can also be found near the top of the code. This can be used if your LCD has a different I2C adress (line 44) for example.

The InfluxDB connection requires a database names `planter_readings` to be present so make sure to make that before starting the MQTT bridge!

Connections schematic (default config):
![Image of the schematic](https://github.com/VaeluxV/ESP32-Smart-Planter-School-Project/blob/c1af1cb7f98856f32b2263611ea02691f5bdc3e4/images/SchematicSmartPlanter.jpg)
Please note that the 12v & 5v connections are external connections coming from a 12v PSU (& buck converter for the 5v) while the 3.3v is delivered by the ESP32's internal power conversion.

---

### Relays and 12v components
The 5v relay module, 12v fan & 12v water pump are not shown in this schematic but have to be connected too. The fan is connected to relay channel 1, while the pump is connected on channel 2. All of these share a common ground with the esp32 (through the buck converter).

### Debugging connection
Debugging can be done over serial (9600 baud) using the USB connection. Make sure to still plug in the PSU as otherwise there will not be enough power to run everything and the ESP32 will fail to boot properly!

---

You are free to open issues if you have any questions about something not documented here or otherwise not mentioned.

GLHF!

~ Vaelux
