#ifndef __VTX_RENDERER_GL_OPENGL_RENDERER__
#define __VTX_RENDERER_GL_OPENGL_RENDERER__

#include "buffer.hpp"
#include "include_opengl.hpp"
#include "pass/blur.hpp"
#include "pass/fxaa.hpp"
#include "pass/geometric.hpp"
#include "pass/linearize_depth.hpp"
#include "pass/outline.hpp"
#include "pass/selection.hpp"
#include "pass/shading.hpp"
#include "pass/ssao.hpp"
#include "program_manager.hpp"
#include "vertex_array.hpp"
#include <util/types.hpp>

namespace VTX::Renderer::GL
{
	class OpenGLRenderer
	{
	  public:
		OpenGLRenderer() = delete;
		OpenGLRenderer( void * p_proc );
		~OpenGLRenderer() = default;

		inline void setOutput( const GLuint p_output ) {}

		/*
		inline void enableDepthClamp() const { glEnable( GL_DEPTH_CLAMP ); }
		inline void disableDepthClamp() const { glDisable( GL_DEPTH_CLAMP ); }
		inline void enableDepthTest() const { glEnable( GL_DEPTH_TEST ); }
		inline void disableDepthTest() const { glDisable( GL_DEPTH_TEST ); }
		inline void memoryBarrier( const GLbitfield p_barrier ) const { glMemoryBarrier( p_barrier ); }
		inline void flush() const { glFlush(); }
		inline void finish() const { glFinish(); }
		*/

		void init( const size_t p_width, const size_t p_height );
		void resize( const size_t p_width, const size_t p_height );

		void renderFrame();
		// void updateRenderSetting( const RENDER_SETTING );

		// const Vec2i getPickedIds( const uint p_x, const uint p_y ) const;

	  private:
		size_t _width  = 0;
		size_t _height = 0;

		VertexArray _quadVAO = VertexArray();
		Buffer		_quadVBO = Buffer();

		// Framebuffer _output = Framebuffer();

		Pass::Geometric		 _passGeometric		 = Pass::Geometric();
		Pass::LinearizeDepth _passLinearizeDepth = Pass::LinearizeDepth();
		Pass::SSAO			 _passSSAO			 = Pass::SSAO();
		Pass::Blur			 _passBlur			 = Pass::Blur();
		Pass::Shading		 _passShading		 = Pass::Shading();
		Pass::Outline		 _passOutline		 = Pass::Outline();
		Pass::Selection		 _passSelection		 = Pass::Selection();
		Pass::FXAA			 _passFXAA			 = Pass::FXAA();

		std::vector<Pass::BasePass *> _passes = std::vector<Pass::BasePass *>();

		ProgramManager _programManager = ProgramManager();
	};
} // namespace VTX::Renderer::GL

#endif
