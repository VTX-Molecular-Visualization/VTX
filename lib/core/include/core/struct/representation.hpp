#ifndef __VTX_CORE_STRUCT_REPRESENTATION__
#define __VTX_CORE_STRUCT_REPRESENTATION__

#include <util/types.hpp>

namespace VTX::Core::Struct
{

	// TODO: move default values.
	struct Representation
	{
		// Sphere.
		bool  hasSphere;
		float radiusSphereFixed;
		float radiusSphereAdd;
		bool  radiusFixed;

		// Cylinder.
		bool  hasCylinder;
		float radiusCylinder;
		bool  cylinderColorBlending;

		// Ribbon.
		bool hasRibbon;
		uint ribbonColorBlending;

		// SES.
		bool  hasSes;
		float sesProbeRadius;
		bool  sesIsExterior;
	};
} // namespace VTX::Core::Struct

#endif
