#include "renderer/gl/opengl_renderer.hpp"
#include <util/exceptions.hpp>
#include <util/logger.hpp>

namespace VTX::Renderer::GL
{
	OpenGLRenderer::OpenGLRenderer( void *			 p_proc,
									const size_t	 p_width,
									const size_t	 p_height,
									const FilePath & p_shaderPath ) :
		_width( p_width ),
		_height( p_height )
	{
		VTX_INFO( "Creating renderer..." );

		// Load OpenGL.
		if ( gladLoadGLLoader( (GLADloadproc)p_proc ) == 0 )
		{
			throw GLException( "Failed to initialize GLAD" );
		}

		_getOpenglInfos();

		VTX_INFO( "Device: {} {}", _openglInfos.glVendor, _openglInfos.glRenderer );
		VTX_INFO( "OpenGL initialized: {}.{}", GLVersion.major, GLVersion.minor );

#if ( VTX_OPENGL_VERSION == 450 )
		// Debug infos.
		glEnable( GL_DEBUG_OUTPUT );
		glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
		glDebugMessageCallback( _debugMessageCallback, nullptr );
#endif

		// Program manager.
		_programManager = std::make_unique<ProgramManager>( p_shaderPath );

		// Input buffer data.
		_bufferMeshes	 = std::make_unique<StructBufferMeshes>();
		_bufferMolecules = std::make_unique<StructBufferMolecules>();

		// Passes.
		_passGeometric		= std::make_unique<Pass::PassGeometric>( p_width, p_height, *_programManager );
		_passLinearizeDepth = std::make_unique<Pass::PassLinearizeDepth>( p_width, p_height, *_programManager );
		_passSSAO			= std::make_unique<Pass::PassSSAO>( p_width, p_height, *_programManager );
		_passBlur			= std::make_unique<Pass::PassBlur>( p_width, p_height, *_programManager );
		_passShading		= std::make_unique<Pass::PassShading>( p_width, p_height, *_programManager );
		_passOutline		= std::make_unique<Pass::PassOutline>( p_width, p_height, *_programManager );
		_passSelection		= std::make_unique<Pass::PassSelection>( p_width, p_height, *_programManager );
		_passFXAA			= std::make_unique<Pass::PassFXAA>( p_width, p_height, *_programManager );

		// Setup default routing.
		_setupRouting();

		// Init quad vao/vbo for deferred shading.
		std::vector<Vec2f> quad = { Vec2f( -1.f, 1.f ), Vec2f( -1.f, -1.f ), Vec2f( 1.f, 1.f ), Vec2f( 1.f, -1.f ) };

		_vbo = std::make_unique<Buffer>();
		_vao = std::make_unique<VertexArray>();
		_ubo = std::make_unique<Buffer>();

		_vao->enableAttribute( 0 );
		_vao->setVertexBuffer<float>( 0, *_vbo, sizeof( Vec2f ) );
		_vao->setAttributeFormat<float>( 0, 2 );
		_vao->setAttributeBinding( 0, 0 );

		_vbo->set( quad );

		// Global uniforms buffer.
		_ubo->set( _globalUniforms, GL_DYNAMIC_DRAW );

		glViewport( 0, 0, GLsizei( _width ), GLsizei( _height ) );

		VTX_INFO( "Renderer initialized" );
	}

	void OpenGLRenderer::resize( const size_t p_width, const size_t p_height )
	{
		_width	= p_width;
		_height = p_height;

		_passGeometric->resize( _width, _height );
		_passLinearizeDepth->resize( _width, _height );
		_passSSAO->resize( _width, _height );
		_passBlur->resize( _width, _height );
		_passShading->resize( _width, _height );
		_passOutline->resize( _width, _height );
		_passSelection->resize( _width, _height );
		_passFXAA->resize( _width, _height );

		glViewport( 0, 0, GLsizei( _width ), GLsizei( _height ) );
	}

