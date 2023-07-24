#include "renderer/gl/opengl_renderer.hpp"
#include <util/exceptions.hpp>
#include <util/logger.hpp>

namespace VTX::Renderer::GL
{
	OpenGLRenderer::OpenGLRenderer( const size_t	 p_width,
									const size_t	 p_height,
									const FilePath & p_shaderPath,
									void *			 p_proc ) :
		_width( p_width ),
		_height( p_height )
	{
		VTX_INFO( "Creating renderer..." );

		// Load OpenGL.
		if ( p_proc && gladLoadGLLoader( (GLADloadproc)p_proc ) == 0 )
		{
			throw GLException( "Failed to load OpenGL" );
		}
		else if ( gladLoadGL() == 0 )
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
		_passPixelize		= std::make_unique<Pass::PassPixelize>( p_width, p_height, *_programManager );

		// Setup default routing.
		_setupRouting();

		// Init quad vao/vbo for deferred shading.
		std::vector<Vec2f> quad = { Vec2f( -1.f, 1.f ), Vec2f( -1.f, -1.f ), Vec2f( 1.f, 1.f ), Vec2f( 1.f, -1.f ) };

		_vbo = std::make_unique<Buffer>();
		_vao = std::make_unique<VertexArray>();

		_vao->bind();
		_vao->enableAttribute( 0 );
		_vao->setVertexBuffer<float>( 0, *_vbo, sizeof( Vec2f ) );
		_vao->setAttributeFormat<float>( 0, 2 );
		_vao->setAttributeBinding( 0, 0 );
		_vbo->set( quad );
		_vao->unbind();

		// Camera uniforms buffer.
		_uboCamera = std::make_unique<Buffer>( _uniformsCamera, GL_DYNAMIC_DRAW );

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
		_passPixelize->resize( _width, _height );

		glViewport( 0, 0, GLsizei( _width ), GLsizei( _height ) );
	}

