#include <MyHomeBridgeWifi.h>
#include <MyWifiSettings.h>

MyHomeBridgeWifi wifi;

const char *APssid = "ESPapYOyo";
const char *APpassword = "thereisnospoon";

void setup()
{
  Serial.begin(115200);

  wifi.accessPointConfig(APssid, APpassword);

  // IPAddress ip(192,168,0,93);
  // wifi.connect(MYWIFISSID, MYWIFIPASSWORD, ip);

  wifi.connect(MYWIFISSID, MYWIFIPASSWORD);

  // wifi.connect("helo", "world");

  // wifi.initServer();
  // wifi.serve();
}

void loop()
{
  wifi.check();
}
