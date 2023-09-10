/*
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <Time.h>
#include "WindDirection.h"
#include <Arduino.h>
namespace Supla
{
  namespace Sensor
  {

    WindDirectionSensor::WindDirectionSensor(int pinDirection, int angleAdjustmentToNorth) : lastReadTime(0), lastSendTime(0) // constructor
    {
      _pinDirection=pinDirection;
      _angleAdjustmentToNorth=angleAdjustmentToNorth;
      channel.setType(SUPLA_CHANNELTYPE_THERMOMETER);
      channel.setDefault(SUPLA_CHANNELFNC_THERMOMETER);
      channel.setNewValue(0);
    };

    void WindDirectionSensor::iterateAlways()
    {
      if (lastReadTime + 1000 < millis())
      {
        lastReadTime = millis();
        averageAngle.add(as5600.rawAngle() * AS5600_RAW_TO_DEGREES+_angleAdjustmentToNorth);
      }
      if (lastSendTime + 10000 < millis())
      {
        lastSendTime = millis();
        channel.setNewValue(averageAngle.getAverage());
        Serial.print("Wind direction: ");
        Serial.println(averageAngle.getAverage());
        averageAngle.reset();
      }
    }

    void WindDirectionSensor::onInit()
    {
      as5600.begin(_pinDirection);            //  set direction pin.
      as5600.setDirection(AS5600_CLOCK_WISE); // default, just be explicit.
      channel.setNewValue(0);
      averageAngle = AverageAngle(AverageAngle::DEGREES);
      averageAngle.reset();
    }

  }; // namespace Sensor
};   // namespace Supla