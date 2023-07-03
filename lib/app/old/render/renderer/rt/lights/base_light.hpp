#ifndef __VTX_APP_OLD_RENDER_RENDERER_RT_BASE_LIGHT__
#define __VTX_APP_OLD_RENDER_RENDERER_RT_BASE_LIGHT__

#include "light_sample.hpp"
#include <util/constants.hpp>

namespace VTX::App::Old::Render::Renderer::RT::Light
{
	class BaseLight
	{
	  public:
		BaseLight( const Util::Color::Rgba & p_color, const float p_power = 1.f ) : _color( p_color ), _power( p_power )
		{
		}
		virtual ~BaseLight() = default;

		const Util::Color::Rgba & getColor() const { return _color; }
		const float				  getPower() const { return _power; }

		virtual LightSample sample( const Vec3f & p_point ) const = 0;

		const bool isSurface() const { return _isSurface; }

	  protected:
		Util::Color::Rgba _color = COLOR_WHITE;
		// TODO: keep it ?
		float _power = 1.f;

		float _pdf = 1.f;

		bool _isSurface = false;
	};
} // namespace VTX::App::Old::Render::Renderer::RT::Light

#endif
