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
#include "struct_global_uniforms.hpp"
#include "vertex_array.hpp"
#include <util/types.hpp>

namespace VTX::Renderer::GL
{
	class OpenGLRenderer
	{
	  public:
		OpenGLRenderer( void * p_proc, const FilePath & p_shaderPath );
		~OpenGLRenderer() = default;

		void init( const size_t p_width, const size_t p_height );
		void resize( const size_t p_width, const size_t p_height );

		void renderFrame();

		// inline void setOutput( const GLuint p_output ) {}
		//  void updateRenderSetting( const RENDER_SETTING );

		// const Vec2i getPickedIds( const uint p_x, const uint p_y ) const;

	  private:
		size_t _width  = 0;
		size_t _height = 0;

		VertexArray _vaoQuad   = VertexArray();
		Buffer		_vboQuad   = Buffer();
		Buffer		_uboGlobal = Buffer();

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

		std::unique_ptr<ProgramManager> _programManager = nullptr;
	};
} // namespace VTX::Renderer::GL

#endif
