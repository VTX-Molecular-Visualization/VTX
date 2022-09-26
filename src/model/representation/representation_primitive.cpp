#include "representation_primitive.hpp"
#include "setting.hpp"

namespace VTX::Model::Representation
{

	SphereData::SphereData() :
		radiusFixed( Setting::ATOMS_RADIUS_DEFAULT ), radiusAdd( Setting::ATOMS_RADIUS_ADD_DEFAULT ),
		isRadiusFixed( Setting::ATOMS_IS_RADIUS_FIXED_DEFAULT )
	{
	}

	CylinderData::CylinderData() :
		radius( Setting::BONDS_RADIUS_DEFAULT ), colorBlendingMode( Setting::BONDS_COLOR_BLENDING_MODE_DEFAULT )
	{
	}

	RibbonData::RibbonData() :
		colorMode( Setting::SS_COLOR_MODE_DEFAULT ), colorBlendingMode( Setting::SS_COLOR_BLENDING_MODE_DEFAULT )
	{
	}

	SESData::SESData() : resolution( Setting::SES_RESOLUTION_DEFAULT ) {}
} // namespace VTX::Model::Representation
