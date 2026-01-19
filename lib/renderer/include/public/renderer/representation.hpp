#ifndef __VTX_RENDERER_REPRESENTATION__
#define __VTX_RENDERER_REPRESENTATION__

#include <util/types.hpp>

namespace VTX::Renderer
{
	enum struct E_REPRESENTATION_VALUES
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

	// Default values.
	constexpr bool	HAS_SPHERE_DEFAULT			   = true;
	constexpr float RADIUS_SPHERE_FIXED_DEFAULT	   = 0.5f;
	constexpr float RADIUS_SPHERE_FIXED_MIN		   = 0.01f;
	constexpr float RADIUS_SPHERE_FIXED_MAX		   = 3.f;
	constexpr float RADIUS_SPHERE_ADD_DEFAULT	   = 0.f;
	constexpr float RADIUS_SPHERE_ADD_MIN		   = 0.f;
	constexpr float RADIUS_SPHERE_ADD_MAX		   = 3.f;
	constexpr bool	IS_SPHERE_RADIUS_FIXED_DEFAULT = true;

	constexpr bool	HAS_CYLINDER_DEFAULT			= true;
	constexpr float RADIUS_CYLINDER_DEFAULT			= 0.1f;
	constexpr float RADIUS_CYLINDER_MIN				= 0.01f;
	constexpr float RADIUS_CYLINDER_MAX				= 3.f;
	constexpr bool	CYLINDER_COLOR_BLENDING_DEFAULT = false;

	constexpr bool HAS_RIBBON_DEFAULT			 = true;
	constexpr bool RIBBON_COLOR_BLENDING_DEFAULT = true;

	constexpr bool	HAS_SES_DEFAULT			 = false;
	constexpr float SES_PROBE_RADIUS_DEFAULT = 1.4f;
	constexpr float SES_PROBE_RADIUS_MIN	 = 0.2f;
	constexpr float SES_PROBE_RADIUS_MAX	 = 3.f;

	struct Representation
	{
		// Sphere.
		bool  hasSphere;
		float radiusSphereFixed;
		float radiusSphereAdd;
		bool  isRadiusSphereFixed;

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

	using RepresentationIndex = uchar;

	namespace Representations
	{
		inline const Representation STICKS { // Sphere.
											 false,
											 RADIUS_SPHERE_FIXED_DEFAULT,
											 RADIUS_SPHERE_ADD_DEFAULT,
											 true,
											 // Cylinder.
											 true,
											 0.15f,
											 false,
											 // Ribbon.
											 false,
											 false,
											 // SES.
											 false,
											 SES_PROBE_RADIUS_DEFAULT
		};

		inline const Representation BALLS_AND_STICKS { // Sphere.
													   true,
													   0.4f,
													   RADIUS_SPHERE_ADD_DEFAULT,
													   true,
													   // Cylinder.
													   true,
													   0.15f,
													   false,
													   // Ribbon.
													   false,
													   false,
													   // SES.
													   false,
													   SES_PROBE_RADIUS_DEFAULT
		};

		inline const Representation VAN_DER_WAALS { // Sphere.
													true,
													RADIUS_SPHERE_FIXED_DEFAULT,
													RADIUS_SPHERE_ADD_DEFAULT,
													false,
													// Cylinder.
													false,
													RADIUS_CYLINDER_DEFAULT,
													false,
													// Ribbon.
													false,
													false,
													// SES.
													false,
													SES_PROBE_RADIUS_DEFAULT
		};

		inline const Representation RIBBONS { // Sphere.
											  false,
											  RADIUS_SPHERE_FIXED_DEFAULT,
											  RADIUS_SPHERE_ADD_DEFAULT,
											  true,
											  // Cylinder.
											  false,
											  RADIUS_CYLINDER_DEFAULT,
											  false,
											  // Ribbon.
											  true,
											  true,
											  // SES.
											  false,
											  SES_PROBE_RADIUS_DEFAULT
		};

		inline const Representation SES { // Sphere.
										  false,
										  RADIUS_SPHERE_FIXED_DEFAULT,
										  RADIUS_SPHERE_ADD_DEFAULT,
										  false,
										  // Cylinder.
										  false,
										  RADIUS_CYLINDER_DEFAULT,
										  false,
										  // Ribbon.
										  false,
										  false,
										  // SES.
										  true,
										  SES_PROBE_RADIUS_DEFAULT
		};
	} // namespace Representations
} // namespace VTX::Renderer

#endif
