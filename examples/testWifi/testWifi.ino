#include <MyHomeBridgeWifi.h>
#include <MyWifiSettings.h>

MyHomeBridgeWifi wifi;

void setup()
{
  Serial.begin(9600);

  IPAddress ip(192,168,0,93);
  wifi.init(MYWIFISSID, MYWIFIPASSWORD, ip);
  wifi.connect();
}

void loop()
{
  delay(3000);
  wifi.check();
}