	void OpenGLRenderer::renderFrame()
	{
		if ( _needUpdate )
		{
			//_vao.drawCalls = 0;

			_ubo->bind( GL_UNIFORM_BUFFER, 15 );

			_times.fill( 0.f );
			_times[ ENUM_TIME_ITEM::GEOMETRIC ]		  = _funChrono( [ & ]() { _passGeometric->render( *_vao ); } );
			_times[ ENUM_TIME_ITEM::LINEARIZE_DEPTH ] = _funChrono( [ & ]() { _passLinearizeDepth->render( *_vao ); } );
			if ( _activeSSAO )
			{
				_times[ ENUM_TIME_ITEM::SSAO ] = _funChrono( [ & ]() { _passSSAO->render( *_vao ); } );
				_times[ ENUM_TIME_ITEM::BLUR ] = _funChrono( [ & ]() { _passBlur->render( *_vao ); } );
			}
			_times[ ENUM_TIME_ITEM::SHADING ] = _funChrono( [ & ]() { _passShading->render( *_vao ); } );
			if ( _activeOutline )
			{
				_times[ ENUM_TIME_ITEM::OUTLINE ] = _funChrono( [ & ]() { _passOutline->render( *_vao ); } );
			}
			_times[ ENUM_TIME_ITEM::SELECTION ] = _funChrono( [ & ]() { _passSelection->render( *_vao ); } );
			_times[ ENUM_TIME_ITEM::FXAA ]		= _funChrono( [ & ]() { _passFXAA->render( *_vao ); } );

			// Copy to output (temp).

			_times[ ENUM_TIME_ITEM::BLIT ] = _funChrono(
				[ & ]()
				{
					glBindFramebuffer( GL_READ_FRAMEBUFFER,
									   _activeFXAA ? _passFXAA->out.fbo->getId() : _passSelection->out.fbo->getId() );
					glBindFramebuffer( GL_DRAW_FRAMEBUFFER, _fboOutputId );
					glBlitFramebuffer( 0,
									   0,
									   GLint( _width ),
									   GLint( _height ),
									   0,
									   0,
									   GLint( _width ),
									   GLint( _height ),
									   GL_COLOR_BUFFER_BIT,
									   GL_LINEAR );
					glBindFramebuffer( GL_FRAMEBUFFER, 0 );
				} );

#if ( GL_NVX_gpu_memory_info == 1 )
			glGetIntegerv( GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX,
						   &_openglInfos.gpuMemoryInfoCurrentAvailableVidMemNVX );
#endif

			_ubo->unbind();

			//_needUpdate = false;
		}
	}

	const Vec2i OpenGLRenderer::getPickedIds( const uint p_x, const uint p_y )
	{
		return _passGeometric->getPickedData( p_x, p_y );
	}

	void OpenGLRenderer::addMesh( const StructProxyMesh & p_proxy )
	{
		// TODO: handle multiple meshes.
		_bufferMeshes->vboPositions.set( *p_proxy.vertices );
		_bufferMeshes->vboNormals.set( *p_proxy.normals );
		_bufferMeshes->vboColors.set( *p_proxy.colors );
		_bufferMeshes->vboVisibilities.set( *p_proxy.visibilities );
		_bufferMeshes->vboSelections.set( *p_proxy.selections );
		_bufferMeshes->vboIds.set( *p_proxy.ids );
		_bufferMeshes->ebo.set( *p_proxy.indices );
		_bufferMeshes->size = p_proxy.indices->size();
	}

	void OpenGLRenderer::addMolecule( const StructProxyMolecule & p_proxy )
	{
		// TODO: handle multiple molecules.
		_bufferMolecules->vboPositions.set( *p_proxy.atomPositions );
		_bufferMolecules->vboColors.set( *p_proxy.atomColors );
		_bufferMolecules->vboRadii.set( *p_proxy.atomRadii );
		_bufferMolecules->vboVisibilities.set( *p_proxy.atomVisibilities );
		_bufferMolecules->vboSelections.set( *p_proxy.atomSelections );
		_bufferMolecules->vboIds.set( *p_proxy.atomIds );
		if ( p_proxy.bonds->size() > 0 )
		{
			_bufferMolecules->eboBonds.set( *p_proxy.bonds );
		}
		_bufferMolecules->sizeAtoms = p_proxy.atomPositions->size();
		_bufferMolecules->sizeBonds = p_proxy.bonds->size();
	}

