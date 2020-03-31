#include <Arduino.h>
#include "SSD1306.h"

namespace imdisplay
{
    SSD1306 display(0x3c, 4, 15);
    void Setup()
    {
        Serial.println("-imfine display initialize-");
        pinMode(16, OUTPUT); // 16
        digitalWrite(16, LOW);
        //delay(50);
        digitalWrite(16, HIGH);
        display.init();
    }

    void clear()
    {
        display.clear();
    }

    /*
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    */
    void setTextAlignment(OLEDDISPLAY_TEXT_ALIGNMENT value)
    {
        display.setTextAlignment(TEXT_ALIGN_LEFT);
    }

    /*
    display.setFont(ArialMT_Plain_10); //폰트 크기 지정
    */
    void setFont(const uint8_t *font)
    {
        display.setFont(font);
    }

    /*
    display.drawString(0, 26, "Hello world");
    */
    void drawString(int16_t x , int16_t y , String str)
    {
        display.drawString(x,y,str);
    }

    void commit()
    {
        display.display();
    }

    void print()
    {
        
    }

    void println(String str)
    {
        /*
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.println(str);
        */
    }

    void ex(int &d)
    {
        d += 100;
    }
    
    int screenW = 128;
    int screenH = 64;
    void clear(SSD1306 &d)
    {
        d.clear();
    }

    void drawWifiStatus(SSD1306 &d,const String &line1 ,const String &line2 )
    {
        int box_w = screenW;
        int box_h = 26;

        
        d.setColor(OLEDDISPLAY_COLOR::BLACK);
        d.fillRect(0,0,box_w , box_h);
        d.setColor(OLEDDISPLAY_COLOR::WHITE);
        //d.clear();

        d.drawRect(0 , 0 , box_w , box_h);

        
       
        d.setFont(ArialMT_Plain_10);
        //d.drawString(box_x + 1 , box_y , line1);

        d.setLogBuffer(2,32);
        d.println(line1);
        d.println(line2);
        d.drawLogBuffer(1,0);
        
        d.drawString(0,screenH - 10 , "www.im-fine.co.kr");
        d.display();
    }

    void drawSignalStatus(SSD1306& d, byte go , byte back , byte wheel)
    {
        //d.clear();
        
        
        d.setColor(OLEDDISPLAY_COLOR::BLACK);
        d.fillRect(0,30,screenW , 13);
        d.setColor(OLEDDISPLAY_COLOR::WHITE);
        
        //d.drawRect(0,30,screenW , 13);

        char* str = nullptr;
        asprintf(&str , "F[ %.3d ] B[ %.3d ] W[ %.3d ]" , go , back , wheel);
        

        //d.setFont(ArialMT_Plain_10);
        //d.drawString(0,30 , "cccccccc");
        d.drawString(0,30 , str);
        d.display();

        free(str);
    }
}



/*
void textOut(void)
{
  Serial.println("t1");
  display.setLogBuffer(2,10); // text arrange width height!!

  const char* Eunsoo[] = 
  {
      "RC_Car",
      "Starting",
      "hello stranger"
  };
  for(uint8_t i = 0; i < 3; i++)
  {
    display.clear(); //init the screen
    display.setFont(ArialMT_Plain_10); // 글자 크기 정해줌 단위는 fix
    display.println(Eunsoo[i]); //Eunsoo text output
    display.drawLogBuffer(10,10); // arrange line
    display.display(); //appear
    delay(1500);
  }
}

void OtherText(void)
{
  Serial.println("t2");
  display.clear(); // 시작전 빈화면
  display.setTextAlignment(TEXT_ALIGN_LEFT); //문장 정열
  display.setFont(ArialMT_Plain_10); //폰트 크기 지정
  display.drawString(0, 0, "Hello world");
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "Hello world");
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 26, "Hello world");
  display.drawString(10, 128, String(0)); 
  display.display();
}

*/