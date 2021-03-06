#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "ESP8266WebServer.h"
#include "WiFiClient.h"
#include "MyHomeBridgeWifi.h"

void MyHomeBridgeWifi::initServer(int port)
{
  server = * new ESP8266WebServer(port);
}

void MyHomeBridgeWifi::_routWifiConfig()
{
  Serial.println("_routWifiConfig.");
  if (server.hasArg("ssid") && server.hasArg("password")
   && (server.arg("ssid") != WiFi.SSID() || server.arg("password") != WiFi.psk())) {
    server.send(200, "text/html", "connect to wifi...");
    connect(server.arg("ssid").c_str(), server.arg("password").c_str());
  }
  server.send(200, "text/html", "\
  <form>\
    <label>Wifi SSID</label><br />\
    <input name='ssid' placeholder='SSID' value='" + WiFi.SSID() + "' /><br /><br />\
    <label>Wifi password</label><br />\
    <input name='password' placeholder='password' value='" + WiFi.psk() + "' /><br /><br />\
    <button type='submit'>Save</button><br />\
  <form>");
}

void MyHomeBridgeWifi::serve()
{
  serve(std::bind(&MyHomeBridgeWifi::_routWifiConfig, this));
}

void MyHomeBridgeWifi::serve(ESP8266WebServer::THandlerFunction fnNotFound)
{
	server.onNotFound(fnNotFound);
	server.begin();
	Serial.println("HTTP server started for wifi configuration.");
}

void MyHomeBridgeWifi::accessPointConfig(const char* ssid, const char *passphrase)
{
  _APssid = ssid;
  _APpassphrase = passphrase;
}

void MyHomeBridgeWifi::accessPointStart()
{
  Serial.print("Start access point: ");
  Serial.println(_APssid);
	WiFi.softAP(_APssid, _APpassphrase);

	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);  
}

void MyHomeBridgeWifi::connect(const char* ssid, const char *passphrase)
{
  _STAssid = ssid;
  _STApassphrase = passphrase;
	_lastCheck = millis();
  connect();
}

void MyHomeBridgeWifi::connect(const char* ssid, const char *passphrase, IPAddress local_ip)
{
  IPAddress gateway(192,168,0,1); 
  IPAddress subnet(255,255,255,0);
  connect(ssid, passphrase, local_ip, gateway, subnet);
}

void MyHomeBridgeWifi::connect(const char* ssid, const char *passphrase, IPAddress local_ip, IPAddress gateway, IPAddress subnet)
{
  WiFi.config(local_ip, gateway, subnet);
	connect(ssid, passphrase);
}

void MyHomeBridgeWifi::connect()
{
  WiFi.disconnect(true);
  WiFi.softAPdisconnect();
  isConnected = false;
  Serial.print("Connect to WiFi: ");  
  Serial.println(_STAssid);
  WiFi.begin(_STAssid, _STApassphrase);  

  if (isConnectionAlive()) {
	  Serial.print("\n\nConnected to wifi: ");
    Serial.println(WiFi.SSID());
	  Serial.println(WiFi.localIP());	  	  
    Serial.print("GatewayIP: ");
    Serial.println(WiFi.gatewayIP().toString());
    Serial.print("mac: ");
    Serial.println(WiFi.macAddress());   
    _gatewayIP =  WiFi.gatewayIP();
  }
  else {
    Serial.println("\n\nCan't connect to wifi.");
    accessPointStart();
    initServer();
    serve();
  }
}

bool MyHomeBridgeWifi::isConnectionAlive() {
  int test = 40;
  while (test && WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");  
    test--;
  }
  isConnected = test;
  return isConnected;
}

int MyHomeBridgeWifi::callUrl(String url) {
  Serial.print("Call url: ");
  Serial.println(url);
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if(httpCode > 0) {
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
		lastPayload = http.getString();
  } 
  else {
        Serial.printf("[HTTP] GET... failed, error: %d\n", httpCode);
  }
  http.end(); 

  return httpCode;
}

bool MyHomeBridgeWifi::_checkUrlCall() {
  Serial.print("Call check url: ");
  int httpCode = callUrl("http://" + _gatewayIP.toString() + "/");
  Serial.println(httpCode);

  return httpCode == 200;
}

void MyHomeBridgeWifi::_ping() {
  Serial.println("Ping");
  
  memset(&_pingOptions, 0, sizeof(struct ping_option));
  
  // Repeat count (how many time send a ping message to destination)
  _pingOptions.count = 0;
  // Time interval between two ping (seconds??)
  _pingOptions.coarse_time = 1;
  // Destination machine
  _pingOptions.ip = _gatewayIP;  
  _pingOptions.recv_function = reinterpret_cast<ping_recv_function>(&MyHomeBridgeWifi::_ping_recv);
  _pingOptions.sent_function = NULL;

  ping_start(&_pingOptions);
}

void MyHomeBridgeWifi::check() 
{
  if (isConnected && millis() - _lastCheck > 60000) { // check every minute
    _lastCheck = millis();     
    if (checkPing) _ping();
    if (!isConnectionAlive() || (checkUrlCall && !_checkUrlCall())) {
      MyHomeBridgeWifi::_disconnected();
    } 
  }
  
  server.handleClient();
}

void MyHomeBridgeWifi::_ping_recv(void *opt, void *pdata) 
{
  Serial.println("Pong");
  ping_resp*   ping_resp = reinterpret_cast<struct ping_resp*>(pdata);
  ping_option* ping_opt  = reinterpret_cast<struct ping_option*>(opt);

  if (ping_resp->ping_err == -1) _pingCountError++;
  else _pingCountError = 0;

  if (_pingCountError > 2) _disconnected();
}

void MyHomeBridgeWifi::_disconnected()
{
    Serial.println("\nDisconnected from Wifi, reset in 5 sec.");
    delay(5000);
    //connectToWifi();
    ESP.reset();
}

byte MyHomeBridgeWifi::_pingCountError = 0;
