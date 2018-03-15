#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

BlynkTimer timer;
BlynkTimer timerUpdate;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "e3094aa9d9414b9c9745fb6513e9b876";

float dataBuffer[30] = {0};
char i = 0;
float total = 0;
float currentValue = 0;
long times = 0;
float degValue = 0;

void degreeValue(void){
  float degree = (0.22 * currentValue) / 3600; //calculate degree per second
  degValue = degValue + degree;
}

void getCurrentValue(void){
  while(i < 30){
    total = total - dataBuffer[i];
    dataBuffer[i] = (analogRead(0)-510)*3.3/1024/0.04-0.04;
    total = total + dataBuffer[i];
    i++;
  }
  i = 0;
  currentValue = abs(total / 30);
  total = 0;
  degreeValue();
}

BLYNK_CONNECTED(){
  Blynk.syncVirtual(V2);
}

BLYNK_WRITE(V2){
  int buttonState = param.asInt();
  if(buttonState){
    degValue = 0;
  }
}

void updateValue(void){
  Blynk.virtualWrite(V0,currentValue);
  Blynk.virtualWrite(V1,degValue);

  Serial.println(times);
  Serial.println(currentValue);
  Serial.println(degValue);
  Serial.println();
}

void setup()
{
  // Debug console
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  int cnt = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (cnt++ >= 10) {
      WiFi.beginSmartConfig();
      while (1) {
        delay(1000);
        if (WiFi.smartConfigDone()) {
          Serial.println();
          Serial.println("SmartConfig: Success");
          break;
        }
        Serial.print("|");
      }
    }
  }

  WiFi.printDiag(Serial);

  Blynk.config(auth);
  timer.setInterval(1000L, getCurrentValue);
  timerUpdate.setInterval(5000L, updateValue);
}

void loop()
{
  Blynk.run();
  timer.run();
  timerUpdate.run();
}

