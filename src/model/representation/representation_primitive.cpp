#include "representation_primitive.hpp"
#include "setting.hpp"

namespace VTX::Model::Representation
{

	SphereData::SphereData() :
		_radiusFixed( Setting::ATOMS_RADIUS_DEFAULT ), _radiusAdd( Setting::ATOMS_RADIUS_ADD_DEFAULT ),
		_isRadiusFixed( Setting::ATOMS_IS_RADIUS_FIXED )
	{
	}

	CylinderData::CylinderData() :
		_radius( Setting::BONDS_RADIUS_DEFAULT ), _colorBlendingMode( Setting::BONDS_COLOR_BLENDING_MODE_DEFAULT )
	{
	}

	RibbonData::RibbonData() :
		_colorMode( Setting::SS_COLOR_MODE_DEFAULT ), _colorBlendingMode( Setting::SS_COLOR_BLENDING_MODE_DEFAULT )
	{
	}
} // namespace VTX::Model::Representation
