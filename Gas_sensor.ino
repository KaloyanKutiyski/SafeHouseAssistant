
#define MQ2A 34
#define RELAY 26

int sensorValueA = 0;
const int defaultDangerLevel = 1500;
int dangerLevel = defaultDangerLevel;
 
void setup()
{
 Serial.begin(115200);
 pinMode(MQ2A, INPUT);
 pinMode(RELAY, OUTPUT);
}

 
void loop()
{
  sensorValueA = analogRead(MQ2A);
  Serial.println(sensorValueA);
  delay(1000);
 
  if (sensorValueA > dangerLevel)
  {
    digitalWrite(RELAY, LOW);
    Serial.println("Current Flowing");
  }
 
  else
  {
    digitalWrite(RELAY, HIGH);
    Serial.println("Current not Flowing");
  }
} 
