#ifndef __VTX_APP_OLD_RENDER_RENDERER_BASE_RENDERER__
#define __VTX_APP_OLD_RENDER_RENDERER_BASE_RENDERER__

#include "app/old/application/scene.hpp"
#include "app/old/core/model/base_model.hpp"
#include "app/old/render/generic/base_opengl.hpp"
#include "app/old/render/renderer/enum_renderer.hpp"
#include "gl/framebuffer.hpp"
#include <vector>

namespace VTX::App::Old::Render::Renderer
{
	class BaseRenderer : public Render::Generic::BaseOpenGL
	{
	  public:
		BaseRenderer()			= default;
		virtual ~BaseRenderer() = default;

		inline const uint			   getWidth() const { return _width; }
		inline const uint			   getHeight() const { return _height; }
		inline const GL::Framebuffer & getOutputFramebuffer() const { return _outputFramebuffer; }

		virtual void resize( const uint p_width, const uint p_height, const GLuint p_outputFramebufferId )
		{
			_outputFramebuffer.assign( p_outputFramebufferId );
			_width	= p_width;
			_height = p_height;
		}

		virtual void init( const uint, const uint, const GLuint )	= 0;
		virtual void renderFrame( const App::Old::Application::Scene & ) = 0;

		virtual void updateRenderSetting( const RENDER_SETTING ) {}

		virtual const Vec2i getPickedIds( const uint, const uint ) const
		{
			return Vec2i( App::Old::Core::Model::ID_UNKNOWN, App::Old::Core::Model::ID_UNKNOWN );
		}

	  protected:
		uint			_width			   = 0;
		uint			_height			   = 0;
		GL::Framebuffer _outputFramebuffer = GL::Framebuffer();
	};
} // namespace VTX::App::Old::Render::Renderer

#endif
