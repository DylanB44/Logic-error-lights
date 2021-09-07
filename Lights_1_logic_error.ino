

#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> 
#endif

#define PIN        13
#define NUM_LEDS 300


Adafruit_NeoPixel strip(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500

void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // neopixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // fastlED
   FastLED.show();
 #endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  
  showStrip();
}


void rainbowCycle(int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { 
    for(i=0; i< NUM_LEDS; i++) {
      c=Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      setPixel(i, *c, *(c+1), *(c+2));
    }
    strip.setBrightness(255);
    showStrip();
    delay(SpeedDelay);
  }
}

byte * Wheel(byte WheelPos) {
  static byte c[3];
 
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}


const char* ssid     = "BT-TGAHZR";
const char* password = "beqxyVRt6cGng7";

WiFiServer server(80);

void setup()
{
    Serial.begin(115200);
    pinMode(5, OUTPUT);     

    delay(10);

    

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

    strip.begin();



}

int value = 0;

void loop(){
  
 WiFiClient client = server.available();  

  if (client) {                            
    Serial.println("New Client.");           
    String currentLine = "";                
    while (client.connected()) {            
      if (client.available()) {            
        char c = client.read();            
        Serial.write(c);                    
        if (c == '\n') {                   

   
          if (currentLine.length() == 0) {
       
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

          
            client.print("Click <a href=\"/N\">here</a> to Neophase.<br>");
            client.print("Click <a href=\"/R\">here</a> to Running Lights.<br>");
            client.print("Click <a href=\"/RC\">here</a> to Rainbow Cycle.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn off.<br>");

          
            client.println();
            
            break;
          } else {   
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }

      
     
        }
        while (currentLine.endsWith("GET /RC")) {
          rainbowCycle(5);
        }
        if (currentLine.endsWith("GET /L")) {
          setAll(0,0,0);
        }
      }
    }
   
    client.stop();
    Serial.println("Client Disconnected.");
  }
