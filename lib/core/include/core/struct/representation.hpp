#ifndef __VTX_CORE_STRUCT_REPRESENTATION__
#define __VTX_CORE_STRUCT_REPRESENTATION__

#include <util/types.hpp>

namespace VTX::Core::Struct
{

	// TODO: move default values.
	struct Representation
	{
		// Sphere.
		bool  hasSphere			= true;
		float radiusSphereFixed = 0.4f;
		float radiusSphereAdd	= 0.f;
		bool  radiusFixed		= false;

		// Cylinder.
		bool  hasCylinder			= true;
		float radiusCylinder		= 0.1f;
		bool  cylinderColorBlending = false;

		// Ribbon.
		bool hasRibbon			 = true;
		uint ribbonColorBlending = true;

		// SES.
		bool hasSes = false;
	};
} // namespace VTX::Core::Struct

#endif
