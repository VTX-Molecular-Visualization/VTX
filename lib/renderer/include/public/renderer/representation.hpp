#ifndef __VTX_RENDERER_REPRESENTATION__
#define __VTX_RENDERER_REPRESENTATION__

#include <util/enum.hpp>
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
		SES_PROBE_RADIUS,
		SES_COMPUTE_MODE
	};

	enum struct E_SES_COMPUTE_MODE : uint8_t
	{
		VTX_ENUM_ENABLE_BITMASK,
		NONE		 = 0,
		POLYMER		 = 1u << 0,
		CARBOHYDRATE = 1u << 1,
		LIGAND		 = 1u << 2,
		//  ION, SOLVENT, WATER, UNKNOWN.
		OTHERS = 1u << 3,
		//  Mixed means selected categories will build only one surface.
		//  Otherwise, each category will build its own surface.
		MIXED = 1u << 4,
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

	constexpr bool				 HAS_SES_DEFAULT = false;
	constexpr E_SES_COMPUTE_MODE SES_COMPUTE_MODE_DEFAULT
		= E_SES_COMPUTE_MODE::POLYMER | E_SES_COMPUTE_MODE::CARBOHYDRATE;
	constexpr float SES_PROBE_RADIUS_DEFAULT = 1.4f;
	constexpr float SES_PROBE_RADIUS_MIN	 = 0.f;
	constexpr float SES_PROBE_RADIUS_MAX	 = 2.5f;

	using RepresentationIndex = uint8_t;

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
		bool			   hasSes;
		E_SES_COMPUTE_MODE sesComputeMode;
		float			   sesProbeRadius;
	};

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
											 SES_COMPUTE_MODE_DEFAULT,
											 SES_PROBE_RADIUS_DEFAULT
		};

		inline const Representation STICKS_AND_RIBBONS { // Sphere.
														 false,
														 RADIUS_SPHERE_FIXED_DEFAULT,
														 RADIUS_SPHERE_ADD_DEFAULT,
														 true,
														 // Cylinder.
														 true,
														 0.15f,
														 false,
														 // Ribbon.
														 true,
														 true,
														 // SES.
														 false,
														 SES_COMPUTE_MODE_DEFAULT,
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
													   SES_COMPUTE_MODE_DEFAULT,
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
													SES_COMPUTE_MODE_DEFAULT,
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
											  SES_COMPUTE_MODE_DEFAULT,
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
										  SES_COMPUTE_MODE_DEFAULT,
										  SES_PROBE_RADIUS_DEFAULT
		};
	} // namespace Representations
} // namespace VTX::Renderer

#endif