	void OpenGLRenderer::setActiveSSAO( const bool p_active )
	{
		_activeSSAO = p_active;
		_passBlur->clearTexture();
	}

	void OpenGLRenderer::setActiveOutline( const bool p_active )
	{
		_activeOutline = p_active;
		_setupRouting();
	}

	void OpenGLRenderer::setActiveFXAA( const bool p_active )
	{
		_activeFXAA = p_active;
		_setupRouting();
	}

	void OpenGLRenderer::setMatrixModelTmp( const Mat4f & p_model )
	{
		_ubo->setSub( p_model, offsetof( StructGlobalUniforms, matrixModel ), sizeof( Mat4f ) );
		_ubo->setSub( Util::Math::transpose( Util::Math::inverse( p_model ) ),
					  offsetof( StructGlobalUniforms, matrixNormal ),
					  sizeof( Mat4f ) );
	}

	void OpenGLRenderer::setMatrixView( const Mat4f & p_view )
	{
		_globalUniforms.matrixView = p_view;
		_ubo->setSub( p_view, offsetof( StructGlobalUniforms, matrixView ), sizeof( Mat4f ) );
	}

	void OpenGLRenderer::setMatrixProjection( const Mat4f & p_proj )
	{
		_globalUniforms.matrixProjection = p_proj;
		_ubo->setSub( p_proj, offsetof( StructGlobalUniforms, matrixProjection ), sizeof( Mat4f ) );
	}

	void OpenGLRenderer::setCameraClipInfos( const float p_near, const float p_far )
	{
		_globalUniforms.cameraClipInfos = Vec4f( p_near * p_far, p_far, p_far - p_near, p_near );
		_ubo->setSub(
			_globalUniforms.cameraClipInfos, offsetof( StructGlobalUniforms, cameraClipInfos ), sizeof( Vec4f ) );
	}

	void OpenGLRenderer::setColorBackground( Util::Color::Rgba & p_color )
	{
		_globalUniforms.colorBackground = p_color;
		_ubo->setSub( p_color, offsetof( StructGlobalUniforms, colorBackground ), sizeof( Util::Color::Rgba ) );
	}

	void OpenGLRenderer::setColorLight( Util::Color::Rgba & p_color )
	{
		_globalUniforms.colorLight = p_color;
		_ubo->setSub( p_color, offsetof( StructGlobalUniforms, colorLight ), sizeof( Util::Color::Rgba ) );
	}

	void OpenGLRenderer::setColorFog( Util::Color::Rgba & p_color )
	{
		_globalUniforms.colorFog = p_color;
		_ubo->setSub( p_color, offsetof( StructGlobalUniforms, colorFog ), sizeof( Util::Color::Rgba ) );
	}

	void OpenGLRenderer::setColorOutline( Util::Color::Rgba & p_color )
	{
		_globalUniforms.colorOutline = p_color;
		_ubo->setSub( p_color, offsetof( StructGlobalUniforms, colorOutline ), sizeof( Util::Color::Rgba ) );
	}

	void OpenGLRenderer::setColorSelection( Util::Color::Rgba & p_color )
	{
		_globalUniforms.colorSelection = p_color;
		_ubo->setSub( p_color, offsetof( StructGlobalUniforms, colorSelection ), sizeof( Util::Color::Rgba ) );
	}

	void OpenGLRenderer::setSpecularFactor( float p_factor )
	{
		_globalUniforms.specularFactor = p_factor;
		_ubo->setSub( p_factor, offsetof( StructGlobalUniforms, specularFactor ), sizeof( float ) );
	}

	void OpenGLRenderer::setFogNear( float p_near )
	{
		_globalUniforms.fogNear = p_near;
		_ubo->setSub( p_near, offsetof( StructGlobalUniforms, fogNear ), sizeof( float ) );
	}

