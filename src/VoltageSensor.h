#include <Arduino.h>
#include <SuplaDevice.h>

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
      _pinInput = pin;
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
        int mv = analogReadMilliVolts(_pinInput);
        Serial.print("Analog read of voltage:");
        Serial.println(mv);
        // 2.686 - 8.4
        //  read - x
        channel.setNewValue(mv * 8400 / 2686 / 1000.0);
      }
    }

    void VoltageMeasurement::onInit()
    {
      channel.setNewValue(0);
    }

  }; // namespace Sensor
}; // namespace Supla