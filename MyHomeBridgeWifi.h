#ifndef MyHomeBridgeWifi_h
#define MyHomeBridgeWifi_h

#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"

class MyHomeBridgeWifi
{
  public:
    void init(const char* ssid, const char *passphrase);
	void init(const char* ssid, const char *passphrase, IPAddress local_ip);
    void connect();
	bool isConnected();
	int callUrl(String url);
	void check();
	
  private:
    const char * _ssid;
    const char * _passphrase;
	long _lastCheck;	
	
	bool _checkUrlCall();
};

#endif