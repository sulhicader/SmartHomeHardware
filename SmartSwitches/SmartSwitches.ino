#include <ESP8266WiFi.h>   // importing required libraries
#include <FirebaseArduino.h>

#define ssid  "AndroidAP"  // wifi user name
#define password "Sulhi123"   // wifi password

#define firebaseURI "ice-remote-pro.firebaseio.com"   // firebase details
#define authCode "9VmCrHYSe3SYP01zcz4VOG4rvnhTi4NEpSFBNwgI"

#define light 14  //define pin numbers to variables 14:D5 , 12:D6 , 13:D7
#define fan 12
#define motor 13

bool lightstate = false;
bool fanstate = false;
bool motorstate = false;

void setup() {
  Serial.begin(115200);
  pinMode(light,OUTPUT);
  pinMode(fan,OUTPUT);
  pinMode(motor,OUTPUT);
  WiFi.begin(ssid,password);    // try to connect the nodemacu to wifi
  while(WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.print("\n");
  Serial.print("connected\n");
  Firebase.begin(firebaseURI, authCode);  //begin the connection to database
  delay(100);

}

void loop() {
  
  if(Firebase.getBool("user1/Switch/switch1")!=lightstate){
    digitalWrite(light,!digitalRead(light));
    lightstate=!lightstate;
    String arg1 = "user1/Switch/lastOp";    // there is a field in database as name lastop which use to conform the code segment is executed . in app , after doing some on/off it check the
    //lastop in database .actually the operation is happend in hardware lastop is set as true. then only app shows the result. this is the mechanisam we used to get feedback of the work.
    Firebase.setBool(arg1,true);
  }
  if(Firebase.getBool("user1/Switch/switch2")!=fanstate){
    digitalWrite(fan,!digitalRead(fan));
    fanstate=!fanstate;
    String arg1 = "user1/Switch/lastOp";    // there is a field in database as name lastop which use to conform the code segment is executed . in app , after doing some on/off it check the
    //lastop in database .actually the operation is happend in hardware lastop is set as true. then only app shows the result. this is the mechanisam we used to get feedback of the work.
    Firebase.setBool(arg1,true);
  }
  if(Firebase.getBool("user1/Switch/switch3")!=motorstate){
    digitalWrite(motor,!digitalRead(motor));
    motorstate=!motorstate;
    String arg1 = "user1/Switch/lastOp";    // there is a field in database as name lastop which use to conform the code segment is executed . in app , after doing some on/off it check the
    //lastop in database .actually the operation is happend in hardware lastop is set as true. then only app shows the result. this is the mechanisam we used to get feedback of the work.
    Firebase.setBool(arg1,true);
  }
  
  
  

}
