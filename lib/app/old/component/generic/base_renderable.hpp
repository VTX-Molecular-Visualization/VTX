#ifndef __VTX_APP_OLD_COMPONENT_GENERIC_BASE_RENDERABLE__
#define __VTX_APP_OLD_COMPONENT_GENERIC_BASE_RENDERABLE__

#include "app/old/component/render/_fwd.hpp"

namespace VTX::App::Old::Component::Generic
{
	class BaseRenderable
	{
	  public:
		virtual ~BaseRenderable() = default;

		virtual void render( const Component::Render::Camera & ) const = 0;
		virtual void init() {};
	};
} // namespace VTX::App::Old::Component::Generic
#endif
