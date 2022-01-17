#ifndef __VTX_UTIL_MEASUREMENT__
#define __VTX_UTIL_MEASUREMENT__

#include <algorithm>
#include <string>

namespace VTX::Model::Measurement
{
	class Distance;
}

namespace VTX::Util::Measurement
{
	std::string getDistanceString( const Model::Measurement::Distance & p_distanceModel );
} // namespace VTX::Util::Measurement

#endif
