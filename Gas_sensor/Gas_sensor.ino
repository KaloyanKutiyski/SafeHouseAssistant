#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "auth_key";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "wifi_name";
char pass[] = "wifi_pass";

#define MQ2A 34
#define RELAY 26
#define BTN_PIN 2

BlynkTimer timer;

int sensorValueA = 0;
const int defaultDangerLevel = 1500;
int dangerLevel = defaultDangerLevel;
unsigned long last_stop = 0;
 
void setup()
{
 Serial.begin(115200);
 pinMode(MQ2A, INPUT);
 pinMode(RELAY, OUTPUT);
 Blynk.begin(auth, ssid, pass);
 timer.setInterval(500L, readSensor); // read sensors every 500ms
 timer.setInterval(500L, readRemoteInput); // read data from mobile device every 500ms
}

void readRemoteInput()
{
  boolean isPressed = (digitalRead(BTN_PIN) == LOW);
  if (!isPressed) {
    digitalWrite(RELAY, HIGH);
    last_stop = millis();
  } else {
    digitalWrite(RELAY, LOW);
  }
}

void readSensor()
{
  sensorValueA = analogRead(MQ2A);
  Serial.println(sensorValueA);
 
  if (sensorValueA > dangerLevel)
  {
    if(millis() - last_stop < 10000)
    {
      digitalWrite(RELAY, LOW);
      Serial.println("Current Flowing");
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
