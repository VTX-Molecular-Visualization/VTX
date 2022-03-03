#ifndef __VTX_MODEL_REPRESENTATION_PRIMITIVE__
#define __VTX_MODEL_REPRESENTATION_PRIMITIVE__

#include "generic/base_colorable.hpp"

namespace VTX::Model::Representation
{

	class SphereData
	{
	  public:
		SphereData() {}
		float _radiusFixed	 = 1.0f;
		float _radiusAdd	 = 0.f;
		bool  _isRadiusFixed = true;
	};

	class CylinderData
	{
	  public:
		CylinderData() {}
		float						 _radius			= 1.0f;
		Generic::COLOR_BLENDING_MODE _colorBlendingMode = Generic::COLOR_BLENDING_MODE::HARD;
	};

	class RibbonData
	{
	  public:
		RibbonData() {}
		Generic::SECONDARY_STRUCTURE_COLOR_MODE _colorMode		   = Generic::SECONDARY_STRUCTURE_COLOR_MODE::JMOL;
		Generic::COLOR_BLENDING_MODE			_colorBlendingMode = Generic::COLOR_BLENDING_MODE::HARD;
	};

} // namespace VTX::Model::Representation
#endif
