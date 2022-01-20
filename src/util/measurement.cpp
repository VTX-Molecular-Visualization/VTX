#include "util/measurement.hpp"
#include "model/measurement/angle.hpp"
#include "model/measurement/distance.hpp"
#include "style.hpp"
#include "util/string.hpp"
#include <string>

namespace VTX::Util::Measurement
{
	std::string getDistanceString( const Model::Measurement::Distance & p_distanceModel )
	{
		return Util::String::floatToStr( p_distanceModel.getDistance(), Style::MEASUREMENT_DISTANCE_NB_DECIMALS )
			   + "\u212B";
	}

	std::string getAngleString( const Model::Measurement::Angle & p_angleModel )
	{
		return std::to_string( int( p_angleModel.getAngle() ) ) + "\u00B0";
	}
} // namespace VTX::Util::Measurement
