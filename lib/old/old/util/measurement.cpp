#include "util/measurement.hpp"
#include "model/measurement/angle.hpp"
#include "model/measurement/dihedral_angle.hpp"
#include "model/measurement/distance.hpp"
#include "model/molecule.hpp"
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

	std::string getDihedralAngleString( const Model::Measurement::DihedralAngle & p_dihedralAngleModel )
	{
		return std::to_string( int( p_dihedralAngleModel.getAngle() ) ) + "\u00B0";
	}

} // namespace VTX::Util::Measurement
