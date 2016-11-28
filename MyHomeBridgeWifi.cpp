#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "MyHomeBridgeWifi.h"

void MyHomeBridgeWifi::init(const char* ssid, const char *passphrase)
{
    _ssid = ssid;
    _passphrase = passphrase;
	_lastCheck = millis();
}

void MyHomeBridgeWifi::init(const char* ssid, const char *passphrase, IPAddress local_ip)
{
    IPAddress gateway(192,168,0,1);
	IPAddress subnet(255,255,255,0);	
	
	WiFi.config(local_ip, gateway, subnet);
	init(ssid, passphrase);
}

void MyHomeBridgeWifi::connect()
{
  WiFi.disconnect(true);
  Serial.print("Connect to WiFi: ");  
  Serial.println(_ssid);
  WiFi.begin(_ssid, _passphrase);  

  if (isConnected()) {
	Serial.print("Connected to wifi: ");
	Serial.println(WiFi.localIP());	  	  
  }
}

bool MyHomeBridgeWifi::isConnected() {
  int test = 20;
  while (test && WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");  
    test--;
  }
  return test;
}

int MyHomeBridgeWifi::callUrl(String url) {
  Serial.print("Call url: ");
  Serial.println(url);
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if(httpCode > 0) {
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
  } 
  else {
        Serial.printf("[HTTP] GET... failed, error: %d\n", httpCode);
  }
  http.end(); 

  return httpCode;
}

bool MyHomeBridgeWifi::_checkUrlCall() {
  bool isValid = true;
  if (millis() - _lastCheck > 60000) { // check every minute
    _lastCheck = millis();
    Serial.print("Call check url: ");
    int httpCode = callUrl("http://192.168.0.1/");
    Serial.println(httpCode);
    isValid = httpCode == 200;
  }

  return isValid;
}

void MyHomeBridgeWifi::check() {       
  if (!isConnected() || !_checkUrlCall()) {
      Serial.println("\nDisconnected from Wifi, reset in 5 sec.");
      delay(5000);
      //connectToWifi();
      ESP.reset();
  }  
}
