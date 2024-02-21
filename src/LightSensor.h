#include "Adafruit_LTR390.h"
#include <supla/sensor/general_purpose_measurement.h>
namespace Supla
{
    namespace Sensor
    {

        class LightSensor : public GeneralPurposeMeasurement
        {
        public:
            LightSensor() : lastReadTime(0), GeneralPurposeMeasurement(nullptr, false)
            {
                setDefaultUnitAfterValue("lx");                
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
            double getValue() override
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

        protected:
            uint64_t lastReadTime;
            Adafruit_LTR390 ltr;
        };
    };
};