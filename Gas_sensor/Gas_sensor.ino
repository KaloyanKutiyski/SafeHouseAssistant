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
  Serial.println("-------------------");
  Serial.println(sensorValueA);
  Serial.printf("alarm trigger %i\n", dangerLevel);

  lcd.setCursor(0, 0);
  lcd.print("Sensor:");
  lcd.setCursor(12, 0);
  lcd.print(sensorValueA);
  
  lcd.setCursor(0, 1);
  lcd.print("Limit:");
  lcd.setCursor(12, 1);
  lcd.print(dangerLevel);
 
  if (sensorValueA > dangerLevel)
  {
    if(millis() - last_stop > 10000)
    {
      digitalWrite(RELAY, LOW);
      Serial.println("Current Flowing");
    } else {
      Serial.println("muted, time remaining:");
      Serial.println(10000 - (millis() - last_stop));
    }
  }
 
  else
  {
    digitalWrite(RELAY, HIGH);
    Serial.println("Current not Flowing");
  }
}
 
void loop()
{
  Blynk.run();
  timer.run();
} 
