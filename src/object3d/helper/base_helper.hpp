#ifndef __VTX_BASE_OBJECT_3D_HELPER__
#define __VTX_BASE_OBJECT_3D_HELPER__

#include "buffer/base_buffer_opengl.hpp"
#include "generic/base_renderable.hpp"

namespace VTX::Object3D::Helper
{
	class BaseHelper : public Generic::BaseRenderable, public Buffer::BaseBufferOpenGL
	{
	  public:
		BaseHelper()		  = default;
		virtual ~BaseHelper() = default;

		// BaseRenderable.
		virtual void render( const Object3D::Camera & ) const override {}
		// BaseBufferOpenGL.
		virtual void generate() override {}
	};

} // namespace VTX::Object3D::Helper

#endif
