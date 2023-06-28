#ifndef __VTX_APP_OLD_COMPONENT_GENERIC_BASE_COLORABLE__
#define __VTX_APP_OLD_COMPONENT_GENERIC_BASE_COLORABLE__

#include <util/color/rgba.hpp>

namespace VTX::App::Old::Component::Generic
{
	class BaseColorable
	{
	  public:
		inline const Util::Color::Rgba & getColor() const { return _color; }
		inline void						 setColor( const Util::Color::Rgba & p_color ) { _color = p_color; }

	  protected:
		Util::Color::Rgba _color = Util::Color::Rgba::random();
	};
} // namespace VTX::App::Old::Component::Generic
#endif
