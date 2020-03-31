#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

namespace web
{
    String readString;
    String changestring;
    WiFiServer server(80);

    int GetRandomNum(int from , int to)
    {
        return rand() % (from - to + 1) + from;
    }

    void Setup(String &ssid , String &password , String &out_cur_ip)
    {
        WiFi.softAP(ssid.c_str(), password.c_str());
        IPAddress myIP = WiFi.softAPIP();
        out_cur_ip = myIP.toString().c_str();
        server.begin();
    }

    void loop()
    {
        // Create a client connection
        WiFiClient client = server.available();
        if (client) {
            while (client.connected()) {
            if (client.available()) {
                char c = client.read();

                //read char by char HTTP request
                if (readString.length() < 100) {

                //store characters to string
                readString += c;
                //Serial.print(c);
                }

                //if HTTP request has ended
                if (c == '\n') {

                ///////////////
                //Serial.println(readString); //see what was captured
                changestring = readString;
                changestring.replace("GET /?", "");
                changestring.replace("&submit=send+ssid HTTP/1.1","");
                changestring.replace("GET /favicon.ico HTTP/1.1","");
                changestring.replace("\n","");
                changestring.replace("\n","");
                
                if(changestring.length() > 0)
                {
                    Serial.print("RESULT : ");
                    Serial.println(changestring); 
                }
                //now output HTML data header

                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: text/html");
                client.println();

                client.println("<HTML>");
                
                client.println(changestring);
                client.println("<HEAD>");
                client.println("<TITLE>Arduino GET test page</TITLE>");
                client.println("</HEAD>");
                client.println("<BODY>");

                client.println("<H1>test</H1>");

                client.println("<FORM ACTION='/' method=get >"); //uses IP/port of web page

                client.println("ssid : <INPUT TYPE=TEXT NAME='ssid' VALUE='' SIZE='25' MAXLENGTH='50'><BR>");
                client.println("password : <INPUT TYPE=TEXT NAME='password' VALUE='' SIZE='25' MAXLENGTH='50'><BR>");
                client.println("<INPUT TYPE=SUBMIT NAME='submit' VALUE='send ssid'>");

                client.println("</FORM>");

                client.println("<BR>");

                client.println("</BODY>");
                client.println("</HTML>");

                delay(1);
                //stopping client
                client.stop();

                /////////////////////
                
                //clearing string for next read
                
                readString="";
                changestring="";

                }
            }
            }
        }
    }
}