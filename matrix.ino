#include <Arduino.h>
#include <ESP8266WiFi.h>              //WiFi
#include <ESP8266HTTPClient.h>        //HTTP Client
#include <WiFiClientSecureBearSSL.h>  //Secure Connection (SSL)
#include <SPI.h>                      //SPI for Matrix
#include <Adafruit_GFX.h>             //charset for Matrix
#include <Max72xxPanel.h>             //Driver MAX7219 for Matrix
#include <ArduinoJson.h>              //JSON Library

// WiFi Connection Parameter

const char* ssid = "Your awsome SSID";
const char* password = "Your awsome WiFi Password";

//Matrix Parameter

const int pinCS = D6;                   //Pin D6 for Communication
int numberOfHorizontalDisplays = 4;     //horizontal Modules
int numberOfVerticalDisplays = 1;       //vertical Modules

int wait = 60;                  //scroll speed (ms)
int brightness = 0;             //Default brightness (0-15)
int spacer = 1;                 //Space between Text
int width = 5 + spacer;         //Textsize

String matrixtext = ssid; //default Text


Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays); // Setting up


//Setup

void setup() {
  Serial.begin(115200);
  //Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  //WiFi Connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(900);
  }
    matrix.setIntensity(brightness); // Default brightness
  //Matrix rotation
  matrix.setRotation(0, 1);
  matrix.setRotation(1, 1);
  matrix.setRotation(2, 1);
  matrix.setRotation(3, 1); 
}

void show() {
  for ( int i = 0 ; i < width * matrixtext.length() + matrix.width() - 1 - spacer; i++ ) {

    matrix.fillScreen(LOW);

    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2;

    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < matrixtext.length() ) {
        matrix.drawChar(x, y, matrixtext[letter], HIGH, LOW, 1);
        
      }
      letter--;
      x -= width;
    }

    matrix.write(); //Display beschreiben
    delay(wait);    //Warten für Scroll Geschwindigkeit
  }
}

void loop() {
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    // Ignore SSL certificate validation
    client->setInsecure();
    
    //create an HTTPClient instance
    HTTPClient https;
    
    //Initializing an HTTPS communication using the secure client
    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://api.coingecko.com/api/v3/simple/price?ids=litecoin%2Cbitcoin%2Cterra-luna&vs_currencies=eur&precision=00")) {  // 'precision' parameter allows to specify price data in full or 0-18 decimals -> https://apiguide.coingecko.com/getting-started/introduction
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);
          // load HTTP as JSON
          String jsonoutput = payload;
          DynamicJsonDocument config(1024);
          DeserializationError error = deserializeJson(config, jsonoutput);
          if (error)
            return;
            //get JSON content
          String litecoin = config["litecoin"]["eur"];
          Serial.println(litecoin);
          String bitcoin = config["bitcoin"]["eur"];
          Serial.println(bitcoin);
          String luna = config["terra-luna"]["eur"];
          Serial.println(luna);
          matrixtext = "LTC:" + litecoin + " BTC:" + bitcoin + " LUNC:" + luna + " ";
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
  Serial.println();
  delay(10000);
  show();
  delay(40000);
}
