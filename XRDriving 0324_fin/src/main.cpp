#include <Arduino.h>
#include <TimeLib.h>

#include <WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <ESP32Servo.h>

#include "button.h"
#include "SSD1306.h" // 모니터
#include "OLEDDisplayUi.h"
#include "IMDisplay.h"
#include "IMConfigServer.h"

const int EEPROM_ADDR_WIFI_SSID = 0;
const int EEPROM_ADDR_WIFI_PW = 128;



const char *ssid = "im 2G";
const char *password = "imfine1027";
const int safty_value = 20;
const int port = 12200;

Servo wheelControl;
Servo DcControl;
const int button = 15;

//Servo MServo;
//WiFiServer server(port);
WiFiUDP udp;

SSD1306Wire display(0x3c, 4, 15);
OLEDDisplayUi ui(&display);
int screenW = 128;
int screenH = 64;

bool udp_setup = false;

const int pin1 = 2;
const int pin2 = 18;
const int pin3 = 5;

int speed = 10;

void DisplaySetup()
{
  pinMode(16, OUTPUT); // 16
  digitalWrite(16, LOW);
  //delay(50);
  digitalWrite(16, HIGH);
  display.init();
}



//wifi event handler
void WiFiEvent(WiFiEvent_t event)
{
  switch (event)
  {
  case SYSTEM_EVENT_STA_GOT_IP:
    //When connected set 2
    Serial.print("WiFi connected! IP address: ");
    Serial.println(WiFi.localIP());
    //initializes the UDP state
    //This initializes the transfer buffer

    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    Serial.println("WiFi lost connection");
    //connected = false;
    imdisplay::drawWifiStatus(display, "WIFI DISCONNECTED", "");
    break;
  default:
    break;
  }
}

void WifiSetup()
{
  //AP MODE
  if (ssid == nullptr)
  {
    /*
    char* ap_ssid = nullptr;
    String ap_pw = "123456789";
    String ap_ip = "";

    asprintf(&ap_ssid , "im_%s" , web::GetRandomNum(0,1000));

    return;
    String str_ap_ssid(ap_ssid);
    web::Setup(str_ap_ssid , ap_pw , ap_ip);
    
    char* header;
    //sprintf(header , "%s" , ap_ip);
    asprintf(&header , "AP MODE: %s" , ap_ip.c_str());
    Serial.println(header);

    char* text = nullptr;
    asprintf(&text , "%s / %s" , ap_ssid , ap_pw.c_str());
    
    imdisplay::drawWifiStatus(display , header , text);

    free(header);
    free(text);

    Serial.print("APIP: ");
    Serial.println(ap_ip);
    return;
    */
  }

  char *header = nullptr;
  asprintf(&header, ">>%s", ssid);

  imdisplay::drawWifiStatus(display, header, "");

  WiFi.disconnect(true);
  WiFi.onEvent(WiFiEvent);
  WiFi.begin(ssid, password);

  int try_count = 0;
  char *str_try_count = nullptr;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);

    imdisplay::clear(display);
    asprintf(&str_try_count, "try:%d", try_count);
    imdisplay::drawWifiStatus(display, header, str_try_count);

    try_count++;
  }
  udp.begin(WiFi.localIP(), port);

  asprintf(&header, "[ %s ]", ssid);
  char *ip_port = nullptr;
  asprintf(&ip_port, "%s : %d", WiFi.localIP().toString().c_str(), port);

  imdisplay::drawWifiStatus(display, header, ip_port);
  Serial.println(ip_port);
  udp_setup = true;
  //server.begin();
}

void mainMotor()
{

  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);


  ledcSetup(0, 5000, 8); //일반모터
  ledcSetup(1, 5000, 8); // 일반모터
  //ledcSetup(2, 50, 8);   // 서보모터

  ledcAttachPin(pin1, 0);
  ledcAttachPin(pin2, 1);
  ledcAttachPin(pin3, 2); //서보 채널

  
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Motor start");

  /*
  bool r = EEPROM.begin(512);
  Serial.println(r);
  
  //EEPROM.writeString(EEPROM_ADDR_WIFI_SSID,"addrtest1");
  //EEPROM.writeString(EEPROM_ADDR_WIFI_PW,"__str__");
  //EEPROM.commit();  

  
  String readstr = EEPROM.readString(EEPROM_ADDR_WIFI_SSID);
  Serial.println(readstr);
  //ssid = readstr.c_str();

  readstr = EEPROM.readString(EEPROM_ADDR_WIFI_PW);
  Serial.println(readstr);
  password = readstr.c_str();
  */
  //mainMotor();
  //  mainMotor();
 // DisplaySetup();
   mainMotor();
   DisplaySetup();
  WifiSetup();
}

