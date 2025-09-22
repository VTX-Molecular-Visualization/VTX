#ifndef __VTX_APP_LIBRARY_PRESET_REPRESENTATION__
#define __VTX_APP_LIBRARY_PRESET_REPRESENTATION__

#include "app/core/library/library.hpp"
#include <core/struct/representation.hpp>
#include <util/math.hpp>

namespace VTX::App::Library::Preset
{

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

	class Representation : public Core::Library::Preset<VTX::Core::Struct::Representation>
	{
	  public:
		Representation();

		void save() override;
		void load() override;

		template<VTX::Core::Struct::E_REPRESENTATION_VALUES S, typename T>
		void setValue( const T p_value )
		{
			using namespace VTX::Core::Struct;
			using namespace Util::Math;

			T value = p_value;

			// Sphere.
			if constexpr ( S == E_REPRESENTATION_VALUES::HAS_SPHERE )
			{
				_data.hasSphere = value;
			}
			else if constexpr ( S == E_REPRESENTATION_VALUES::RADIUS_SPHERE_FIXED )
			{
				value					= clamp( value, RADIUS_SPHERE_FIXED_MIN, RADIUS_SPHERE_FIXED_MAX );
				_data.radiusSphereFixed = value;
			}
			else if constexpr ( S == E_REPRESENTATION_VALUES::RADIUS_SPHERE_ADD )
			{
				value				  = clamp( value, RADIUS_SPHERE_ADD_MIN, RADIUS_SPHERE_ADD_MAX );
				_data.radiusSphereAdd = value;
			}
			else if constexpr ( S == E_REPRESENTATION_VALUES::IS_SPHERE_RADIUS_FIXED )
			{
				_data.radiusFixed = value;
			}
			// Cylinder.
			else if constexpr ( S == E_REPRESENTATION_VALUES::HAS_CYLINDER )
			{
				_data.hasCylinder = value;
			}
			else if constexpr ( S == E_REPRESENTATION_VALUES::RADIUS_CYLINDER )
			{
				value				 = clamp( value, RADIUS_CYLINDER_MIN, RADIUS_CYLINDER_MAX );
				_data.radiusCylinder = value;
			}
			else if constexpr ( S == E_REPRESENTATION_VALUES::CYLINDER_COLOR_BLENDING )
			{
				_data.cylinderColorBlending = value;
			}
			// Ribbon.
			else if constexpr ( S == E_REPRESENTATION_VALUES::HAS_RIBBON )
			{
				_data.hasRibbon = value;
			}
			else if constexpr ( S == E_REPRESENTATION_VALUES::RIBBON_COLOR_BLENDING )
			{
				_data.ribbonColorBlending = value;
			}
			// SES.
			else if constexpr ( S == E_REPRESENTATION_VALUES::HAS_SES )
			{
				_data.hasSes = value;
			}
			else if constexpr ( S == E_REPRESENTATION_VALUES::SES_PROBE_RADIUS )
			{
				value				 = clamp( value, SES_PROBE_RADIUS_MIN, SES_PROBE_RADIUS_MAX );
				_data.sesProbeRadius = value;
			}
			else
			{
				static_assert( std::is_same_v<T, void>, "Unknown representation setting." );
			}

			getCallback<S, T>()( value );
		}
	};
} // namespace VTX::App::Library::Preset

#endif
