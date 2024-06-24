
#include <supla/sensor/rain.h>
namespace Supla
{
    namespace Sensor
    {

        unsigned long _lastRainTime;
        unsigned long _currentRainMin;
        double _bucketSize;
        double _currentRainCount;
        int debounceTimeGlobal = 0;
        void ICACHE_RAM_ATTR rain_detect()
        {
            unsigned long currentTime = (unsigned long)(micros() - _lastRainTime);

            _lastRainTime = micros();
            if (currentTime > debounceTimeGlobal) // debounce
            {
                _currentRainCount += 1;

                if (currentTime < _currentRainMin)
                {
                    _currentRainMin = currentTime;
                }
            }
        }

        class RainSensor : public Supla::Sensor::Rain
        {
        public:
            RainSensor(int pinRain, double bucketSize, int debounceTime)
            {
                channel.setType(SUPLA_CHANNELTYPE_RAINSENSOR);
                channel.setDefault(SUPLA_CHANNELFNC_RAINSENSOR);
                channel.setNewValue(RAIN_NOT_AVAILABLE);
                _bucketSize = bucketSize;
                _pinRain = pinRain;
                debounceTimeGlobal = debounceTime;
            };

            void onInit() override
            {
                // pinMode(_pinRain, INPUT_PULLUP);
                attachInterrupt(digitalPinToInterrupt(_pinRain), rain_detect, FALLING);
                channel.setNewValue(getValue());
            }
            double getValue()
            {
                return _currentRainCount * _bucketSize;
            }

            void iterateAlways()
            {
                if (millis() -lastReadTime > 10000)
                {
                    lastReadTime = millis();
                    channel.setNewValue(getValue());
                }
            }

            void setReset(bool value)
            {
                if (value)
                    _currentRainCount = 0;
            }

        protected:
            int _pinRain;
            bool _reset;

            unsigned long _lastReadTime;
        };
    };
};