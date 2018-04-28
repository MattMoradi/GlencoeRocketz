/*
 * Web Server
 *
 * (Based on Ethernet's WebServer Example)
 *
 * A simple web server that shows the value of the analog input pins.
 */

#include "WiFly.h"
#include "Credentials.h"
#include <string.h>
#include <Streaming.h>
#include "MemoryFree.h"
#include <avr/pgmspace.h> // for progmem
#define P(name) static const prog_uchar name[] PROGMEM // declare a static string

const int inPinPlus = 1; // analog pin
const int inPinMinus = 2;

Server server(80);

P(page1) = 
"<html>\n"
"<head>\n"
"<script\n"
"       src='https://ajax.googleapis.com/ajax/libs/jquery/1.7.2/jquery.min.js'></script>\n"
"<script\n"
"       src='https://raw.github.com/betanik/jquery-textfill/master/jquery.textfill.min.js'></script>\n"
"</head>\n"
"<body style='margin:0px'>\n"
"       <div class='jtextfill'\n"
"               style='text-align:center; height: 100%; width: 100%; white-space: nowrap'>\n"
"               <span id='temperature'>\n";

P(page2) =
"               </span>\n"
"       </div>\n"
"       <script>\n"
"               $(document).ready(function() {\n"
"                       $('.jtextfill').textfill({\n"
"                               maxFontPixels : 9000\n"
"                       });\n"
"                       //http://techoctave.com/c7/posts/60-simple-long-polling-example-with-javascript-and-jquery\n"
"                       (function poll() {\n"
"                               setTimeout(function() {\n"
"                                       $.ajax({\n"
"                                               url : 'http://192.168.1.81/temperature',\n"
"                                               timeout: 20000,\n"
"                                               cache: false,\n"
"                                               datatype: 'text',                                               \n"
"                                               success : function(data) {\n"
"                                                       $('#temperature').html($.trim(data));\n"
"                                                       $('.jtextfill').textfill({\n"
"                                                               maxFontPixels : 9000\n"
"                                                       });\n"
"                                                       //Setup the next poll recursively\n"
"                                                       poll();\n"
"                                               },\n"
"                               error: function (xhr, textStatus, thrownError){\n"
"                                                       poll();\n"
"                               }                                               \n"
"                                       });\n"
"                               }, 10000);\n"
"                       })();\n"
"               });\n"
"       </script>\n"
"</body>\n"
"</html>\n";


//based on Arduino Cookbook 2nd ed
float measureTemperatureFull() {
  int valuePlus = analogRead(inPinPlus);
  int valueMinus = analogRead(inPinMinus);
  int value = valuePlus - valueMinus;

  Serial << "Plus: " << valuePlus << " Minus: " << valueMinus << " = " << value << "\n";

  Serial.print(value); 
  Serial.print(" > ");
  float millivolts = (value / 1024.0) * 5000;
  float celsius = millivolts / 10; // sensor output is 10mV per degree Celsius 
  Serial.print(celsius);
  Serial.print(" degrees Celsius, ");
  Serial.print( (celsius * 9)/ 5 + 32 ); // converts to fahrenheit 
  Serial.println(" degrees Fahrenheit");
  return celsius;
}

int measureTemperature() {
  return round(measureTemperatureFull());
}




void sendHtmlHeader(Client client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(); 

}

void send404(Client client) {
  client.println("HTTP/1.1 404 Not Found");
  client.println();
}

void sendTemperature(Client client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println(); 
  client.println(measureTemperature());  
}

void sendTemperatureFull(Client client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println(); 
  client.println(measureTemperatureFull());  
}

void sendMemFree(Client client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println(); 
  client.println(freeMemory());  
}


void unknownPage(char *page) {
}


void setup() {
  WiFly.begin();

  if (!WiFly.join(ssid/*, passphrase*/)) {
    while (1) {
      // Hang on failure.
    }
  }

  Serial.begin(9600);
  Serial.print("IP: ");
  Serial.println(WiFly.ip());

  server.begin();
  Serial << F("After Setup mem:") << freeMemory() << endl ;

}

void loop() {
  Client client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean current_line_is_blank = true;
    boolean in_command = false;
    boolean stop_buffering = false;
    String buffer = "";  
    String command = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if(!stop_buffering) {
          buffer += c;    
        }
        // if we've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so we can send a reply
        if (c == '\n' && current_line_is_blank) {
          Serial << "\nReceived " << buffer << "\n";
          break;
        }
        if (c == '\n') {
          // we're starting a new line
          current_line_is_blank = true;
        } 
        else if (c != '\r') {
          // we've gotten a character on the current line
          current_line_is_blank = false;
          if(in_command && c == ' ') {
            in_command = false;
            stop_buffering = true;
          } 
          else if (in_command) {
            command += c;
          }
        }

      }
      if(buffer.equals("GET ")) {
        in_command = true;
      }
    }
    Serial << "\nCommand: " << command << "\n";

    if(command.indexOf("temperatureFull") != -1) {
      sendTemperatureFull(client);
    } 
    else if (command.indexOf("temperature") != -1) {
      sendTemperature(client);
    } 
    else if (command.indexOf("memFree") != -1) {
      sendMemFree(client);
    } 
    else if (command.endsWith("monitor.html")) {
      sendHtmlHeader(client);
      printP(page1, client);
      client.print(measureTemperature());  
      //client.print("&deg;C");
      printP(page2,client);
    }
    else {
      send404(client);
    }


    Serial << F("\nAfter request:") << freeMemory() << endl ;
    // give the web browser time to receive the data
    delay(100);
    client.stop();
  }



}



void printP(const prog_uchar *str, Client client)
{
  // copy data out of program memory into local storage, write out in // chunks of 32 bytes to avoid extra short TCP/IP packets
  // from webduino library Copyright 2009 Ben Combee, Ran Talbott 
  uint8_t buffer[32];
  size_t bufferEnd = 0;
  while (buffer[bufferEnd++] = pgm_read_byte(str++))
  {
    if (bufferEnd == 32)
    {
      client.write(buffer, 32);
      bufferEnd = 0; 
    }
  }
  // write out everything left but trailing NUL if (bufferEnd > 1)
  client.write(buffer, bufferEnd - 1); 
}
 
[Get Code]

Credentials.h


#ifndef __CREDENTIALS_H__
#define __CREDENTIALS_H__

// Wifi parameters
char passphrase[] = "foo";
char ssid[] = "tempserver";
char hostname[] = "alpaca";

#endif
