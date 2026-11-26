#ifndef VTX_RENDERER_REPRESENTATION_HPP
#define VTX_RENDERER_REPRESENTATION_HPP

#include <util/enum.hpp>
#include <util/types.hpp>

namespace VTX::Renderer
{
	enum struct E_REPRESENTATION_VALUES : uint8_t
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
		bool  hasSphere			  = HAS_SPHERE_DEFAULT;
		float radiusSphereFixed	  = RADIUS_SPHERE_FIXED_DEFAULT;
		float radiusSphereAdd	  = RADIUS_SPHERE_ADD_DEFAULT;
		bool  isRadiusSphereFixed = IS_SPHERE_RADIUS_FIXED_DEFAULT;

		// Cylinder.
		bool  hasCylinder			= HAS_CYLINDER_DEFAULT;
		float radiusCylinder		= RADIUS_CYLINDER_DEFAULT;
		bool  cylinderColorBlending = CYLINDER_COLOR_BLENDING_DEFAULT;

		// Ribbon.
		bool hasRibbon			 = HAS_RIBBON_DEFAULT;
		bool ribbonColorBlending = RIBBON_COLOR_BLENDING_DEFAULT;

		// SES.
		bool			   hasSes		  = HAS_SES_DEFAULT;
		E_SES_COMPUTE_MODE sesComputeMode = SES_COMPUTE_MODE_DEFAULT;
		float			   sesProbeRadius = SES_PROBE_RADIUS_DEFAULT;
	};
} // namespace VTX::Renderer

#endif
