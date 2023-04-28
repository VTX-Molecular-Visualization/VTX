#ifndef __VTX_BASE_RENDERABLE__
#define __VTX_BASE_RENDERABLE__

#include "app/component/render/_fwd.hpp"

namespace VTX::Generic
{
	class BaseRenderable
	{
	  public:
		virtual ~BaseRenderable() = default;

		virtual void render( const App::Component::Render::Camera & ) const = 0;
		virtual void init() {};
	};
} // namespace VTX::Generic
#endif
