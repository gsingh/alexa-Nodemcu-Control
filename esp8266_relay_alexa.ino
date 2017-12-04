

#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>

//#include <WiFiClientSecure.h>
//#include <WiFiServer.h>
//#include <WiFiUdp.h>

#define PubNub_BASE_CLIENT WiFiClient
#define PUBNUB_DEFINE_STRSPN_AND_STRNCASECMP
#include <PubNub.h>


#include <Arduino.h>
//#include <Adafruit_NeoPixel.h>
//#include <ESP8266WiFi.h>
//#include "fauxmoESP.h"
#include <ArduinoJson.h>


#define WIFI_SSID "JioFi2_4F5DCF"
#define WIFI_PASS "gsingh12@"

#define SERIAL_BAUDRATE                 115200



//fauxmoESP fauxmo;

#define RELAY_PIN 5
// #define NEOPIX_PIN 2
// Adafruit_NeoPixel strip = Adafruit_NeoPixel(40, NEOPIX_PIN, NEO_GRB + NEO_KHZ800);
//volatile boolean neopixel_state = false; // off by default!

const static char pubkey[] = "pub-c-7396c4a3-a3a3-4f79-9ff6-598b71e98d40";         // your publish key 

const static char subkey[] = "sub-c-8df09d4c-d4d4-11e7-8fb0-b220df4bdfb9";         // your subscribe key

const static char channel[] = "hello_world"; // channel to use

uint32_t Wheel(byte WheelPos); // function prototype

// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------


void wifiSetup() {
  // Set WIFI module to STA mode
  WiFi.mode(WIFI_STA);

  // Connect
  Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(100);
  }
  Serial.println();

  // Connected!
  Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

PubNub.begin(pubkey, subkey);
Serial.println("PubNub set up");
  
}


//void callback(uint8_t device_id, const char * device_name, bool state) {
//  Serial.printf("[MAIN] %s state: %s\n", device_name, state ? "ON" : "OFF");
//  
////  if ( (strcmp(device_name, "pixels") == 0) ) {
////    // this just sets a variable that the main loop() does something about
////    if (state) {
////      neopixel_state = true;
////    } else {
////      neopixel_state = false;
////    }
////  }
//
//  if ( (strcmp(device_name, "relay") == 0) ) {
//    // adjust the relay immediately!
//    if (state) {
//      digitalWrite(RELAY_PIN, HIGH);
//    } else {
//      digitalWrite(RELAY_PIN, LOW);
//    }
//  }
//}

void setup() {
 // strip.begin();
 // strip.setBrightness(20);
 // strip.show(); // Initialize all pixels to 'off'
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  
    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();
    Serial.println("FauxMo demo sketch");
    Serial.println("After connection, ask Alexa/Echo to 'turn pixels on' or 'off' or 'turn relay on' or 'off'");

  // Wifi
  wifiSetup();

  getRequestPubNub();
}

void getRequestPubNub(){
  WiFiClient client;

  const char destServer[] = "pubsub.pubnub.com";  

  int retVal = client.connect(destServer, 80);

  if (retVal <= 0)

  {

    Serial.println(F("Failed to connect to server."));

    return;

  }



  client.println("GET /subscribe/demo/hello_world/0/0 HTTP/1.1");



  client.println("Host: pubsub.pubnub.com\r\n");



  // available() will return the number of characters

  // currently in the receive buffer.

  while (client.available())

    Serial.write(client.read()); // read() gets the FIFO char
}

void loop() {

 WiFiClient *client;



/* Subscrbe */

Serial.println("waiting for a message (subscribe)");

  PubSubClient *pclient = PubNub.subscribe(channel);

  //getDistance();

  if (!pclient) {

    Serial.println("nothing");

    delay(1000);

    return;

  }

StaticJsonBuffer<128> jsonBuffer;

  char buffer[128]; 

  size_t buflen = 0;

  bool record = false;

  char c;

  while (pclient->wait_for_data()) {

    c = pclient->read();

    if (c=='[') { //ignore of other junk that pubnub produces

      record = true;

    }

    if (record == true) { 

      buffer[buflen++] = c;

    }

    if (c==']') {

      record = false;

    }    

  }

  buffer[buflen] = 0;

  pclient->stop();

  Serial.println(buffer); 



  JsonArray& root = jsonBuffer.parseArray(buffer);

  int commandText = root[0]["action"]; //ints to save space

  int GPIO = root[0]["GPIO"];

  

  if (!root.success()) {

    Serial.println("parseObject() failed");

    return;

  } else {

    

    Serial.println(commandText);

    Serial.println(GPIO);

     

    if (commandText == 1) {

      Serial.println("turning on");

      Serial.print(GPIO);

      gpioOn(GPIO);      

    }else if (commandText == 2) {

      Serial.println("turning off ");

      Serial.print(GPIO);

      gpioOff(GPIO);  

    }else if (commandText == 3) {

      //Serial.println("turning all on ");  

      //loop through all of your active pins and turn them on

    } else if (commandText == 4) {

      //Serial.println("turning all off ");

      //loop thorugh all of your active pins and turn them off  

    } 

  }
}






void gpioOn(int gpio_pin) {

  digitalWrite(gpio_pin, HIGH);

}



void gpioOff(int gpio_pin) {

  digitalWrite(gpio_pin, LOW);

}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
//uint32_t Wheel(byte WheelPos) {
//  if(WheelPos < 85) {
//   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
//  } else if(WheelPos < 170) {
//   WheelPos -= 85;
//   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
//  } else {
//   WheelPos -= 170;
//   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
//  }
//}