	void OpenGLRenderer::renderFrame()
	{
		if ( _needUpdate )
		{
			//_vao.drawCalls = 0;

			// if ( _skybox )
			//{
			//_skybox->render();
			//}

			_uboCamera->bind( GL_UNIFORM_BUFFER, 15 );

			// Passes.
			if ( _enableTimers )
			{
				_times.fill( 0.f );
				_times[ ENUM_TIME_ITEM::GEOMETRIC ] = CHRONO_GPU( [ & ]() { _passGeometric->render( *_vao ); } );
				_vao->bind();
				_times[ ENUM_TIME_ITEM::LINEARIZE_DEPTH ]
					= CHRONO_GPU( [ & ]() { _passLinearizeDepth->render( *_vao ); } );
				if ( _activeSSAO )
				{
					_times[ ENUM_TIME_ITEM::SSAO ] = CHRONO_GPU( [ & ]() { _passSSAO->render( *_vao ); } );
					_times[ ENUM_TIME_ITEM::BLUR ] = CHRONO_GPU( [ & ]() { _passBlur->render( *_vao ); } );
				}
				_times[ ENUM_TIME_ITEM::SHADING ] = CHRONO_GPU( [ & ]() { _passShading->render( *_vao ); } );
				if ( _activeOutline )
				{
					_times[ ENUM_TIME_ITEM::OUTLINE ] = CHRONO_GPU( [ & ]() { _passOutline->render( *_vao ); } );
				}
				_times[ ENUM_TIME_ITEM::SELECTION ] = CHRONO_GPU( [ & ]() { _passSelection->render( *_vao ); } );
				if ( _activeFXAA )
				{
					_times[ ENUM_TIME_ITEM::FXAA ] = CHRONO_GPU( [ & ]() { _passFXAA->render( *_vao ); } );
				}
				if ( _activePixelize )
				{
					_times[ ENUM_TIME_ITEM::PIXELIZE ] = CHRONO_GPU( [ & ]() { _passPixelize->render( *_vao ); } );
				}
				_vao->unbind();
				_times[ ENUM_TIME_ITEM::BLIT ] = CHRONO_GPU( [ & ]() { _blitToOutput(); } );
			}
			else
			{
				_passGeometric->render( *_vao );
				_vao->bind();
				_passLinearizeDepth->render( *_vao );
				if ( _activeSSAO )
				{
					_passSSAO->render( *_vao );
					_passBlur->render( *_vao );
				}
				_passShading->render( *_vao );
				if ( _activeOutline )
				{
					_passOutline->render( *_vao );
				}
				_passSelection->render( *_vao );
				if ( _activeFXAA )
				{
					_passFXAA->render( *_vao );
				}
				if ( _activePixelize )
				{
					_passPixelize->render( *_vao );
				}
				_vao->unbind();
				_blitToOutput();
			}

			_uboCamera->unbind( 15 );

			//_needUpdate = false;
		}

#if ( GL_NVX_gpu_memory_info == 1 )
		if ( _openglInfos.glExtensions[ ENUM_GL_EXTENSIONS::NVX_gpu_memory_info ] )
		{
			glGetIntegerv( GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX,
						   &_openglInfos.gpuMemoryInfoCurrentAvailableVidMemNVX );
		}
#endif
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

	void OpenGLRenderer::setActivePixelize( const bool p_active )
	{
		_activePixelize = p_active;
		_setupRouting();
	}

	void OpenGLRenderer::setMatrixModelTmp( const Mat4f & p_model )
	{
		_uboCamera->setSub( p_model, offsetof( StructUniformsCamera, matrixModel ), sizeof( Mat4f ) );
		_uboCamera->setSub( Util::Math::transpose( Util::Math::inverse( p_model ) ),
							offsetof( StructUniformsCamera, matrixNormal ),
							sizeof( Mat4f ) );
	}

	void OpenGLRenderer::setMatrixView( const Mat4f & p_view )
	{
		_uniformsCamera.matrixView = p_view;
		_uboCamera->setSub( p_view, offsetof( StructUniformsCamera, matrixView ), sizeof( Mat4f ) );
	}

	void OpenGLRenderer::setMatrixProjection( const Mat4f & p_proj )
	{
		_uniformsCamera.matrixProjection = p_proj;
		_uboCamera->setSub( p_proj, offsetof( StructUniformsCamera, matrixProjection ), sizeof( Mat4f ) );
	}

	void OpenGLRenderer::setCameraClipInfos( const float p_near, const float p_far )
	{
		_uniformsCamera.cameraClipInfos = Vec4f( p_near * p_far, p_far, p_far - p_near, p_near );
		_uboCamera->setSub(
			_uniformsCamera.cameraClipInfos, offsetof( StructUniformsCamera, cameraClipInfos ), sizeof( Vec4f ) );
	}

	void OpenGLRenderer::setBlurSize( const float p_size ) { _passBlur->setSize( p_size ); }

	void OpenGLRenderer::setSSAOIntensity( const float p_intensity ) { _passSSAO->setIntensity( p_intensity ); }

	void OpenGLRenderer::setShadingMode( const ENUM_SHADING p_shading ) { _passShading->setMode( p_shading ); }

	void OpenGLRenderer::setSpecularFactor( const float p_specularFactor )
	{
		_passShading->setSpecularFactor( p_specularFactor );
	}

	void OpenGLRenderer::setToonSteps( const uint p_steps ) { _passShading->setToonSteps( p_steps ); }

	void OpenGLRenderer::setColorBackground( const Util::Color::Rgba & p_color )
	{
		_passShading->setColorBackground( p_color );
	}

	void OpenGLRenderer::setColorLight( const Util::Color::Rgba & p_color ) { _passShading->setColorLight( p_color ); }

	void OpenGLRenderer::setColorFog( const Util::Color::Rgba & p_color ) { _passShading->setColorFog( p_color ); }

	void OpenGLRenderer::setFogNear( const float p_near ) { _passShading->setFogNear( p_near ); }

	void OpenGLRenderer::setFogFar( const float p_far ) { _passShading->setFogFar( p_far ); }

	void OpenGLRenderer::setFogDensity( const float p_density ) { _passShading->setFogDensity( p_density ); }

	void OpenGLRenderer::setOutlineSensivity( const float p_sensivity ) { _passOutline->setSensivity( p_sensivity ); }

	void OpenGLRenderer::setOutlineThickness( const uint p_thickness ) { _passOutline->setThickness( p_thickness ); }

	void OpenGLRenderer::setColorOutline( const Util::Color::Rgba & p_color ) { _passOutline->setColor( p_color ); }

	void OpenGLRenderer::setColorSelection( const Util::Color::Rgba & p_color ) { _passSelection->setColor( p_color ); }

	void OpenGLRenderer::setPixelSize( const uint p_size ) { _passPixelize->setSize( p_size ); }

	void OpenGLRenderer::setPixelizeBackground( const bool p_background )
	{
		_passPixelize->setBackground( p_background );
	}

	void OpenGLRenderer::loadSkybox( const std::array<unsigned char *, 6> & p_textures,
									 const size_t							p_width,
									 const size_t							p_height )
	{
		_skybox = std::make_unique<Cubemap>( p_width, p_height, p_textures, *_programManager );
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

		// Extensions.
		GLint numExtensions = 0;
		glGetIntegerv( GL_NUM_EXTENSIONS, &numExtensions );
		for ( GLint i = 0; i < numExtensions; ++i )
		{
			const char * extension = (const char *)glGetStringi( GL_EXTENSIONS, i );
			if ( strcmp( "GL_NVX_gpu_memory_info", extension ) == 0 )
			{
				_openglInfos.glExtensions[ ENUM_GL_EXTENSIONS::NVX_gpu_memory_info ] = true;
			}
		}

// NVX_gpu_memory_info
#if ( GL_NVX_gpu_memory_info == 1 )
		if ( _openglInfos.glExtensions[ ENUM_GL_EXTENSIONS::NVX_gpu_memory_info ] )
		{
			glGetIntegerv( GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &_openglInfos.gpuMemoryInfoDedicatedVidmemNVX );
			glGetIntegerv( GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX,
						   &_openglInfos.gpuMemoryInfoTotalAvailableMemoryNVX );
			glGetIntegerv( GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX,
						   &_openglInfos.gpuMemoryInfoCurrentAvailableVidMemNVX );
		}
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

		_passSelection->in.textureColor
			= _activeOutline ? _passOutline->out.texture.get() : _passShading->out.texture.get();

		_passSelection->in.textureDepth = _passLinearizeDepth->out.texture.get();

		_passFXAA->in.textureColor = _passSelection->out.texture.get();

		_passPixelize->in.textureDataPacked = _passGeometric->out.textureDataPacked.get();
		_passPixelize->in.textureColor = _activeFXAA ? _passFXAA->out.texture.get() : _passSelection->out.texture.get();
	}

	void OpenGLRenderer::_blitToOutput()
	{
		glBindFramebuffer( GL_READ_FRAMEBUFFER,
						   _activePixelize ? _passPixelize->out.fbo->getId()
						   : _activeFXAA   ? _passFXAA->out.fbo->getId()
										   : _passSelection->out.fbo->getId() );
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
