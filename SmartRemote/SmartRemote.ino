
                /////////////////////////////////////////////////////////////////////////////
                ///           This Code Segment is running inside the nodeMcu             ///
                ///           of the TV remort.To uplode this code to your board          ///
                ///           Arduino IDE on your PC must install the following           ///
                ///           libraries.                                                  ///
                ///                                                                       ///
                /////////////////////////////////////////////////////////////////////////////


#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRutils.h>
#include <ESP8266WiFi.h>
#include <WifiUDP.h>
#include <String.h>
#include <NTPClient.h>
#include <Time.h>
#include <TimeLib.h>
#include <Timezone.h>

// Define NTP properties
#define NTP_OFFSET   60 * 60      // In seconds
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "ca.pool.ntp.org"  // change this to whatever pool is closest (see ntp.org)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);


const char* ssid = "AndroidAP";   // insert your own ssid 
const char* password = "Sulhi123";   

#define IR_LED 4  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(IR_LED);

const int hexValue[] = {0x800,0x801,0x802,0x803,0x804,0x805,0x806,0x807,0x808,0x809};  //   These are the frequencies of ir waves when we pressed each button . it indicated in hexa value 
int menu,up,down,left,right;                                                           //
int volumeup = 0x810;                                                                  //
int volumedown=0x811;                                                                  //
int muteir=0x80D;                                                                      //
int progUpIr= 0x820;                                                                   //
int progDownIr= 0x821;                                                                 //
int powerIr = 0x800;                                                                   //
 
#include <FirebaseArduino.h>
#define firebaseURI "ice-remote-pro.firebaseio.com"
#define authCode "9VmCrHYSe3SYP01zcz4VOG4rvnhTi4NEpSFBNwgI"


int current_channel;
int volume;
bool muteState;
bool powerState;

uint8_t state = 100;

/////////////////Inrettupt Routine Functions////////////////////////////////////////////////////////////////////
void button1(){
  state = 1;   // when button 1 is pressed we set the state variable to 1. why we do like this is ,here this is a Interrupt routine.so it should be execute small number of instructions. otherwise 
               // watch dog counter is restarting .
}

void button2(){
  state = 2;
  
}

void button3(){
  state = 3;
  
}

/*void button4(){
  state = 4;
  
}*/

void mute(){
   state = 101; 
}

void volup(){
  state = 102;
}

void voldown(){
  state = 103;
  
}

void power(){
  state = 104;
}

void progUp(){
  state = 105;
}

void progDown(){
  state = 106;
}

int time1(){
    time_t t = now();
    int t1 =t;
    return t1;
}
void firebase_ops(int toSend){
  Firebase.setInt("user1/current_channel",toSend);
  String arg1= "user1/history/" + String(time1());
  Firebase.setInt(arg1,toSend);
  }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void pop(){
  String arg1;
  if(state==101){
    irsend.sendSony(muteir,12,2);
    arg1= "user1/mute";
    muteState = !muteState;
    Firebase.setBool(arg1,muteState);
  }
  else if(state==102){
    irsend.sendSony(volumeup,12,2);
    arg1= "user1/vloume";
    volume +=1;
    Firebase.setInt(arg1,volume);
  }
  else if(state==103){
    irsend.sendSony(volumedown,12,2);
    arg1= "user1/vol";
    volume -=1;
    Firebase.setBool(arg1,volume);
  }else if(state<100){
    current_channel= state;
    irsend.sendSony(hexValue[current_channel],12,2);
    firebase_ops(current_channel);
  }
  else if(state==104){
    irsend.sendSony(powerIr,12,2);
    powerState = !powerState;
    arg1 = "user1/power";
    Firebase.setBool(arg1,powerState);
  }
  else if(state==105){
    irsend.sendSony(progUpIr,12,2);
    arg1 = "user1/current_channel";
    current_channel +=1;
    String arg2 ="user1/history/"+String(time1());
    Firebase.setInt(arg1,current_channel);
    Firebase.setInt(arg2,current_channel);
  }
  else if(state==106){
    irsend.sendSony(progDownIr,12,2);
    arg1 = "user1/current_channel";
    current_channel -=current_channel;
    Firebase.setInt(arg1,current_channel);
    String arg2 ="user1/history/"+String(time1());
    Firebase.setInt(arg2,current_channel);
  }
  
   state = 100;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  irsend.begin();
  Serial.begin(115200,SERIAL_8N1, SERIAL_TX_ONLY);
  pinMode(14,INPUT_PULLUP);
  pinMode(5,INPUT_PULLUP);
  pinMode(2,INPUT_PULLUP);
  pinMode(0,INPUT_PULLUP);
  pinMode(12,INPUT_PULLUP);
  pinMode(13,INPUT_PULLUP);
  pinMode(15,INPUT_PULLUP);
  pinMode(16,INPUT_PULLUP);
  attachInterrupt(5,button1,RISING);
  attachInterrupt(0,button2,RISING);
  attachInterrupt(2,button3,RISING);
  attachInterrupt(15,volup,RISING);
  attachInterrupt(16,power,RISING);
  attachInterrupt(12,progUp,RISING);
  attachInterrupt(14,mute,RISING);
  attachInterrupt(13,progDown,RISING);
  WiFi.begin(ssid,password);
  while(WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  
  timeClient.begin();
  
  if (WiFi.status() == WL_CONNECTED) //Check WiFi connection status
  {   
 // update the NTP client and get the UNIX UTC timestamp 
    timeClient.update();
    unsigned long epochTime =  timeClient.getEpochTime();

    // convert received time stamp to time_t object
    time_t  utc;
    utc = epochTime;

    // Then convert the UTC UNIX timestamp to local time
    TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, 270};  //UTC - 5 hours - change this as needed
    TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, 330};   //UTC - 6 hours - change this as needed
    Timezone usEastern(usEDT, usEST);
    setTime( usEastern.toLocal(utc));

    time_t t = now();
    Firebase.begin(firebaseURI, authCode);
    muteState = Firebase.getBool("user1/mute");
    current_channel = Firebase.getInt("user1/current_channel");
    volume = Firebase.getInt("user1/vol");
  } 
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  if(state!=100){
    pop();
  }else{
    int channel = Firebase.getInt("user1/current_channel");
    if(current_channel!=channel){
      irsend.sendSony(channel,12,2);
      String arg1 = "user1/history/"+String(time1());
      Firebase.setInt(arg1,channel);
      current_channel=channel;
    }
    int vol = Firebase.getInt("user1/vol");
    if(volume!=vol){
      if(volume<vol){
        int t_up = vol-volume;
        for(int i=0 ; i<t_up;i++){
        irsend.sendSony(volumeup,12,2);
        volume+=1;
        }
      }
      else{
        int t_down = vol-volume;
        for(int i=0 ; i<t_down;i++){
          irsend.sendSony(volumedown,12,2);
          volume-=1;
      }
      
    }
    bool muteVal = Firebase.getInt("user1/mute");
    if(muteState!=muteVal){
      irsend.sendSony(muteir,12,2);
      muteState = !muteState;
      
    }
  }
  
  }
  Serial.println(current_channel);
  
}










