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

		const Vec2i getPickedIds( const uint p_x, const uint p_y );

		void setMatrixModelTmp( const Mat4f & );
		void setMatrixView( const Mat4f & );
		void setMatrixProjection( const Mat4f & );
		void setBackgroundColor( Util::Color::Rgba & );

		inline void setNeedUpdate( const bool p_needUpdate ) { _needUpdate = p_needUpdate; }

	  private:
		size_t _width  = 0;
		size_t _height = 0;

		bool _needUpdate = true;

		VertexArray _vao;
		Buffer		_vbo;

		// TEST.
		Buffer				 _ubo;
		StructGlobalUniforms _globalUniforms;

		Pass::PassGeometric		 _passGeometric;
		Pass::PassLinearizeDepth _passLinearizeDepth;
		Pass::PassSSAO			 _passSSAO;
		Pass::PassBlur			 _passBlur;
		Pass::PassShading		 _passShading;
		Pass::PassOutline		 _passOutline;
		Pass::PassSelection		 _passSelection;
		Pass::PassFXAA			 _passFXAA;

		std::unique_ptr<ProgramManager> _programManager = nullptr;

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
