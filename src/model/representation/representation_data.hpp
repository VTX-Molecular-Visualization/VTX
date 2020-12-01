#ifndef __VTX_MODEL_REPRESENTATION_DATA__
#define __VTX_MODEL_REPRESENTATION_DATA__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Model
	{
		namespace Representation
		{
			class SphereData
			{
			  public:
				SphereData() {};

				float _radiusFixed	 = 1.0f;
				float _radiusAdd	 = 0.f;
				bool  _isRadiusFixed = true;
			};

			class CylinderData
			{
			  public:
				CylinderData() {};
				float _radiusFixed = 1.0f;
			};

			class RibbonData
			{
			  public:
				RibbonData() {};
				float _ribbons = 1.0f;
			};

		} // namespace Representation
	}	  // namespace Model
} // namespace VTX
#endif
