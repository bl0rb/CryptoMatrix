# CryptoMatrix
Using Arduino with Matrix Display to monitor Crypto

## Setup
- Arduino Board (D1 Mini NodeMcu with ESP8266-12F in my case)
- Matrix Display (MAX7219 8x32 4 in 1 Dot Matrix LED)
  - <ESP8266WiFi.h>              //WiFi
  - <ESP8266HTTPClient.h>        //HTTP Client
  - <WiFiClientSecureBearSSL.h>  //Secure Connection (SSL)
  - <SPI.h>                      //SPI for Matrix
  - <Adafruit_GFX.h>             //charset for Matrix
  - <Max72xxPanel.h>             //Driver MAX7219 for Matrix
  - <ArduinoJson.h>              //JSON Library
