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
#include "Anemometr.h"
#include <Arduino.h>
namespace Supla
{
  namespace Sensor
  {

    unsigned long lastWindTime;
    long Anemometr::_currentWindCount = 0;
    unsigned long Anemometr::_shortestWindTime = 0;

    void ICACHE_RAM_ATTR serviceInterruptAnem()
    {
      unsigned long currentTime = (unsigned long)(micros() - lastWindTime);

      lastWindTime = micros();
      if (currentTime > 1000) // debounce
      {
        Anemometr::_currentWindCount++;
        if (currentTime < Anemometr::_shortestWindTime)
        {
          Anemometr::_shortestWindTime = currentTime;
        }
      }
    }

    Anemometr::Anemometr(int pinAnem, double wind_factory, int selectedMode) : lastReadTime(0) // constructor
    {

      channel.setType(SUPLA_CHANNELTYPE_WINDSENSOR);
      channel.setDefault(SUPLA_CHANNELFNC_WINDSENSOR);
      channel.setNewValue(WIND_NOT_AVAILABLE);

      _pinAnem = pinAnem;
      _wind_factory = wind_factory;
      _currentWindCount = 0;
      _currentWindSpeed = 0.0;

      lastWindTime = 0;
      _shortestWindTime = 0xffffffff;

      _sampleTime = 5.0;
      _startSampleTime = micros();
    };

    double Anemometr::getValue() // in milliseconds
    {

      if (!_selectedMode)
      {
        _currentWindSpeed = get_current_wind_speed_when_sampling();
      }
      else
      {

        startWindSample(SAMPLETIME);
        _currentWindCount = 0;
        delay(_sampleTime * 1000);
        _currentWindSpeed = ((float)_currentWindCount / _sampleTime) * _wind_factory;
      }
      return _currentWindSpeed;
    };

    void Anemometr::iterateAlways()
    {
      if (millis()- lastReadTime > 10000 )
      {
        lastReadTime = millis();
        channel.setNewValue(getValue());
        Serial.print("Speed: ");
        Serial.println(getValue());
      }
    }

    float Anemometr::get_wind_gust()
    {
      unsigned long latestTime;
      latestTime = _shortestWindTime;
      _shortestWindTime = 0xffffffff;
      double time = latestTime / 1000000.0; // in microseconds

      return (1 / (time)) * _wind_factory / 2;
    };

    void Anemometr::reset_wind_gust()
    {
      _shortestWindTime = 0xffffffff;
    };

    void Anemometr::startWindSample(float sampleTime)
    {

      _startSampleTime = micros();

      _sampleTime = sampleTime;
    };

    float Anemometr::get_current_wind_speed_when_sampling()
    {

      unsigned long compareValue;
      compareValue = _sampleTime * 1000000;

      if (micros() - _startSampleTime >= compareValue)
      {
        // sample time exceeded, calculate currentWindSpeed
        float _timeSpan;
        //     _timeSpan = (unsigned long)(micros() - _startSampleTime);
        _timeSpan = (micros() - _startSampleTime);

        _currentWindSpeed = ((float)_currentWindCount / (_timeSpan)) * _wind_factory * 1000000;

        _currentWindCount = 0;

        _startSampleTime = micros();
      }
      else
      {
        // Serial.println("NOT Triggered");
        // Serial.print("time = ");
        // Serial.println(micros() - _startSampleTime);
        //  if not, then return last wind speed
      }
      return _currentWindSpeed;
    };

    void Anemometr::onInit()
    {

      pinMode(_pinAnem, INPUT_PULLUP);
      digitalWrite(_pinAnem, HIGH);
      attachInterrupt(digitalPinToInterrupt(_pinAnem), serviceInterruptAnem, FALLING);
      channel.setNewValue(getValue());
    }

  }; // namespace Sensor
};   // namespace Supla