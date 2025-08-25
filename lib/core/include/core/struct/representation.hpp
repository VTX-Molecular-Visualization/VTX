#ifndef __VTX_CORE_STRUCT_REPRESENTATION__
#define __VTX_CORE_STRUCT_REPRESENTATION__

#include <util/types.hpp>

namespace VTX::Core::Struct
{
	enum E_REPRESENTATION_VALUES // Not a class to avoid static_cast<int>.
	{
		HAS_SPHERE,
		RADIUS_SPHERE_FIXED,
		RADIUS_SPHERE_ADD,
		IS_SPHERE_RADIUS_FIXED,

		HAS_CYLINDER,
		RADIUS_CYLINDER,
		CYLINDER_COLOR_BLENDING,

		HAS_RIBBON,
		RIBBON_COLOR_BLENDING,

		HAS_SES,
		SES_PROBE_RADIUS
	};

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
	};
} // namespace VTX::Core::Struct

#endif