void ClientProcessType1(WiFiClient client)
{
  Serial.println("Start Read");
  byte timestamp_data[8];
  client.readBytes(timestamp_data, 8);

  byte header_data[2];
  client.readBytes(header_data, 2);

  byte value = client.read();
  client.write(1);

  long *timestamp = (long *)timestamp_data;
  short *header = (short *)header_data;

  Serial.print("result: ");
  Serial.print(*timestamp);
  Serial.print(",");
  Serial.print(*header);
  Serial.print(",");
  Serial.print(value);
  //Serial.println(*i);
  Serial.println("-------");
}

const int wheel_ragne_min = 60;//11;  //25;
const int wheel_range_max = 120;//26; //145;

const int forward_range_min = 0;
const int forward_range_max = 150;

const int backward_range_min = 0;
const int backward_range_max = 150;

const int stop = 70;
const int go = 69;
const int backLimit = 69;

byte old_forward = 0;
byte old_backward = 69;

bool isReady = false;
bool isStart = false;
bool accelating = false;
void loop()
{

  /* if(Serial.available())
  {
    char command = Serial.read();

    if (command == 's')
    {
          DcControl.write(remap_backward);
         delay(20);

      Serial.println("stop");
      DcControl.write(70);
      delay(200000);
    }
  }*/

  if (udp_setup == true)
  {
    int packetSize = udp.parsePacket();
    if (packetSize)
    {
      byte acc = udp.read();

      byte go = 0;
      byte back = 0;
      if (acc != 128)
      {
        //back 0~128 -> 0 ~ 255
        if (acc < 128)
        {
          back = 255 - (acc * 2);
        }
        //go 128 ~ 255 -> 0 ~ 255
        else
        {
          go = (acc - 128) * 2;
        }
      }

      byte wheel = udp.read();
      //client.write(1);
      byte remap_wheel = map(wheel, 60, 120, wheel_ragne_min, wheel_range_max);
      byte remap_forward = map(go, 0, 255, forward_range_min, forward_range_max);
      byte remap_backward = map(back, 0, 255, backward_range_min, backward_range_max);


       
      // if(remap_forward > 0)
      // {
      //     isStart = true;
          
      //     digitalWrite(pin1, LOW);
      //     Serial.printf("go %d", remap_forward);
      //     ledcWrite(0, remap_forward);
      //    ledcWrite(2, remap_wheel);

      // } else if (remap_backward  > 0)
      // {
      //   if(isStart == true)
      //   {
      //     Serial.println("stop");
      //     ledcWrite(0, 0);
      //     ledcWrite(1, 0);
      //    ledcWrite(2, remap_wheel);
      //    // isStart = false;
      //   //  isReady = true;
      //   //  delay(2000);
      //   }

      //   if(isReady == true)
      //   {
      //    //isStart = false;
      //    Serial.println("back");
      //    digitalWrite(pin2, LOW);
      //    ledcWrite(1, remap_backward);
      //    isReady = false;
      //   }
        

      // } 
      // else 
      //if ( remap_wheel > 10)
      {
      //  Serial.printf("wheel %d", remap_wheel);
        ledcWrite(2, remap_wheel); 
        //MServo.write(remap_wheel);
      }

      imdisplay::drawSignalStatus(display, remap_forward, remap_backward, remap_wheel);


/*      if(Serial.available())
      {
        char text = Serial.read();

        if(text == 's')
        {
          Serial.println("Hi");
          digitalWrite(pin1, LOW);
          ledcWrite(0, 20);
        }

        if(text == 'd')
        {
          Serial.println("back");
          digitalWrite(pin2, LOW);
          ledcWrite(1, 20);
        }

        if(text == 'f')
        {
          Serial.println("stop");
          ledcWrite(0, 0);
          ledcWrite(1, 0);
          ledcWrite(2, 18);
        }

        if(text == 'e')
        {
          Serial.println("Servo");
          speed += 1;
          ledcWrite(2, speed);
          Serial.println(speed);

          if(speed > 40)
          {
            speed = 10;
          }
        }
      }*/

      

     /* if (Serial.available())
      {
        char command = Serial.read();

        if (command == 'a')
        {
          Serial.println("go\n");
         // DcControl.write(80); //motor.write(81);
          delay(20);
        }

        if (command == 's')
        {
          //DcControl.write(remap_backward);
          //delay(20);

          Serial.println("stop\n");
         // DcControl.write(60);
          delay(20);
          //delay(200000);
        }
      }


      if (remap_forward > 71)
      {
        accelating = true;
       // DcControl.write(remap_forward);
        Serial.printf("ACC : %d \n" , remap_forward);
      }
      else if (remap_backward < 69)
      {
        
        if(accelating == true)
        {
          Serial.printf("BACK: %d \n", remap_backward);
          //DcControl.write(remap_backward);
          //delay(20);
          DcControl.write(60);
          Serial.println("Stop");
          delay(20);
        }
        
        accelating = false;
       // DcControl.write(60);
      }

       //if(remap_forward > 71)
        {
          isGo = true;
        }
      
      Serial.printf("%d %d %d\n", remap_forward, remap_backward, remap_wheel);
      
      
      if(remap_backward < backLimit )
      {
          DcControl.write(70);
          Serial.printf("back");
          delay(40);
         DcControl.write(70);
         delay(70000);
        //  delay(40);
      }

      int buttonClick;
      buttonClick = digitalRead(button);

      if(buttonClick == LOW)
      {
         DcControl.write(70);
         delay(20);
      }

      if(stop == 70 && remap_backward == 70 && remap_forward == 71)
      {
        DcControl.write(70);
        Serial.printf("Stop");
        delay(40);
      }

       if( old_backward == 70 && remap_backward <69)
      {
       
        Serial.printf("------ %d %d\n", remap_backward, old_backward );

       
        DcControl.write(70);
        delay(40);
       
        

      // old_backward = remap_backward;
      }
       if(remap_forward > 71)
      {
        old_backward = 70;
           
      // DcControl.write(remap_forward);
      }

      // Serial.printf("%d , %d , %d , %d \n" , old_backward , remap_backward , remap_forward, remap_wheel);

      //old_backward = remap_backward;

      if(remap_backward < 69)
      {
         // DcControl.write(70);
         // Serial.println("back");

        DcControl.write(remap_backward);
        Serial.printf("back");

      }

        if(remap_forward >= forwardLimit)
      {
        if(remap_forward == forwardLimit)
        {
          DcControl.write(70);
          Serial.println("go");
        }
        DcControl.write(remap_forward);
      }


      if(remap_backward <= backwardLimit)
      {
        if(remap_backward == backwardLimit)
        {
           DcControl.write(70);
           Serial.println("back");
        }
      //  Serial.printf("BACK!!!!!!! %d" , remap_backward);
        // DcControl.write(remap_backward);
       //  delay(40);
        // DcControl.write(70);
       //  Serial.printf("Stop");
       //  delay(40);
       DcControl.write(remap_backward);

      }

      //Serial.printf("%d \n", remap_backward);

      // bool is_new_value = old_forward != remap_forward;
       if(old_forward != remap_forward)
      {
        Serial.printf("new value : %d \n" , remap_forward);
        
        //
        old_forward = remap_forward;
      }

      
        은수씨 코드 여기다가 테스트 하세요
      

       if(old_backward == 69 && remap_backward < 69)
      {
        //이순간. 이마음. 
        Serial.printf("------  %d\n", remap_backward );

        
        DcControl.write(remap_backward);
        delay(40);
        DcControl.write(70);
        delay(40);
       
        

       old_backward = remap_backward;
      }
      if(remap_backward == backwardLimit)
      {
      //  DcControl.write(70);
      //  Serial.printf("Stop");
        
      }
      if(remap_backward < backwardLimit)
      {
      //  Serial.printf("BACK!!!!!!! %d" , remap_backward);
        // DcControl.write(remap_backward);
       //  delay(40);
        // DcControl.write(70);
       //  Serial.printf("Stop");
       //  delay(40);
       DcControl.write(remap_backward);

      }

      if(remap_forward > 71)
      {
        old_backward = 69;
           
       DcControl.write(remap_forward);
      }

      // Serial.printf("%d , %d , %d , %d \n" , old_backward , remap_backward , remap_forward, remap_wheel);

      //old_backward = remap_backward;

      // DcControl.write(remap_forward);

      //  Serial.printf("%d\n", remap_backward);

      // Serial.printf("%d\n", remap_forward);

      //  Serial.printf("%d\n", remap_backward);

      //   DcControl.write(remap_backward);

      //  Serial.printf(" %d : %d \n" , old_forward , remap_forward);

      //Serial.printf("%d) go[%d] back[%d] wheel[%d]\n" , is_new_value ,remap_forward , remap_backward , remap_wheel);
      // Serial.printf("go[%d] back[%d] wheel[%d]\n" , go , back , wheel); */
     // imdisplay::drawSignalStatus(display, remap_forward, remap_backward, remap_wheel);
    }
  }
  else
  {
    web::loop();
  }
}
