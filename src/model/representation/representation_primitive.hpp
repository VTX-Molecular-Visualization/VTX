#ifndef __VTX_MODEL_REPRESENTATION_PRIMITIVE__
#define __VTX_MODEL_REPRESENTATION_PRIMITIVE__

#include "generic/base_colorable.hpp"

namespace VTX::Model::Representation
{

	struct SphereData
	{
		SphereData();
		float radiusFixed;
		float radiusAdd;
		bool  isRadiusFixed;
	};

	struct CylinderData
	{
		CylinderData();
		float						 radius;
		Generic::COLOR_BLENDING_MODE colorBlendingMode;
	};

	struct RibbonData
	{
		RibbonData();
		Generic::SECONDARY_STRUCTURE_COLOR_MODE colorMode;
		Generic::COLOR_BLENDING_MODE			colorBlendingMode;
	};

} // namespace VTX::Model::Representation
#endif
