#ifndef __VTX_RENDERER_GL_OPENGL_RENDERER__
#define __VTX_RENDERER_GL_OPENGL_RENDERER__

#include "buffer.hpp"
#include "cubemap.hpp"
#include "enum_bench_item.hpp"
#include "enum_shading.hpp"
#include "include_opengl.hpp"
#include "pass/blur.hpp"
#include "pass/fxaa.hpp"
#include "pass/geometric.hpp"
#include "pass/linearize_depth.hpp"
#include "pass/outline.hpp"
#include "pass/pixelize.hpp"
#include "pass/selection.hpp"
#include "pass/shading.hpp"
#include "pass/ssao.hpp"
#include "program_manager.hpp"
#include "renderer/gl/chrono.hpp"
#include "struct_buffer_meshes.hpp"
#include "struct_buffer_molecules.hpp"
#include "struct_opengl_infos.hpp"
#include "struct_proxy_mesh.hpp"
#include "struct_proxy_molecule.hpp"
#include "vertex_array.hpp"
#include <array>
#include <functional>
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

		inline void setNeedUpdate( const bool p_needUpdate ) { _needUpdate = p_needUpdate; }
		const Vec2i getPickedIds( const uint p_x, const uint p_y );

		// Data.
		void addMesh( const StructProxyMesh & );
		void addMolecule( const StructProxyMolecule & );

		// Active passes.
		inline bool isActiveSSAO() const { return _activeSSAO; }
		void		setActiveSSAO( const bool p_active );
		inline bool isActiveOutline() const { return _activeOutline; }
		void		setActiveOutline( const bool p_active );
		inline bool isActiveFXAA() const { return _activeFXAA; }
		void		setActiveFXAA( const bool p_active );
		inline bool isActivePixelize() const { return _activePixelize; }
		void		setActivePixelize( const bool p_active );

		// Camera uniforms.
		void setMatrixModelTmp( const Mat4f & );
		void setMatrixView( const Mat4f & );
		void setMatrixProjection( const Mat4f & );
		void setCameraClipInfos( const float, const float );

		// Blur uniforms.
		inline float getBlurSize() const { return _passBlur->uniforms.size; }
		void		 setBlurSize( const float p_size );

		// SSAO uniforms.
		inline float getSSAOIntensity() const { return _passSSAO->uniforms.intensity; }
		void		 setSSAOIntensity( const float p_intensity );

		// Shading uniforms.
		inline ENUM_SHADING getShadingMode() const { return ENUM_SHADING( _passShading->uniforms.shadingMode ); }
		void				setShadingMode( const ENUM_SHADING p_shading );
		inline float		getSpecularFactor() const { return _passShading->uniforms.specularFactor; }
		void				setSpecularFactor( const float p_specularFactor );
		inline const Util::Color::Rgba & getColorBackground() const { return _passShading->uniforms.colorBackground; }
		void							 setColorBackground( const Util::Color::Rgba & p_color );
		inline const Util::Color::Rgba & getColorLight() const { return _passShading->uniforms.colorLight; }
		void							 setColorLight( const Util::Color::Rgba & p_color );
		inline const Util::Color::Rgba & getColorFog() const { return _passShading->uniforms.colorFog; }
		void							 setColorFog( const Util::Color::Rgba & p_color );
		inline float					 getFogNear() const { return _passShading->uniforms.fogNear; }
		void							 setFogNear( const float p_near );
		inline float					 getFogFar() const { return _passShading->uniforms.fogFar; }
		void							 setFogFar( const float p_far );
		inline float					 getFogDensity() const { return _passShading->uniforms.fogDensity; }
		void							 setFogDensity( const float p_density );

		// Outline uniforms.
		inline float					 getOutlineSensivity() const { return _passOutline->uniforms.sensivity; }
		void							 setOutlineSensivity( const float p_sensivity );
		inline uint						 getOutlineThickness() const { return _passOutline->uniforms.thickness; }
		void							 setOutlineThickness( const uint p_thickness );
		inline const Util::Color::Rgba & getColorOutline() const { return _passOutline->uniforms.color; }
		void							 setColorOutline( const Util::Color::Rgba & p_color );

		// Selection uniforms.
		inline const Util::Color::Rgba & getColorSelection() const { return _passSelection->uniforms.color; }
		void							 setColorSelection( const Util::Color::Rgba & p_color );

		// Pixelize uniforms.
		inline uint getPixelSize() const { return _passPixelize->uniforms.size; }
		void		setPixelSize( const uint p_size );
		inline bool isPixelizeBackground() const { return _passPixelize->uniforms.background; }
		void		setPixelizeBackground( const bool p_background );

		// Times.
		inline bool isTimersEnabled() const { return _enableTimers; }
		void		setTimersEnabled( const bool p_enable ) { _enableTimers = p_enable; }
		inline std::array<float, ENUM_TIME_ITEM::COUNT> & getTimes() { return _times; }

		// Opengl infos.
		inline const StructOpenglInfos & getOpenglInfos() const { return _openglInfos; }

		// Shaders hot reload.
		inline void compileShaders()
		{
			_programManager->compileShaders();
			_passSSAO->refreshKernel();
		}

		void loadSkybox( const std::array<unsigned char *, 6> & p_textures,
						 const size_t							p_width,
						 const size_t							p_height );

		inline static const bool ACTIVE_SSAO_DEFAULT	 = true;
		inline static const bool ACTIVE_OUTLINE_DEFAULT	 = false;
		inline static const bool ACTIVE_FXAA_DEFAULT	 = true;
		inline static const bool ACTIVE_PIXELIZE_DEFAULT = false;

	  private:
		size_t _width	   = 0;
		size_t _height	   = 0;
		bool   _needUpdate = true;

		// Quad VAO.
		std::unique_ptr<VertexArray> _vao;
		std::unique_ptr<Buffer>		 _vbo;

		// Uniforms.
		std::unique_ptr<Buffer> _uboCamera;
		std::unique_ptr<Buffer> _uboModels;

		struct StructUniformsCamera
		{
			Mat4f matrixModel  = MAT4F_ID;
			Mat4f matrixNormal = MAT4F_ID;
			Mat4f matrixView;
			Mat4f matrixProjection;
			// _near * _far, _far, _far - _near, _near
			Vec4f cameraClipInfos;
			bool  isCameraPerspective = true;
		} _uniformsCamera;

		struct StructUniformsModels
		{
			Mat4f matrixModel  = MAT4F_ID;
			Mat4f matrixNormal = MAT4F_ID;
		} _uniformsModels;

		// Pass.
		std::unique_ptr<Pass::PassGeometric>	  _passGeometric;
		std::unique_ptr<Pass::PassLinearizeDepth> _passLinearizeDepth;
		std::unique_ptr<Pass::PassSSAO>			  _passSSAO;
		std::unique_ptr<Pass::PassBlur>			  _passBlur;
		std::unique_ptr<Pass::PassShading>		  _passShading;
		std::unique_ptr<Pass::PassOutline>		  _passOutline;
		std::unique_ptr<Pass::PassSelection>	  _passSelection;
		std::unique_ptr<Pass::PassFXAA>			  _passFXAA;
		std::unique_ptr<Pass::PassPixelize>		  _passPixelize;

		bool _activeSSAO	 = ACTIVE_SSAO_DEFAULT;
		bool _activeOutline	 = ACTIVE_OUTLINE_DEFAULT;
		bool _activeFXAA	 = ACTIVE_FXAA_DEFAULT;
		bool _activePixelize = ACTIVE_PIXELIZE_DEFAULT;

		// Output.
		GLuint _fboOutputId = 0;

		// Input data.
		std::unique_ptr<StructBufferMeshes>	   _bufferMeshes;
		std::unique_ptr<StructBufferMolecules> _bufferMolecules;

		// Program manager.
		std::unique_ptr<ProgramManager> _programManager;

		// Skybox.
		std::unique_ptr<Cubemap> _skybox;

		// Bench.
		bool									 _enableTimers = true;
		std::array<float, ENUM_TIME_ITEM::COUNT> _times;

		using Task										= std::function<void()>;
		std::function<float( const Task & )> _funChrono = [ this ]( const Task & p_task )
		{
			if ( _enableTimers )
			{
				Chrono chrono;
				chrono.start();
				p_task();
				return float( chrono.stop() );
			}
			else
			{
				p_task();
				return 0.f;
			}
		};

		// Specs.
		StructOpenglInfos _openglInfos;

		void _getOpenglInfos();
		void _setupRouting();
#if ( VTX_OPENGL_VERSION == 450 )
		static void APIENTRY _debugMessageCallback( const GLenum   p_source,
													const GLenum   p_type,
													const GLuint   p_id,
													const GLenum   p_severity,
													const GLsizei  p_length,
													const GLchar * p_msg,
													const void *   p_data );
#endif
	};
} // namespace VTX::Renderer::GL

#endif
