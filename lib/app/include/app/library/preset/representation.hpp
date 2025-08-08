#ifndef __VTX_APP_LIBRARY_PRESET_REPRESENTATION__
#define __VTX_APP_LIBRARY_PRESET_REPRESENTATION__

#include "app/core/library/library.hpp"
#include <core/struct/representation.hpp>
#include <renderer/proxy/representation.hpp>

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
		Representation()
		{
			// Set default values.
			_data.hasSphere				= HAS_SPHERE_DEFAULT;
			_data.radiusSphereFixed		= RADIUS_SPHERE_FIXED_DEFAULT;
			_data.radiusSphereAdd		= RADIUS_SPHERE_ADD_DEFAULT;
			_data.radiusFixed			= IS_SPHERE_RADIUS_FIXED_DEFAULT;
			_data.hasCylinder			= HAS_CYLINDER_DEFAULT;
			_data.radiusCylinder		= RADIUS_CYLINDER_DEFAULT;
			_data.cylinderColorBlending = CYLINDER_COLOR_BLENDING_DEFAULT;
			_data.hasRibbon				= HAS_RIBBON_DEFAULT;
			_data.ribbonColorBlending	= RIBBON_COLOR_BLENDING_DEFAULT;
			_data.hasSes				= HAS_SES_DEFAULT;
			_data.sesProbeRadius		= SES_PROBE_RADIUS_DEFAULT;
		}

		void save() override {}
		void load() override {}

		template<Renderer::Proxy::E_REPRESENTATION_SETTINGS S, typename T>
		void setValue( const T p_value )
		{
			using namespace Renderer::Proxy;

			// TODO: check min max values.

			// Sphere.
			if constexpr ( S == E_REPRESENTATION_SETTINGS::HAS_SPHERE )
			{
				_data.hasSphere = p_value;
			}
			else if constexpr ( S == E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_FIXED )
			{
				_data.radiusSphereFixed = p_value;
			}
			else if constexpr ( S == E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_ADD )
			{
				_data.radiusSphereAdd = p_value;
			}
			else if constexpr ( S == E_REPRESENTATION_SETTINGS::IS_SPHERE_RADIUS_FIXED )
			{
				_data.radiusFixed = p_value;
			}
			// Cylinder.
			else if constexpr ( S == E_REPRESENTATION_SETTINGS::HAS_CYLINDER )
			{
				_data.hasCylinder = p_value;
			}
			else if constexpr ( S == E_REPRESENTATION_SETTINGS::RADIUS_CYLINDER )
			{
				_data.radiusCylinder = p_value;
			}
			else if constexpr ( S == E_REPRESENTATION_SETTINGS::CYLINDER_COLOR_BLENDING )
			{
				_data.cylinderColorBlending = p_value;
			}
			// Ribbon.
			else if constexpr ( S == E_REPRESENTATION_SETTINGS::HAS_RIBBON )
			{
				_data.hasRibbon = p_value;
			}
			else if constexpr ( S == E_REPRESENTATION_SETTINGS::RIBBON_COLOR_BLENDING )
			{
				_data.ribbonColorBlending = p_value;
			}
			// SES.
			else if constexpr ( S == E_REPRESENTATION_SETTINGS::HAS_SES )
			{
				_data.hasSes = p_value;
			}
			else if constexpr ( S == E_REPRESENTATION_SETTINGS::SES_PROBE_RADIUS )
			{
				_data.sesProbeRadius = p_value;
			}
			else
			{
				static_assert( std::is_same_v<T, void>, "Unknown representation setting." );
			}

			// callback<S, T>()( p_value );
		}
	};
} // namespace VTX::App::Library::Preset

#endif
