#ifndef MyHomeBridgeWifi_h
#define MyHomeBridgeWifi_h

#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "ESP8266WebServer.h"

class MyHomeBridgeWifi
{
  public:
    bool checkUrlCall = false;
	bool checkPing = true;
	String lastPayload;
	void accessPointConfig(const char* ssid, const char *passphrase);
	void accessPointStart();
    void connect(const char* ssid, const char *passphrase);
	void connect(const char* ssid, const char *passphrase, IPAddress local_ip);
    void connect();
	bool isConnected();
	int callUrl(String url);
	void check();
	ESP8266WebServer server;
	void initServer(int port = 80);
	void serve();
	void serve(ESP8266WebServer::THandlerFunction fnNotFound);	
	
  private:
    bool _isConnected = false;
    const char * _STAssid;
    const char * _STApassphrase;
    const char * _APssid;
    const char * _APpassphrase;	
	long _lastCheck;	
	IPAddress _gatewayIP;
	
	bool _checkUrlCall();
	bool _ping();
	void _routWifiConfig();
};

#endif
