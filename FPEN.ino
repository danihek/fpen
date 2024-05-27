#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <HTTPUpdateServer.h>

Adafruit_MPU6050 mpu;
WiFiUDP udp;

const char * ssid = "";
const char * password = "";

const char * hostname = "fpen wireless";

const int port = 6969;

const int bufferSize = 50;
char buffer[bufferSize];

void setup(void)
{
  Serial.begin(115200);

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  WiFi.setHostname(hostname);

  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    WiFi.begin(ssid, password);
    Serial.println("WiFi failed, retrying.");
  }

  udp.begin(port);

  /* Failed to find MPU6050 chip! */
  if (!mpu.begin())
  {
    while (1)
    {
      int packetSize = udp.parsePacket();
      if (packetSize)
      {
        int len = udp.read(buffer, bufferSize);
        if (len > 0)
        {
          udp.beginPacket(udp.remoteIP(), udp.remotePort());
          sprintf(buffer, "%d%d%d", -1,-1,-1);
          udp.write((const uint8_t *)buffer,strlen(buffer));
          udp.endPacket();
        }
      }
      delay(10);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop()
{
  if (udp.parsePacket())
  {
    int len = udp.read(buffer, bufferSize);
    if (len > 0)
    {
      IPAddress remoteIP = udp.remoteIP();

      while (1)
      {
        sensors_event_t a, g, temp;
        mpu.getEvent(&g, &a, &temp);
        
        sprintf(buffer, "%.2f,%.2f,%.2f", a.acceleration.x, a.acceleration.y, a.acceleration.z);

        udp.beginPacket(udp.remoteIP(), udp.remotePort());
        udp.write((const uint8_t *)buffer,strlen(buffer));
        udp.endPacket();
        
        bzero(buffer,bufferSize);
      }
    }
  }
}
