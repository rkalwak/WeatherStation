#include <Arduino.h>
#include <SuplaDevice.h>
#include <supla/sensor/DS18B20.h>
#include <Wire.h>
#include <HDC1080.h>
#include <MS5611Sensor.h>
#include <supla/sensor/rain.h>
#include <supla/sensor/wind.h>
#include <supla/sensor/pressure.h>
#include <supla/channel_element.h>
#include <supla/network/esp_wifi.h>
#include <LightSensor.h>
#include <RainSensor.h>
#include <supla/clock/clock.h>
#include <Anemometr.h>
#include <WindDirection.h>
#include <SPS30Sensor.h>
#include <VoltageSensor.h>
#include <supla/storage/littlefs_config.h>
#include <supla/network/esp_web_server.h>
#include <supla/network/html/device_info.h>
#include <supla/network/html/wifi_parameters.h>
#include <supla/network/html/protocol_parameters.h>
#include <supla/device/supla_ca_cert.h>
Supla::Clock suplaClock;
Supla::Sensor::RainSensor *rainSensor;
bool resetOncePerHour = true;

Supla::ESPWifi wifi(wifiSSID, wifiPass);
Supla::LittleFsConfig configSupla;

void setup()
{

  Serial.begin(115200);
  // Replace the falowing GUID with value that you can retrieve from https://www.supla.org/arduino/get-guid
  char GUID[SUPLA_GUID_SIZE] = {0x59, 0x37, 0xA9, 0x23, 0x69, 0x7E, 0x90, 0xD3, 0x48, 0x2A, 0x30, 0xC7, 0xA7, 0x96, 0xAF, 0x46};

  // Replace the following AUTHKEY with value that you can retrieve from: https://www.supla.org/arduino/get-authkey
  char AUTHKEY[SUPLA_AUTHKEY_SIZE] = {0xF9, 0x39, 0x9D, 0x7E, 0x4E, 0xE6, 0x6B, 0x92, 0x1E, 0x18, 0xBD, 0x9A, 0x39, 0xB2, 0xB2, 0x0E};
 
 Supla::Storage::Init();

  auto cfg = Supla::Storage::ConfigInstance();

  if (cfg)
  {
    char buf[100];
    if (!cfg->getGUID(buf))
    {
      Serial.println("[Main] setting config.");
      cfg->setGUID(GUID);
      cfg->setAuthKey(AUTHKEY);
      cfg->setWiFiSSID(wifiSSID);
      cfg->setWiFiPassword(wifiPass);
      cfg->setSuplaServer(serverVariable);
      cfg->setEmail(emailVariable);
    }
    else
    {
      Serial.println("[Main] Can load guid.");
    }
  }
  else
  {
    Serial.println("[Main] Config not found.");
  }
  new Supla::Sensor::LightSensor();
  Supla::Sensor::SPS30_X *sps30 = new Supla::Sensor::SPS30_X();
  auto spsPM005 = new Supla::Sensor::SPS30_PM005(sps30);
  auto spsPM01 = new Supla::Sensor::SPS30_PM01(sps30);
  auto spsPM025 = new Supla::Sensor::SPS30_PM025(sps30);
  auto spsPM04 = new Supla::Sensor::SPS30_PM04(sps30);
  auto spsPM10 = new Supla::Sensor::SPS30_PM10(sps30);
  
  new Supla::Sensor::DS18B20(4);
  new Supla::Sensor::HDC1080();  
  new Supla::Sensor::MS5611Sensor(248);
  rainSensor = new Supla::Sensor::RainSensor(2, 2.5, 300);
  new Supla::Sensor::WindDirectionSensor(18, 15);

  new Supla::Sensor::Anemometr(15, 1, 1);

  // new Supla::Sensor::VoltageMeasurement(34);
  
  Wire.setClock(100000);
  
  SuplaDevice.setSuplaCACert(suplaCACert);
  SuplaDevice.setSupla3rdPartyCACert(supla3rdCACert);
  SuplaDevice.begin();
  SuplaDevice.addClock(new Supla::Clock);
}

void loop()
{
  SuplaDevice.iterate();
  // reset counter every hour as rain is counter hourly
  if (suplaClock.getMin() == 0 && resetOncePerHour)
  {
    rainSensor->setReset(true);
    resetOncePerHour = false;
  }
  if (suplaClock.getMin() == 1)
  {
    resetOncePerHour = true;
  }
}
