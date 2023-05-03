#ifndef __VTX_APP_COMPONENT_OBJECT3D_HELPER_BASE_HELPER__
#define __VTX_APP_COMPONENT_OBJECT3D_HELPER_BASE_HELPER__

#include "app/old_app/generic/base_renderable.hpp"
#include "app/render/buffer/base_buffer_opengl.hpp"

namespace VTX::App::Component::Object3D::Helper
{
	class BaseHelper : public Generic::BaseRenderable, public App::Render::Buffer::BaseBufferOpenGL
	{
	  public:
		BaseHelper()		  = default;
		virtual ~BaseHelper() = default;

		// BaseRenderable.
		virtual void render( const Component::Render::Camera & ) const override {}

		virtual void refresh() {}

	  private:
		// BaseBufferOpenGL.
		virtual void _generate() override {}
	};

} // namespace VTX::App::Component::Object3D::Helper

#endif
