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
		OpenGLRenderer( void * p_proc, const size_t p_width, const size_t p_height, const FilePath & p_shaderPath );
		~OpenGLRenderer() = default;

		void resize( const size_t p_width, const size_t p_height );

		void renderFrame();

		const Vec2i getPickedIds( const uint p_x, const uint p_y );

		void setCameraMatrix( const Mat4f & p_view, const Mat4f & p_proj );
		void setBackgroundColor( Util::Color::Rgba & );

	  private:
		size_t _width  = 0;
		size_t _height = 0;

		VertexArray _vaoQuad;
		Buffer		_vboQuad;

		// TEST.
		Buffer				 _ubo;
		StructGlobalUniforms _globalUniforms;

		Pass::Geometric		 _passGeometric;
		Pass::LinearizeDepth _passLinearizeDepth;
		Pass::SSAO			 _passSSAO;
		Pass::Blur			 _passBlur;
		Pass::Shading		 _passShading;
		Pass::Outline		 _passOutline;
		Pass::Selection		 _passSelection;
		Pass::FXAA			 _passFXAA;

		std::vector<Pass::BasePass *> _passes;

		ProgramManager _programManager;

		static void APIENTRY _debugMessageCallback( const GLenum   p_source,
													const GLenum   p_type,
													const GLuint   p_id,
													const GLenum   p_severity,
													const GLsizei  p_length,
													const GLchar * p_msg,
													const void *   p_data );
	};
} // namespace VTX::Renderer::GL

#endif
