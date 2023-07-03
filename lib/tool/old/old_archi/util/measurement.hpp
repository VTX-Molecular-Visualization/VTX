#ifndef __VTX_UTIL_MEASUREMENT__
#define __VTX_UTIL_MEASUREMENT__

#include <algorithm>
#include <string>

namespace VTX::Model::Measurement
{
	class Distance;
	class Angle;
	class DihedralAngle;
} // namespace VTX::Model::Measurement

namespace VTX::Util::Measurement
{
	std::string getDistanceString( const Model::Measurement::Distance & p_distanceModel );
	std::string getAngleString( const Model::Measurement::Angle & p_angleModel );
	std::string getDihedralAngleString( const Model::Measurement::DihedralAngle & p_dihedralAngleModel );

} // namespace VTX::Util::Measurement

#endif
