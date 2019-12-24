#include "ESP8266.h"
#include  <Wire.h>
#include  <LiquidCrystal_I2C.h>

#define SSID        "AFNet"
#define PASSWORD    "AF1919##"

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

SoftwareSerial mySerial(10,11);
ESP8266 wifi(mySerial);

const int sensorAtas=2;
const int sensorBawah=3;
const int sensorReset=4;
const int buzzer=5;

float metal;
int reading;
int metalPin = A0;
int led=13;
int relay=12;

int bacaAtas,bacaBawah,bacaReset;
boolean statusAtas=false,statusBawah=false,statusReset=false;
int totalTabungA=0,totalTabungB=0;
char buffData[200];
boolean statusTombol=false;
int tempTabungA=200,tempTabungB=200;

boolean statusKirim=false;
void setup() 
{
  Serial.begin(9600);
 pinMode(led,OUTPUT);
 pinMode(relay,OUTPUT);
  lcd.begin(16,2);   

  lcd.setCursor(0,0);
  lcd.print(" SYSTEM MONITOR ");
  lcd.setCursor(0,1);
  lcd.print("    TABUNG      ");
  lcd.clear();
      
  pinMode(sensorAtas,INPUT_PULLUP);
  pinMode(sensorBawah,INPUT_PULLUP);
  pinMode(sensorReset,INPUT_PULLUP);
  pinMode(buzzer,OUTPUT);

  digitalWrite(buzzer,HIGH);
  
  lcd.backlight();
  delay(250);
  lcd.noBacklight();
  delay(250);
  lcd.backlight();

  wifi.setOprToStation();

  if (wifi.joinAP(SSID, PASSWORD)) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Join Net OK");
  } else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Join Net Fail");
  }
  delay(2000);
}

void loop() 
{  
  bacaAtas=digitalRead(sensorAtas);
  bacaBawah=digitalRead(sensorBawah);
  bacaReset=digitalRead(sensorReset);

  if(bacaReset){
    if(!statusReset){
      statusTombol=true;
      digitalWrite(buzzer,LOW);
      statusKirim=true;
      delay(500);
      digitalWrite(buzzer,HIGH);
      statusReset=true;
        digitalWrite(relay, HIGH);
    }
  }else{
      statusReset=false;    
  }  

  if(bacaAtas){
    if(!statusAtas){
      totalTabungA++;
      statusAtas=true;
        digitalWrite(relay, HIGH);
    }
  }else{
      statusAtas=false;    
  }

  
  if(bacaBawah){
    if(!statusBawah){
      totalTabungB++;
      totalTabungA--;
      statusBawah=true;
      digitalWrite(relay, HIGH);
    }
    
  }else{
      statusBawah=false;
    
  }  





  lcd.setCursor(0,0);
  lcd.print("Tabung 12 KG=");
  lcd.print(totalTabungA);

  lcd.setCursor(0,1);
  lcd.print("Tabung 50 KG=");
  lcd.print(totalTabungB);
  
if(totalTabungA!=tempTabungA){
  tempTabungA=totalTabungA;
  statusKirim=true;
  lcd.setCursor(13,0);
  lcd.print("   ");
  //lcd.clear();

}
  
if(totalTabungB!=tempTabungB){
  tempTabungB=totalTabungB;
  lcd.setCursor(13,1);
  lcd.print("   ");
  //lcd.clear();
  statusKirim=true;

}







  if (wifi.createTCP("afebryanto.my.id", 80)) {
    Serial.print("create tcp ok\r\n");
  } else {
    Serial.print("create tcp err\r\n");
  }

if(statusKirim){
  if(statusTombol){
  sprintf(buffData,"GET http://afebryanto.my.id/prototype/upload.php?data1=%d&data2=%d&status=0 HTTP/1.1\r\nHost: www.afebryanto.my.id\r\nConnection: close\r\n\r\n",totalTabungA,totalTabungB);
  wifi.send((const uint8_t*)buffData, strlen(buffData));
  totalTabungA=0;
  totalTabungB=0;
  statusTombol=false; 
  }else{
  sprintf(buffData,"GET http://afebryanto.my.id/prototype/upload.php?data1=%d&data2=%d&status=1 HTTP/1.1\r\nHost: www.afebryanto.my.id\r\nConnection: close\r\n\r\n",totalTabungA,totalTabungB);
  wifi.send((const uint8_t*)buffData, strlen(buffData));    
  }
  statusKirim=false;
}

reading = analogRead(metalPin);
metal = (float)reading*100/1024.0;
if(reading<10){
//Serial.println("Metal Detected");
digitalWrite(relay, LOW);
digitalWrite(led, HIGH);
}
else {
 digitalWrite(led, LOW); 
}
delay(1000);
}


