#include <supla/channel_element.h>
#include "Adafruit_LTR390.h"
#include <supla/sensor/general_purpose_measurement.h>
namespace Supla
{
    namespace Sensor
    {

        class LightSensor : public GeneralPurposeMeasurement
        {
        public:
            LightSensor()
            {
                this->setInitialCaption("Natężenie światła");
                this->setDefaultUnitAfterValue("lx");
            };

            void onInit()
            {
#ifdef normalMode
                ltr = Adafruit_LTR390();
                ltr.begin();
                ltr.setGain(LTR390_GAIN_3);
                ltr.setResolution(LTR390_RESOLUTION_16BIT);
                ltr.setThresholds(100, 1000);
                ltr.configInterrupt(true, LTR390_MODE_ALS);
                ltr.setMode(LTR390_MODE_ALS);
#endif
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
               #ifdef normalMode
                return ltr.readALS();
                #else
                return 0.0;
                #endif
            
            }

        protected:
            Adafruit_LTR390 ltr;
        };
    };
};