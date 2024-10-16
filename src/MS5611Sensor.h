/*
Copyright (C) krycha88

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

#ifndef _ms5611Sensor_h
#define _ms5611Sensor_h

// Dependency: MS5611 library - use library manager to install it
// https://github.com/RobTillaart/MS5611

#include <MS5611.h>
#include <Wire.h>
#include <supla/sensor/pressure.h>

namespace Supla
{
  namespace Sensor
  {
    class MS5611Sensor : public Supla::Sensor::Pressure
    {
    public:
      MS5611Sensor(int altitude)
      {
        this->altitude = altitude;
      }

      double getValue()
      {
        Serial.println(F("MS5611 getting pres."));

        float value = -123;
#ifdef normalMode
        int result = ms5611.read();
        if (result != MS5611_READ_OK)
        {
          Serial.print("Error in read: ");
          Serial.println(result);
        }
        else
        {
          value = ms5611.getPressure();
          float temp = ms5611.getTemperature();
          float multiplier = 0.0065;
          value = pow(((multiplier * altitude) / (temp + 273.15) + 1), 5.255) * value;
        }
#endif
        return value;
      }

      void onInit() override
      {
        Serial.println(F("MS5611 init."));
#ifdef normalMode
        ms5611 = MS5611(0x77);
        if (ms5611.begin() == true)
        {
          Serial.println("MS5611 found.");
        }
        else
        {
          Serial.println("MS5611 not found.");
        }
#endif
        channel.setNewValue(getValue(), -1.0);
      }

    protected:
      ::MS5611 ms5611; // I2C
      float altitude = 248;
    };

  }; // namespace Sensor
}; // namespace Supla

#endif
