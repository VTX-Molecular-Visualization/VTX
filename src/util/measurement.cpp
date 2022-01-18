#include "util/measurement.hpp"
#include "model/measurement/distance.hpp"
#include "style.hpp"
#include "util/string.hpp"

namespace VTX::Util::Measurement
{
	std::string getDistanceString( const Model::Measurement::Distance & p_distanceModel )
	{
		return Util::String::floatToStr( p_distanceModel.getDistance(), Style::MEASUREMENT_DISTANCE_NB_DECIMALS )
			   + "\u212B";
	}

} // namespace VTX::Util::Measurement
