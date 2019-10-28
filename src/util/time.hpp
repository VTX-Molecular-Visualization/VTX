#ifndef __MY_TIME__
#define __MY_TIME__

#include <iostream>

namespace VTX
{
	namespace Util
	{
		class Time
		{
		  public:
			static std::string getNowString();

		  private:
			Time() = default;
		};
	} // namespace Util
} // namespace VTX

#endif
