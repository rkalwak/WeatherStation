#include <supla/channel_element.h>
#include "Adafruit_LTR390.h"
namespace Supla
{
    namespace Sensor
    {

        class LightSensor : public ChannelElement
        {
        public:
            LightSensor() : lastReadTime(0)
            {
                channel.setType(SUPLA_CHANNELTYPE_THERMOMETER);
                channel.setDefault(SUPLA_CHANNELFNC_THERMOMETER);
                channel.setNewValue(0.0);
            };

            void onInit()
            {
                ltr = Adafruit_LTR390();
                ltr.begin();
                ltr.setGain(LTR390_GAIN_3);
                ltr.setResolution(LTR390_RESOLUTION_16BIT);
                ltr.setThresholds(100, 1000);
                ltr.configInterrupt(true, LTR390_MODE_ALS);
                ltr.setMode(LTR390_MODE_ALS);
            }
            double getValue()
            {
                /*
                if (ltr.newDataAvailable())
                {
                  ltr.setMode(LTR390_MODE_UVS);
                  Serial.print("UV data: ");
                  Serial.println(ltr.readUVS());
                }
                */
                return ltr.readALS();
            }
            void iterateAlways()
            {
                if (lastReadTime + 10000 < millis())
                {
                    lastReadTime = millis();
                    channel.setNewValue(getValue());
                }
            }

        protected:
            uint64_t lastReadTime;
            Adafruit_LTR390 ltr;
        };
    };
};