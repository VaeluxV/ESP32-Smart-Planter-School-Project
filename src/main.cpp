/* --- Libraries --- */
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// OneWire bus (soil temperature sensor)
#define ONE_WIRE_BUS 33
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass the oneWire reference to the Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

// LCD stuff
#define LCD_ADDRESS 0x27

#define LCD_COLUMNS 20
#define LCD_ROWS 4

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

//SPI (RFID reader) stuff
#define SS_PIN   5 // Set the SPI SS PIN
#define SCK_PIN  18
#define MOSI_PIN 23
#define MISO_PIN 19
#define RST_PIN  25 // Set the PN532 RST PIN

Adafruit_PN532 nfc(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);

// unsigned long previousTempMillis = 0;  // Variable to store the last time temperature was printed
// const long tempInterval = 1000;         // Interval at which to print temperature (in milliseconds)

//Buzzer
#define BUZZER_PIN 26 // Define the pin number for the buzzer

void rfidRead();

void lcdHeading();

void setup() {
  Serial.begin(9600);
  Serial.println("Hello!");

  // Initialize the I2C communication
  Wire.begin(); // SDA pin 21, SCL pin 22

  // Initialize the LCD
  lcd.init();
  // Turn on the lcd backlight
  lcd.backlight();

  lcdHeading();

  // Initialize the DS18B20 sensor
  sensors.begin();

  // Initialize the RFID reader
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    lcd.setCursor(0, 3);
    lcd.print("Didn't find PN53x!");
    while (1); // Halt
  }

  // Got good data, printing it. (firmware version & chip type)
  Serial.print("Found chip PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

  lcd.setCursor(0, 3);
  lcd.print("Found RFID reader!");
  delay(250);

  // Configure board to read RFID tags
  nfc.SAMConfig();
  Serial.println("Waiting for an RFID card...");
  lcd.clear();

  lcdHeading();
  lcd.setCursor(0, 3);
  lcd.print("Found RFID reader!");

  delay(250);

  lcd.clear();
  lcdHeading();
}

void loop() {
  unsigned long currentTime = millis(); // Get the current time in milliseconds

  // Get soil temperatures
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);

  // Debug print temperatures
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.println(" °C");

  if (currentTime % 10 == 0) {
    rfidRead();
  }
}

void lcdHeading(){
  lcd.setCursor(0, 0);
  lcd.print("  ESP Smart planter  ");
}

void rfidRead() {
  // Check for an RFID card
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Buffer to store the returned UID
  uint8_t uidLength; // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    Serial.println("Found an RFID card!");
    lcd.clear();
    lcdHeading();
    lcd.setCursor(0, 1);
    lcd.print("Found an RFID card!");

    Serial.print("UID Length: ");
    Serial.print(uidLength, DEC);
    Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(" 0x");
      Serial.print(uid[i], HEX);
    }
    Serial.println("");

    // Define the authorized UIDs
    uint8_t tomatoesUID[] = {0xF3, 0x47, 0xD, 0x19};
    uint8_t cabbageUID[] = {0xA3, 0xD4, 0x14, 0x19};

    // Compare the UID of the card with authorized UIDs
    if (memcmp(uid, tomatoesUID, uidLength) == 0) {
      Serial.println("Tomatoes");
      lcd.clear();
      lcdHeading();
      lcd.setCursor(0, 1);
      lcd.print("Target set:");
      lcd.setCursor(0, 2);
      lcd.print("Tomatoes");
      tone(BUZZER_PIN, 950);
      delay(75);
      noTone(BUZZER_PIN);
      delay(50);
      tone(BUZZER_PIN, 1150);
      delay(75);
      noTone(BUZZER_PIN);
      delay(300);
      lcd.clear();
      lcdHeading();
    } else if (memcmp(uid, cabbageUID, uidLength) == 0) {
      Serial.println("Cabbage");
      lcd.clear();
      lcdHeading();
      lcd.setCursor(0, 1);
      lcd.print("Target set:");
      lcd.setCursor(0, 2);
      lcd.print("Cabbage");
      tone(BUZZER_PIN, 950);
      delay(75);
      noTone(BUZZER_PIN);
      delay(50);
      tone(BUZZER_PIN, 1150);
      delay(75);
      noTone(BUZZER_PIN);
      delay(300);
      lcd.clear();
      lcdHeading();
    } else {
      Serial.println("Error");
      lcd.clear();
      lcdHeading();
      lcd.setCursor(0, 1);
      lcd.print("Error, unrecognised.");
      tone(BUZZER_PIN, 600);
      delay(60);
      noTone(BUZZER_PIN);
      delay(40);
      tone(BUZZER_PIN, 550);
      delay(60);
      noTone(BUZZER_PIN);
      delay(40);
      tone(BUZZER_PIN, 500);
      delay(60);
      noTone(BUZZER_PIN);
      delay(240);
      lcd.clear();
      lcdHeading();
    }
  }
}


