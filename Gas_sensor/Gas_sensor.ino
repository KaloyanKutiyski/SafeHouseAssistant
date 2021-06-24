#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";

#define MQ2A 34
#define RELAY 26
#define BTN_PIN 2
#define SDA 21
#define SCL 22
#define GAUGE 2
#define SENSITIVITY_SLIDER_PIN V1
#define MUTE_DURATION_PIN V3

#define MILLIS_IN_SEC 1000

BlynkTimer timer;
LiquidCrystal_I2C lcd(0x27, 16, 2);  

int sensorValueA = 0;
const int defaultDangerLevel = 1500;
int dangerLevel = defaultDangerLevel;
unsigned long last_stop = 0;
unsigned long last_notify = 0;
bool muted;
int secondsOnMute;
int muteMillis = 10 * MILLIS_IN_SEC;
int muteMillisSetting;

BLYNK_WRITE(SENSITIVITY_SLIDER_PIN)
{
  dangerLevel = param.asInt();
}
BLYNK_WRITE(MUTE_DURATION_PIN)
{
  muteMillisSetting = param.asInt() * MILLIS_IN_SEC;
}
 
void setup()
{
 pinMode(MQ2A, INPUT);
 pinMode(RELAY, OUTPUT);
 lcd.init();
 lcd.backlight();
 Blynk.begin(auth, ssid, pass);
 timer.setInterval(500L, readSensor); // read sensors every 500ms
 timer.setInterval(100L, readRemoteInput); // read data from mobile device every 100ms
 timer.setInterval(100L, printDataToDevice); // prints data every 100ms to device
 timer.setInterval(1000L, millisOverflowProtection); // the millis() function overflows roughly every 7 weeks
  // in such case, if the timestamps for muting and notifying are not reset, those functionalities may stop working.
}

void millisOverflowProtection() {
  if (millis() < last_stop || millis() < last_notify) {
    last_stop = last_notify = 0;
  }
}

void printDataToDevice()
{
  Blynk.virtualWrite(GAUGE, sensorValueA);
}

void readRemoteInput()
{
  boolean isPressed = (digitalRead(BTN_PIN) == LOW);
  if (!isPressed)
  {
    muteMillis = muteMillisSetting;
    last_stop = millis();
  }
}

void notify(const char* msg)
{
  if(Blynk.connected() && millis()-last_notify > 60 * MILLIS_IN_SEC) 
  {
    Blynk.notify(msg);
    last_notify = millis();
  }
}

void printOnLCD()
{
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

  if (muted)
  {
    lcd.setCursor(7, 1);
    lcd.print("MUTE:");
    secondsOnMute = (muteMillis - millis() + last_stop) / MILLIS_IN_SEC;
    if (secondsOnMute >= 1000)
    {
      lcd.setCursor(12, 1);
    }
    else
    {
      lcd.setCursor(13, 1);
    }
    lcd.print(secondsOnMute);
  }
  else
  {
    lcd.setCursor(8, 1);
    lcd.print("SOUND ON");
  }
}

void printDangerLvlLCD(const char* msg)
{
  lcd.setCursor(0, 1);
  lcd.print(msg);
}

void readSensor()
{
  sensorValueA = analogRead(MQ2A);
  muted = millis() - last_stop <= muteMillis;

  printOnLCD();

  if (sensorValueA > dangerLevel)
  {
    printDangerLvlLCD("DANGER");
    
    if (!muted)
    {
      notify("Hazardous concentrations of smoke and/or volatile gasses detected\nAlarm is on");
      digitalWrite(RELAY, LOW);
    }
    else 
    {
      notify("High concentrations of smoke and/or volatile gasses detected\nAlarm is off");
      digitalWrite(RELAY, HIGH);
    }
  }
  else if ((double) sensorValueA > 0.8  * (double) dangerLevel)
  {
    if (!muted)
    {
      notify("Abnormal concentrations of smoke and/or volatile gasses detected");
    }
    printDangerLvlLCD("WRNING");
  }
  else
  {
    printDangerLvlLCD("SAFE");
    digitalWrite(RELAY, HIGH);
  }
}
 
void loop()
{
  if (Blynk.connected()) {
    Blynk.run();
    timer.run();
  } else {
    readSensor();
    printOnLCD();
    Blynk.connect();
  }
} 
