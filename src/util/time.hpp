#ifndef __VTX__TIME__
#define __VTX__TIME__

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