	void OpenGLRenderer::setFogFar( float p_far )
	{
		_globalUniforms.fogFar = p_far;
		_ubo->setSub( p_far, offsetof( StructGlobalUniforms, fogFar ), sizeof( float ) );
	}

	void OpenGLRenderer::setFogDensity( float p_density )
	{
		_globalUniforms.fogDensity = p_density;
		_ubo->setSub( p_density, offsetof( StructGlobalUniforms, fogDensity ), sizeof( float ) );
	}

	void OpenGLRenderer::setSSAOIntensity( float p_intensity )
	{
		_globalUniforms.ssaoIntensity = p_intensity;
		_ubo->setSub( p_intensity, offsetof( StructGlobalUniforms, ssaoIntensity ), sizeof( float ) );
	}

	void OpenGLRenderer::setBlurSize( float p_size )
	{
		_globalUniforms.blurSize = p_size;
		_ubo->setSub( p_size, offsetof( StructGlobalUniforms, blurSize ), sizeof( float ) );
	}

	void OpenGLRenderer::setOutlineSensivity( float p_sensivity )
	{
		_globalUniforms.outlineSensivity = p_sensivity;
		_ubo->setSub( p_sensivity, offsetof( StructGlobalUniforms, outlineSensivity ), sizeof( float ) );
	}

	void OpenGLRenderer::setOutlineThickness( float p_thickness )
	{
		_globalUniforms.outlineThickness = p_thickness;
		_ubo->setSub( p_thickness, offsetof( StructGlobalUniforms, outlineThickness ), sizeof( float ) );
	}

	void OpenGLRenderer::setShadingMode( ENUM_SHADING & p_shading )
	{
		_globalUniforms.shadingMode = p_shading;
		_ubo->setSub( p_shading, offsetof( StructGlobalUniforms, shadingMode ), sizeof( ENUM_SHADING ) );
	}

	void OpenGLRenderer::_getOpenglInfos()
	{
		_openglInfos.glVendor	 = std::string( (const char *)glGetString( GL_VENDOR ) );
		_openglInfos.glRenderer	 = std::string( (const char *)glGetString( GL_RENDERER ) );
		_openglInfos.glVersion	 = std::string( (const char *)glGetString( GL_VERSION ) );
		_openglInfos.glslVersion = std::string( (const char *)glGetString( GL_SHADING_LANGUAGE_VERSION ) );

		glGetIntegerv( GL_MAX_TEXTURE_SIZE, &_openglInfos.glMaxTextureSize );
		glGetIntegerv( GL_MAX_PATCH_VERTICES, &_openglInfos.glMaxPatchVertices );
		glGetIntegerv( GL_MAX_TESS_GEN_LEVEL, &_openglInfos.glMaxTessGenLevel );
		glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &_openglInfos.glMaxComputeWorkGroupCount[ 0 ] );
		glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &_openglInfos.glMaxComputeWorkGroupCount[ 1 ] );
		glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &_openglInfos.glMaxComputeWorkGroupCount[ 2 ] );
		glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &_openglInfos.glMaxComputeWorkGroupSize[ 0 ] );
		glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &_openglInfos.glMaxComputeWorkGroupSize[ 1 ] );
		glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &_openglInfos.glMaxComputeWorkGroupSize[ 2 ] );
		glGetIntegerv( GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &_openglInfos.glMaxComputeWorkGroupInvocations );
		glGetIntegerv( GL_MAX_UNIFORM_BLOCK_SIZE, &_openglInfos.glMaxUniformBlockSize );
		glGetIntegerv( GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &_openglInfos.glMaxShaderStorageBlockSize );
		glGetIntegerv( GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &_openglInfos.glMaxShaderStorageBufferBindings );

#if ( GL_NVX_gpu_memory_info == 1 )
		glGetIntegerv( GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &_openglInfos.gpuMemoryInfoDedicatedVidmemNVX );
		glGetIntegerv( GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX,
					   &_openglInfos.gpuMemoryInfoTotalAvailableMemoryNVX );
		glGetIntegerv( GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX,
					   &_openglInfos.gpuMemoryInfoCurrentAvailableVidMemNVX );
