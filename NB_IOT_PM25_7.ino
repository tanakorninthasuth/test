#include <SoftwareSerial.h>
//#include "Magellan_BC95_lite.h"
//Magellan_BC95_lite magel;

SoftwareSerial pmsSerial(15, 16);

String payload;

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting ... ");
  pmsSerial.begin(9600);
  //magel.begin();
  
}

struct pms5003data
{
  uint16_t framelen;
  uint16_t pm1_standard, pm2_5_standard, pm10_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};

struct pms5003data data;


void loop()
{
  if (readPMSdata(&pmsSerial))
  {
    int b = data.pm2_5_standard;
    Serial.print("PM2_5 : ");
    Serial.print(b);
    Serial.println(" ug/m3");

    int c = data.pm10_standard;
    Serial.print("PM10 : ");
    Serial.print(c);
    Serial.println(" ug/m3");


//    payload = "{\"PM2.5\":" + String(b) + ",\"PM10\":" + String(c) + "}"; //Please provide payload with json format
//    magel.report(payload);


    for (int i = 0; i < 10 ; i++) { // i<120
      delay(1000);
    //  wdt_reset();
    }
  }

}

boolean readPMSdata(Stream *s)
{
  if (! s->available())
  {
    return false;
  }
  if (s->peek() != 0x42)
  {
    s->read();
    return false;
  }
  if (s->available() < 32)
  {
    return false;
  }
  uint8_t buffer[32];
  uint16_t sum = 0;
  s->readBytes(buffer, 32);
  for (uint8_t i = 0; i < 30; i++)
  {
    sum += buffer[i];
  }
  uint16_t buffer_u16[15];
  for (uint8_t i = 0; i < 15; i++)
  {
    buffer_u16[i] = buffer[2 + i * 2 + 1];
    buffer_u16[i] += (buffer[2 + i * 2] << 8);
  }
  memcpy((void *)&data, (void *)buffer_u16, 30);
  if (sum != data.checksum)
  {
    return false;
  }
  return true;
}
