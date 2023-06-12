#ifndef __VTX_RENDERER_GL_OPENGL_RENDERER__
#define __VTX_RENDERER_GL_OPENGL_RENDERER__

#include "buffer.hpp"
#include "enum_shading.hpp"
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
#include "struct_buffer_meshes.hpp"
#include "struct_buffer_molecules.hpp"
#include "struct_global_uniforms.hpp"
#include "struct_proxy_mesh.hpp"
#include "struct_proxy_molecule.hpp"
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

		inline void setNeedUpdate( const bool p_needUpdate ) { _needUpdate = p_needUpdate; }
		const Vec2i getPickedIds( const uint p_x, const uint p_y );

		void addMesh( const StructProxyMesh & );
		void addMolecule( const StructProxyMolecule & );

		void							 setMatrixModelTmp( const Mat4f & );
		void							 setMatrixView( const Mat4f & );
		void							 setMatrixProjection( const Mat4f & );
		void							 setCameraClipInfos( const float, const float );
		inline const Util::Color::Rgba & getColorBackground() const { return _globalUniforms.colorBackground; }
		void							 setColorBackground( Util::Color::Rgba & );
		inline const Util::Color::Rgba & getColorLight() const { return _globalUniforms.colorLight; }
		void							 setColorLight( Util::Color::Rgba & );
		inline const Util::Color::Rgba & getColorFog() const { return _globalUniforms.colorFog; }
		void							 setColorFog( Util::Color::Rgba & );
		inline const Util::Color::Rgba & getColorOutline() const { return _globalUniforms.colorOutline; }
		void							 setColorOutline( Util::Color::Rgba & );
		inline const Util::Color::Rgba & getColorSelection() const { return _globalUniforms.colorSelection; }
		void							 setColorSelection( Util::Color::Rgba & );
		inline float					 getSpecularFactor() const { return _globalUniforms.specularFactor; }
		void							 setSpecularFactor( float );
		inline float					 getFogNear() const { return _globalUniforms.fogNear; }
		void							 setFogNear( float );
		inline float					 getFogFar() const { return _globalUniforms.fogFar; }
		void							 setFogFar( float );
		inline float					 getFogDensity() const { return _globalUniforms.fogDensity; }
		void							 setFogDensity( float );
		inline float					 getSSAOIntensity() const { return _globalUniforms.ssaoIntensity; }
		void							 setSSAOIntensity( float );
		inline float					 getBlurSize() const { return _globalUniforms.blurSize; }
		void							 setBlurSize( float );
		inline float					 getOutlineSensivity() const { return _globalUniforms.outlineSensivity; }
		void							 setOutlineSensivity( float );
		inline float					 getOutlineThickness() const { return _globalUniforms.outlineThickness; }
		void							 setOutlineThickness( float );
		inline ENUM_SHADING				 getShadingMode() const { return _globalUniforms.shadingMode; }
		void							 setShadingMode( ENUM_SHADING & );

	  private:
		size_t _width	   = 0;
		size_t _height	   = 0;
		bool   _needUpdate = true;

		// Quad VAO.
		VertexArray _vao;
		Buffer		_vbo;

		// Uniforms.
		Buffer				 _ubo;
		StructGlobalUniforms _globalUniforms;

		// Pass.
		Pass::PassGeometric		 _passGeometric;
		Pass::PassLinearizeDepth _passLinearizeDepth;
		Pass::PassSSAO			 _passSSAO;
		Pass::PassBlur			 _passBlur;
		Pass::PassShading		 _passShading;
		Pass::PassOutline		 _passOutline;
		Pass::PassSelection		 _passSelection;
		Pass::PassFXAA			 _passFXAA;

		// Input data.
		std::unique_ptr<StructBufferMeshes>	   _bufferMeshes;
		std::unique_ptr<StructBufferMolecules> _bufferMolecules;

		// Program manager.
		std::unique_ptr<ProgramManager> _programManager;

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