#endif
	}

	void OpenGLRenderer::_setupRouting()
	{
		_passGeometric->in.meshes	 = _bufferMeshes.get();
		_passGeometric->in.molecules = _bufferMolecules.get();

		_passLinearizeDepth->in.textureDepth = _passGeometric->out.textureDepth.get();

		_passSSAO->in.textureDataPacked = _passGeometric->out.textureDataPacked.get();
		_passSSAO->in.textureDepth		= _passLinearizeDepth->out.texture.get();

		_passBlur->in.textureColor = _passSSAO->out.texture.get();
		_passBlur->in.textureDepth = _passLinearizeDepth->out.texture.get();

		_passShading->in.textureDataPacked = _passGeometric->out.textureDataPacked.get();
		_passShading->in.textureColor	   = _passGeometric->out.textureColors.get();
		_passShading->in.textureBlur	   = _passBlur->out.texture.get();

		_passOutline->in.textureColor = _passShading->out.texture.get();
		_passOutline->in.textureDepth = _passLinearizeDepth->out.texture.get();

		_passSelection->in.textureDataPacked = _passGeometric->out.textureDataPacked.get();

		if ( _activeOutline )
		{
			_passSelection->in.textureColor = _passOutline->out.texture.get();
		}
		else
		{
			_passSelection->in.textureColor = _passShading->out.texture.get();
		}
		_passSelection->in.textureDepth = _passLinearizeDepth->out.texture.get();

		_passFXAA->in.textureColor = _passSelection->out.texture.get();
	}

#if ( VTX_OPENGL_VERSION == 450 )
	void APIENTRY OpenGLRenderer::_debugMessageCallback( const GLenum	p_source,
														 const GLenum	p_type,
														 const GLuint	p_id,
														 const GLenum	p_severity,
														 const GLsizei	p_length,
														 const GLchar * p_msg,
														 const void *	p_data )
	{
		std::string source;
		std::string type;
		std::string severity;

		switch ( p_source )
		{
		case GL_DEBUG_SOURCE_API: source = "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: source = "WINDOW SYSTEM"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: source = "SHADER COMPILER"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: source = "THIRD PARTY"; break;
		case GL_DEBUG_SOURCE_APPLICATION: source = "APPLICATION"; break;
		case GL_DEBUG_SOURCE_OTHER: source = "UNKNOWN"; break;
		default: source = "UNKNOWN"; break;
		}

		switch ( p_type )
		{
		case GL_DEBUG_TYPE_ERROR: type = "ERROR"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type = "DEPRECATED BEHAVIOR"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: type = "UDEFINED BEHAVIOR"; break;
		case GL_DEBUG_TYPE_PORTABILITY: type = "PORTABILITY"; break;
		case GL_DEBUG_TYPE_PERFORMANCE: type = "PERFORMANCE"; break;
		case GL_DEBUG_TYPE_OTHER: type = "OTHER"; break;
		case GL_DEBUG_TYPE_MARKER: type = "MARKER"; break;
		default: type = "UNKNOWN"; break;
		}

		switch ( p_severity )
		{
		case GL_DEBUG_SEVERITY_HIGH: severity = "HIGH"; break;
		case GL_DEBUG_SEVERITY_MEDIUM: severity = "MEDIUM"; break;
		case GL_DEBUG_SEVERITY_LOW: severity = "LOW"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: severity = "NOTIFICATION"; break;
		default: severity = "UNKNOWN"; break;
		}

		std::string message( "[OPENGL] [" + severity + "] [" + type + "] " + source + ": " + p_msg );

		switch ( p_severity )
		{
		case GL_DEBUG_SEVERITY_HIGH:
			std::cerr << message << std::endl;
			throw GLException( message );
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
		case GL_DEBUG_SEVERITY_LOW: std::cout << message << std::endl; break;
		default: break;
		}
	}
#endif

} // namespace VTX::Renderer::GL
