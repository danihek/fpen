#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <HTTPUpdateServer.h>

#define S_FACTOR 16

Adafruit_MPU6050 mpu;
WiFiUDP udp;

const char * ssid = "";
const char * password = "";

const char * hostname = "fpen wireless";

const int port = 6969;

const int bufferSize = 255;
char buffer[bufferSize];

int8_t f_fixed(float f)
{
  // TODO - change S_FACTOR to more precise value
  int s = round(f * S_FACTOR);
  if (s > 127)
  {
    s = 127;
  }
  else if (s < -128)
  {
    s = -128;
  }
  return (int8_t)s;
}

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

  if (!mpu.begin())
  {
    char *message = "Failed to find MPU6050 chip";
    while (1)
    {
      int packetSize = udp.parsePacket();
      if (packetSize)
      {
        int len = udp.read(buffer, bufferSize);
        if (len > 0)
        {
          udp.beginPacket(udp.remoteIP(), udp.remotePort());
          udp.write((const uint8_t *)message,strlen(message));
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
  int packetSize = udp.parsePacket();
  if (packetSize)
  {
    int len = udp.read(buffer, bufferSize);
    if (len > 0)
    {
      buffer[len] = 0;
      IPAddress remoteIP = udp.remoteIP();
      
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);
      
      // TODO - add availibility to send more data with one request
      switch (buffer[0])
      {
        case '0':
          bzero(buffer,bufferSize);
          sprintf(buffer, "%d", f_fixed(a.acceleration.x));
          break;
 
        case '1':
          bzero(buffer,bufferSize);
          sprintf(buffer, "%d", f_fixed(a.acceleration.y));
          break;
 
        case '2':
          bzero(buffer,bufferSize);
          sprintf(buffer, "%d", f_fixed(a.acceleration.z));
          break;
 
        case '3':
          bzero(buffer,bufferSize);
          sprintf(buffer, "%d", f_fixed(g.gyro.x));
          break;
 
        case '4':
          bzero(buffer,bufferSize);
          sprintf(buffer, "%d", f_fixed(g.gyro.y));
          break;
 
        case '5':
          bzero(buffer,bufferSize);
          sprintf(buffer, "%d", f_fixed(g.gyro.z));
          break;
        default:
          sprintf(buffer, "%f", 0.f);
          break;
      }

      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      udp.write((const uint8_t *)buffer,strlen(buffer));
      udp.endPacket();

      bzero(buffer,bufferSize);
    }
  }
}
