#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "eSmfIcFcORGmqTnF2XPOCLDrrTT-Z66p";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "6П";
char pass[] = "plsnosteal";

#define MQ2A 34
#define RELAY 26
#define BTN_PIN 2
#define SDA 21
#define SCL 22

BlynkTimer timer;
LiquidCrystal_I2C lcd(0x27, 16, 2);  

int sensorValueA = 0;
const int defaultDangerLevel = 1500;
int dangerLevel = defaultDangerLevel;
unsigned long last_stop = 0;
bool muted;
int secondsOnMute;
int muteMillis = 10000;

BLYNK_WRITE(V1) {
  dangerLevel = 1000 + 2 * (1023 - param.asInt());
}
 
void setup()
{
 Serial.begin(115200);
 pinMode(MQ2A, INPUT);
 pinMode(RELAY, OUTPUT);
 lcd.init();
 lcd.backlight();
 Blynk.begin(auth, ssid, pass);
 timer.setInterval(500L, readSensor); // read sensors every 500ms
 timer.setInterval(100L, readRemoteInput); // read data from mobile device every 100ms
 
}

void readRemoteInput()
{
  boolean isPressed = (digitalRead(BTN_PIN) == LOW);
  if (!isPressed) {
    Serial.println("muted");
    last_stop = millis();
  }
}

void readSensor()
{
  sensorValueA = analogRead(MQ2A);
  muted = millis() - last_stop <= muteMillis;
  
  Serial.println("-------------------");
  Serial.println(sensorValueA);
  Serial.printf("alarm trigger %i\n", dangerLevel);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SENSOR:");
  if (sensorValueA >= 1000){
    lcd.setCursor(7, 0);
  } else {
    lcd.setCursor(8, 0);
  }
  lcd.print(sensorValueA);
  lcd.setCursor(11, 0);
  lcd.print("/");
  lcd.setCursor(12, 0);
  lcd.print(dangerLevel);
  lcd.setCursor(6, 1);
  lcd.print("|");

  if (muted) {
    lcd.setCursor(7, 1);
    lcd.print("MUTE:");
    secondsOnMute = (muteMillis - millis() + last_stop) / 1000;
    if (secondsOnMute >= 1000) {
      lcd.setCursor(12, 1);
    } else {
      lcd.setCursor(13, 1);
    }
    lcd.print(secondsOnMute);
  } else {
    lcd.setCursor(8, 1);
    lcd.print("SOUND ON");
  }
 
  if (sensorValueA > dangerLevel)
  {
    lcd.setCursor(0, 1);
    lcd.print("DANGER");
    if (!muted)
    {
      digitalWrite(RELAY, LOW);
      Serial.println("Current Flowing");
    } else {
      Serial.println("muted, time remaining:");
      Serial.println(muteMillis - (millis() - last_stop));
    }
  }
  else
  {
    lcd.setCursor(1, 1);
    lcd.print("SAFE");
    digitalWrite(RELAY, HIGH);
    Serial.println("Current not Flowing");
  }
}
 
void loop()
{
  Blynk.run();
  timer.run();
} 
