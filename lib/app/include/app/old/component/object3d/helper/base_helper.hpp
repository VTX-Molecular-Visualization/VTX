#ifndef __VTX_APP_OLD_COMPONENT_OBJECT3D_HELPER_BASE_HELPER__
#define __VTX_APP_OLD_COMPONENT_OBJECT3D_HELPER_BASE_HELPER__

#include "app/old/component/generic/base_renderable.hpp"
#include "app/old/render/buffer/base_buffer_opengl.hpp"

namespace VTX::App::Old::Component::Object3D::Helper
{
	class BaseHelper : public Component::Generic::BaseRenderable, public App::Old::Render::Buffer::BaseBufferOpenGL
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

} // namespace VTX::App::Old::Component::Object3D::Helper

#endif
