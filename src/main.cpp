#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

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

//Buzzer
#define BUZZER_PIN 26 // Define the pin number for the buzzer
void lcdHeading(){
  lcd.setCursor(0, 0);
  lcd.print("  ESP Smart planter  ");
}

void setup(void) {
  Serial.begin(9600);
  Serial.println("Hello!");

  // Initialize the I2C communication
  Wire.begin(); // SDA pin 21, SCL pin 22

  // Initialize the LCD
  lcd.init();
  // Turn on the lcd backlight
  lcd.backlight();

  lcdHeading();


  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    lcd.setCursor(0, 3);
    lcd.print("Didn't find PN53x!");
    while (1); // Halt
  }

  // Got ok data, print it out!
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

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Buffer to store the returned UID
  uint8_t uidLength; // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an RFID card to be present
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

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
      Serial.print(" 0x");Serial.print(uid[i], HEX);
    }
    Serial.println("");

    // Define the authorized UIDs
    uint8_t authorizedUID1[] = {0xF3, 0x47, 0xD, 0x19};
    uint8_t authorizedUID2[] = {0xA7, 0x43, 0x4A, 0x9C};

    // Compare the UID of the card with authorized UIDs
    if (memcmp(uid, authorizedUID1, uidLength) == 0 || memcmp(uid, authorizedUID2, uidLength) == 0) {
      Serial.println("Tomatoes");
      lcd.clear();
      lcdHeading();
      lcd.setCursor(0, 1);
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

    delay(1000);
  }
}
