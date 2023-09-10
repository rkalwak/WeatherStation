#ifndef _Anemometr_h
#define _Anemometr_h

#include "supla/channel.h"
#include "supla/element.h"

#define WIND_NOT_AVAILABLE -1
#define SAMPLETIME 5.0

namespace Supla
{
	namespace Sensor
	{
		class Anemometr : public Element
		{
		public:
			Anemometr(int pinAnem, double wind_factory, int selectedMode);

			float get_wind_gust();
			void reset_wind_gust();
			static unsigned long _shortestWindTime;
			static long _currentWindCount;
			friend void serviceInterruptAnem();
			virtual double getValue();
			void iterateAlways();
			void onInit();

		protected:
			Channel *getChannel()
			{
				return &channel;
			}
			Channel channel;

			unsigned long lastReadTime;
			double _wind_factory;
			int _pinAnem;
			int _selectedMode;
			unsigned long _startSampleTime;
			float _currentWindSpeed;
			void startWindSample(float sampleTime);
			float get_current_wind_speed_when_sampling();
			float _sampleTime;
		};

	}; // namespace Sensor
};	   // namespace Supla

#endif