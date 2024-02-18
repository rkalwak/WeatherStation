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
Supla::Clock suplaClock;
Supla::Sensor::RainSensor* rainSensor;
bool resetOncePerHour=true;
#define ESP32

Supla::ESPWifi wifi(wifiSSID, wifiPass);
namespace Supla
{
  namespace Sensor
  {
class VoltageMeasurement : public Element
        {
        public:
            VoltageMeasurement(int pinDirection);

            void iterateAlways();
            void onInit();

        protected:
            Channel *getChannel()
            {
                return &channel;
            }
            Channel channel;
            unsigned long lastSendTime;
            int _pinInput;
        };
    VoltageMeasurement::VoltageMeasurement(int pin) : lastSendTime(0) // constructor
    {
      _pinInput=pin;
      pinMode(_pinInput, INPUT);
      analogSetPinAttenuation(_pinInput, ADC_11db);
      channel.setType(SUPLA_CHANNELTYPE_THERMOMETER);
      channel.setDefault(SUPLA_CHANNELFNC_THERMOMETER);
      channel.setNewValue(0);
    };

    void VoltageMeasurement::iterateAlways()
    {

      if (lastSendTime + 10000 < millis())
      {
        lastSendTime = millis();
        int mv=analogReadMilliVolts(_pinInput);
        Serial.print("Analog read of voltage:");
        Serial.println(mv);
        //2.686 - 8.4
        // read - x
        channel.setNewValue(mv*8400/2686/1000.0);
      }
    }

    void VoltageMeasurement::onInit()
    {
      channel.setNewValue(0);
    }

  }; // namespace Sensor
};   // namespace Supla
void setup()
{

  Serial.begin(115200);
  // Replace the falowing GUID with value that you can retrieve from https://www.supla.org/arduino/get-guid
  char GUID[SUPLA_GUID_SIZE] = {0x8E,0xD9,0xE0,0x4D,0xAF,0x3D,0x86,0x82,0xCB,0xD7,0xF0,0x24,0x3F,0xFA,0x56,0x63};

  // Replace the following AUTHKEY with value that you can retrieve from: https://www.supla.org/arduino/get-authkey
  char AUTHKEY[SUPLA_AUTHKEY_SIZE] = {0x3C,0xC9,0x60,0x2A,0x03,0xEB,0x1B,0xD1,0x36,0x85,0x20,0xF6,0xD2,0x2F,0x9A,0xE6};
  new Supla::Sensor::DS18B20(4);
  new Supla::Sensor::HDC1080();
  //rainSensor = new Supla::Sensor::RainSensor(2, 2.5, 300);
  //new Supla::Sensor::LightSensor();
  //new Supla::Sensor::MS5611Sensor(248);
  //new Supla::Sensor::Anemometr(15, 1, 1);
  //new Supla::Sensor::WindDirectionSensor(18,-45);
  //new Supla::Sensor::VoltageMeasurement(34);
  Wire.setClock(100000);

  SuplaDevice.begin(GUID,           // Global Unique Identifier
                    serverVariable, // SUPLA server address
                    emailVariable,  // Email address used to login to Supla Cloud
                    AUTHKEY);
  SuplaDevice.addClock(new Supla::Clock); 
}

void loop()
{
  SuplaDevice.iterate();
  // reset counter every hour as rain is counter hourly
  if(suplaClock.getMin() == 0 && resetOncePerHour)
  {
    //rainSensor->setReset(true);
    resetOncePerHour=false;
  }
  if(suplaClock.getMin() == 1 )
  {
    resetOncePerHour=true;
  }
}
