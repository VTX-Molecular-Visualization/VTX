#ifndef __VTX_MODEL_REPRESENTATION_PRIMITIVE__
#define __VTX_MODEL_REPRESENTATION_PRIMITIVE__

#include "generic/base_colorable.hpp"

namespace VTX::Model::Representation
{

	class SphereData
	{
	  public:
		SphereData();
		float _radiusFixed;
		float _radiusAdd;
		bool  _isRadiusFixed;
	};

	class CylinderData
	{
	  public:
		CylinderData();
		float						 _radius;
		Generic::COLOR_BLENDING_MODE _colorBlendingMode;
	};

	class RibbonData
	{
	  public:
		RibbonData();
		Generic::SECONDARY_STRUCTURE_COLOR_MODE _colorMode;
		Generic::COLOR_BLENDING_MODE			_colorBlendingMode;
	};

} // namespace VTX::Model::Representation
#endif
