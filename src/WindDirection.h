#ifndef _WindDirection_h
#define _WindDirection_h

#include "supla/channel.h"
#include "supla/element.h"
#include "AS5600.h"
#include "AverageAngle.h"
namespace Supla
{
    namespace Sensor
    {
        class WindDirectionSensor : public Element
        {
        public:
            WindDirectionSensor(int pinDirection, int angleAdjustmentToNorth);

            void iterateAlways();
            void onInit();

        protected:
            Channel *getChannel()
            {
                return &channel;
            }
            Channel channel;
            AS5600 as5600;
            AverageAngle averageAngle;
            unsigned long lastReadTime;
            unsigned long lastSendTime;
            int _pinDirection;
            int _angleAdjustmentToNorth;
        };

    }; // namespace Sensor
};     // namespace Supla

#endif