#include <ESP8266WiFi.h>
WiFiClient client;

int sensorPin = 0;
bool openedTooLong = false;
String MakerIFTTT_Key ;
String MakerIFTTT_Event;
char post_rqst[256];
char *p;
char *content_length_here;
char *json_start;
int compi;
double currTemp;
char currTempS[10];

char ssid[] = "";
char pass[] = "";

char *append_str(char *here, String s) {
  int i=0; 
  while (*here++ = s[i]){
    i++;
    }
  return here-1;
  }
  
char *append_ul(char *here, unsigned long u) { 
  char buf[20]; 
  return append_str(here, ultoa(u, buf, 10));
  }

void setup()
{
  openedTooLong = false;
  Serial.begin(9600);
  WiFi.disconnect();
  delay(3000);
  Serial.println("START");
   WiFi.begin(ssid, pass);
  while ((!(WiFi.status() == WL_CONNECTED))){
    delay(300);
    Serial.println("...");
  }
  Serial.println("CONNECTED");
}

void loop()
{
  sprintf(currTempS,"%.2f", currTemp);
  int reading = analogRead(sensorPin); 
  float voltage = reading * 3.3;
  voltage /= 1024.0;
 
 Serial.print(voltage); Serial.println(" volts");
 
  double temperatureC = (voltage - 0.5) * 100 ;
  char temperatureCS[10];
  sprintf(temperatureCS,"%.2f", temperatureC);
  Serial.print(temperatureCS); Serial.println(" degrees C");
  Serial.println(openedTooLong);
  
  if (client.connect("maker.ifttt.com",80)) {
    if(temperatureC > -3) {
      if(openedTooLong){
        MakerIFTTT_Key ="jh0MQsle0e4uiqJqmPL93BdoprOtykGJJXAVKQOUQ1F";
        MakerIFTTT_Event ="chillEvent";
        p = post_rqst;
        p = append_str(p, "POST /trigger/");
        p = append_str(p, MakerIFTTT_Event);
        p = append_str(p, "/with/key/");
        p = append_str(p, MakerIFTTT_Key);
        p = append_str(p, " HTTP/1.1\r\n");
        p = append_str(p, "Host: maker.ifttt.com\r\n");
        p = append_str(p, "Content-Type: application/json\r\n");
        p = append_str(p, "Content-Length: ");
        content_length_here = p;
        p = append_str(p, "NN\r\n");
        p = append_str(p, "\r\n");
        json_start = p;
        p = append_str(p, "{\"value1\":\"");
        p = append_str(p, temperatureCS); //The degrees
        p = append_str(p, "\"}");
    
        compi= strlen(json_start);
        content_length_here[0] = '0' + (compi/10);
        content_length_here[1] = '0' + (compi%10);
        client.print(post_rqst);
        Serial.println("Sent to IFTTT");
        delay(1500000); //25 min
      } else {
        openedTooLong = true;
        //delay(120000); //2 minutes
      }
    } else {
      openedTooLong = false;
    }
  }
  delay(300000); //5 minutes
}
