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
#include <supla/storage/littlefs_config.h>
#include <supla/network/esp_web_server.h>
#include <supla/network/html/device_info.h>
#include <supla/network/html/wifi_parameters.h>
#include <supla/network/html/protocol_parameters.h>
Supla::Clock suplaClock;
Supla::Sensor::RainSensor* rainSensor;
bool resetOncePerHour=true;

//Supla::ESPWifi wifi(wifiSSID, wifiPass);
Supla::ESPWifi wifi;
Supla::LittleFsConfig configSupla;
Supla::EspWebServer suplaServer;
Supla::Html::DeviceInfo htmlDeviceInfo(&SuplaDevice);
Supla::Html::WifiParameters htmlWifi;
Supla::Html::ProtocolParameters htmlProto;

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

  new Supla::Sensor::DS18B20(4);
  new Supla::Sensor::HDC1080();
  rainSensor = new Supla::Sensor::RainSensor(2, 2.5, 300);
  new Supla::Sensor::LightSensor();
  new Supla::Sensor::MS5611Sensor(248);
  new Supla::Sensor::Anemometr(15, 1, 1);
  new Supla::Sensor::WindDirectionSensor(18,0);
  //new Supla::Sensor::VoltageMeasurement(34);
  Wire.setClock(100000);
 SuplaDevice.begin();
}

void loop()
{
  SuplaDevice.iterate();
}
