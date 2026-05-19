#include "renderer/renderer.hpp"
#include "renderer/binary_buffer.hpp"
#include "renderer/builder/system_build.hpp"
#include <util/chrono.hpp>

namespace
{
	/*
	auto linearizeColorFloat = []( float c ) -> float
	{
		return c;
		if ( c <= 0.04045f )
			return c / 12.92f;
		return std::pow( ( c + 0.055f ) / 1.055f, 2.4f );
	};

	using namespace VTX;
	auto linearizeColor = []( const Util::Color::Rgba & c ) -> Vec4f
	{
		return Vec4f( linearizeColorFloat( c.r() ), linearizeColorFloat( c.g() ), linearizeColorFloat( c.b() ), c.a() );
	};
	*/

} // namespace

namespace VTX::Renderer
{
	Renderer::Renderer( const size_t p_width, const size_t p_height ) : _width( p_width ), _height( p_height ) {}

#pragma region Contexts

	void Renderer::setDefault()
	{
		Util::ScopedChrono timer( "[RENDERER] setDefault" );

		_context.setNull();
		try
		{
			_context.build( _queue, _graph.getResources() );
			_bindExternalPasses();
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "{}", p_e.what() );
			VTX_ERROR( "Can not build default backend" );
		}
	}

	void Renderer::setOpenGL( const Desc::NativeContextInfo & p_contextInfo, const FilePath & p_shaderDir )
	{
		Util::ScopedChrono timer( "[RENDERER] setOpenGL46 / surface: " + std::to_string( p_contextInfo.surface ) );

		_context.setOpenGL( _width, _height, p_contextInfo, p_shaderDir );
		try
		{
			_context.build( _queue, _graph.getResources() );
			_bindExternalPasses();
			_context.fillInfos( _infos );
			onReady();
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "{}", p_e.what() );
			VTX_ERROR( "Can not build openGL 4.6 backend" );
			setDefault();
		}
	}

#pragma endregion

#pragma region Renderer

	void Renderer::resize( const size_t p_width, const size_t p_height )
	{
		Util::ScopedChrono timer( "[RENDERER] resize" );

		VTX_TRACE( "Resizing renderer to {}x{}", p_width, p_height );

		_width	= p_width;
		_height = p_height;

		_context.resize(
			static_cast<uint32_t>( p_width ),
			static_cast<uint32_t>( p_height ),
			_graph.getPasses(),
			_graph.getResources().textures
		);

		setNeedUpdate( true );
	}

	void Renderer::clear()
	{
		_context.clear();
		_graph.clear();
		_needUpdate = false;
	}

	bool Renderer::render( const float p_deltaTime, const float p_elapsedTime ) noexcept
	{
		if ( _systemToRefresh.size() )
		{
			Builder::Context buildContext {
				_context, _systems, _cacheRepresentations, _cacheCamera, _layouts, _geometries, _systemToRefresh,
			};
			for ( const auto & system : _systemToRefresh )
			{
				Builder::SystemVisibility::refreshGeometryVisibility( buildContext, system );
			}
			Builder::DrawRanges::buildDrawRanges( buildContext );

			_systemToRefresh.clear();
			setNeedUpdate( true );
		}

		if ( _needUpdate || _forceUpdate )
		{
			_render( p_deltaTime, p_elapsedTime );
			setNeedUpdate( false );
			return true;
		}

		return false;
	}

	std::vector<std::byte> Renderer::snapshot()
	{
		Util::ScopedChrono timer( "[RENDERER] snapshot" );

		_context.setRenderTarget( Desc::E_RENDER_TARGET::OFFSCREEN );
		_context.build( _queue, _graph.getResources() );
		_bindExternalPasses();
		_render( 0.f, 0.f );

		// TODO: get last pass instead of hardcoding FXAA.
		std::vector<std::byte> data = _context.getTextureData( "FXAA", Desc::E_FORMAT::RGBA8UI );

		_context.setRenderTarget( Desc::E_RENDER_TARGET::SCREEN );
		_context.build( _queue, _graph.getResources() );
		_bindExternalPasses();
		_render( 0.f, 0.f );

		return data;
	}

	Vec2i Renderer::getPickedIds( const size_t p_x, const size_t p_y ) const
	{
		std::vector<std::byte> data = _context.getTextureData( "Picking", {}, p_x, _height - p_y );

		assert( data.size() == sizeof( Vec2i ) );

		Vec2i v;
		std::memcpy( &v, data.data(), sizeof( Vec2i ) );

		return v;
	}

	const StructInfos & Renderer::getInfos( const bool p_refresh )
	{
		if ( not p_refresh )
		{
			return _infos;
		}

		_context.fillInfos( _infos );

		return _infos;
	}

	void Renderer::_bindExternalPasses()
	{
		if ( not _context.containsPass( Geometry::SES::PASS_COMPUTE ) )
		{
			return;
		}

		const auto function = reinterpret_cast<uintptr_t>( &Renderer::_executeSESExternalPass );
		const auto context	= reinterpret_cast<uintptr_t>( this );
		_context.setExternalPass( Geometry::SES::PASS_COMPUTE, function, context );
	}

	void Renderer::_markSESDirty()
	{
		if ( _context.containsPass( Geometry::SES::PASS_COMPUTE ) )
		{
			_context.markPassDirty( Geometry::SES::PASS_COMPUTE );
		}
	}

	void Renderer::_executeSESExternalPass( const uintptr_t p_context )
	{
		auto * const renderer = reinterpret_cast<Renderer *>( p_context );
		if ( renderer == nullptr )
		{
			return;
		}

		try
		{
			renderer->_geometries.ses.compute( renderer->_context );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "SES external pass failed: {}", p_e.what() );
		}
	}

#pragma endregion

#pragma region Buffers

	void Renderer::setCamera(
		const Camera & p_camera,
		const Vec3f &  p_position,
		const Mat4f &  p_matView,
		const Mat4f &  p_matProj
	)
	{
		// Util::ScopedChrono timer( "[RENDERER] setCamera" );

		const Mat4f matrixViewInv	   = Util::Math::inverse( p_matView );
		const Mat4f matrixViewInvTrans = Util::Math::transpose( matrixViewInv );

		BinaryBuffer140 buffer;
		buffer.write( p_matView );
		buffer.write( p_matProj );
		buffer.write( matrixViewInv );
		buffer.write( matrixViewInvTrans );
		buffer.write( p_position );
		buffer.write(
			Vec4f( p_camera.near * p_camera.far, p_camera.far, p_camera.far - p_camera.near, p_camera.near )
		);
		buffer.write( Vec2i( width(), height() ) );
		buffer.write( Vec2i() );
		buffer.write( uint( p_camera.projection == PROJECTION::PERSPECTIVE ) );
		buffer.close();

		_context.setBuffer( "Camera", buffer );

		_cacheCamera = { p_camera, p_position, p_matView, p_matProj };

		Builder::Context buildContext {
			_context, _systems, _cacheRepresentations, _cacheCamera, _layouts, _geometries, _systemToRefresh,
		};
		Builder::SystemModels::upload( buildContext );

		setNeedUpdate( true );
	}

	void Renderer::setGraphicsConfig( const GraphicsConfig & p_config )
	{
		Util::ScopedChrono timer( "[RENDERER] setGraphicsConfig" );

		// If graph changed from config, rebuild backend.
		if ( _refreshGraph( p_config ) )
		{
			_context.build( _queue, _graph.getResources() );
			_bindExternalPasses();
		}

		BinaryBuffer140 bufferShading;
		bufferShading.write( p_config.colorBackground );
		bufferShading.write( p_config.colorLight );
		bufferShading.write( p_config.colorFog );
		bufferShading.write( uint32_t( p_config.shadingMode ) );
		bufferShading.write( p_config.specularFactor );
		bufferShading.write( p_config.shininess );
		bufferShading.write( p_config.toonSteps );
		bufferShading.write( p_config.fogNear );
		bufferShading.write( p_config.fogFar );
		bufferShading.write( p_config.activeFog ? p_config.fogDensity : 0.f );
		bufferShading.close();
		_context.setBuffer( "Shading", bufferShading );

		if ( p_config.activeSSAO )
		{
			BinaryBuffer140 bufferSSAO;
			bufferSSAO.write( p_config.ssaoIntensity );
			bufferSSAO.close();
			_context.setBuffer( "SSAO", bufferSSAO );

			BinaryBuffer140 bufferBlurX;
			bufferBlurX.write( Vec2i( 1, 0 ) );
			bufferBlurX.write( p_config.blurSize );
			bufferBlurX.close();
			_context.setBuffer( "BlurX", bufferBlurX );
			BinaryBuffer140 bufferBlurY;
			bufferBlurY.write( Vec2i( 0, 1 ) );
			bufferBlurY.write( p_config.blurSize );
			bufferBlurY.close();
			_context.setBuffer( "BlurY", bufferBlurY );
		}
		if ( p_config.activeOutline )
		{
			BinaryBuffer140 bufferOutline;
			bufferOutline.write( p_config.colorOutline );
			bufferOutline.write( p_config.outlineSensitivity );
			bufferOutline.write( p_config.outlineThickness );
			bufferOutline.close();
			_context.setBuffer( "Outline", bufferOutline );
		}
		if ( p_config.activeSelection )
		{
			BinaryBuffer140 bufferSelection;
			bufferSelection.write( p_config.colorSelection );
			bufferSelection.close();
			_context.setBuffer( "Selection", bufferSelection );
		}

		setNeedUpdate( true );
	}

	void Renderer::setColorLayout( const Color::Layout & p_layout )
	{
		Util::ScopedChrono timer( "[RENDERER] setColorLayout" );

		_context.setBuffer<Util::Color::Rgba>( "ColorLayout", p_layout.colors );

		setNeedUpdate( true );
	}

	void Renderer::setRepresentations( const std::vector<const Representation *> & p_representations )
	{
		Util::ScopedChrono timer( "[RENDERER] setRepresentations" );

		BinaryBuffer<E_LAYOUT_TYPE::Std140> buffer;
		RepresentationIndex					index = 0;

		for ( const auto * representation : p_representations )
		{
			// Aply logic.
			bool showSphere	  = representation->hasSphere;
			bool showCylinder = representation->hasCylinder;
			bool showRibbon	  = representation->hasRibbon;
			bool showSes	  = representation->hasSes;

			bool		isSphereRadiusFixed = representation->isRadiusSphereFixed;
			const float cylinderRadius		= representation->radiusCylinder;
			float		sphereRadiusFixed	= representation->radiusSphereFixed;

			// Asked SES, hide all others.
			if ( showSes )
			{
				showSphere	 = false;
				showCylinder = false;
				showRibbon	 = false;
			}
			else
			{
				// Pure VdW display is sphere-only, but keep other primitives available when spheres are hidden.
				if ( showSphere && not isSphereRadiusFixed )
				{
					showCylinder = false;
					showRibbon	 = false;
				}
				else
				{
					// If B&S.
					if ( showSphere && showCylinder )
					{
						// Scale sphere radius to cylinder radius if not VdW.
						if ( isSphereRadiusFixed && sphereRadiusFixed < cylinderRadius )
						{
							sphereRadiusFixed = cylinderRadius;
						}
					}

					// If sticks only, force sphere at cylinder radius.
					else if ( not showSphere && showCylinder )
					{
						showSphere			= true;
						isSphereRadiusFixed = true;
						sphereRadiusFixed	= cylinderRadius;
					}
				}
			}

			// Write buffer.
			buffer.write( sphereRadiusFixed );
			buffer.write( representation->radiusSphereAdd );
			buffer.write( uint( isSphereRadiusFixed ) );
			buffer.write( representation->radiusCylinder );
			buffer.write( uint( representation->cylinderColorBlending ) );
			buffer.write( uint( representation->ribbonColorBlending ) );
			buffer.write( representation->sesProbeRadius );
			buffer.write( Geometry::SES::MAX_PROBE_NEIGHBOR_NB );

			// Cache.
			_cacheRepresentations[ index ] = Cache::Representation { showSphere, showCylinder, showRibbon, showSes };

			index++;
		}

		buffer.close();

		_context.setBuffer( "Representations", buffer );

		auto handles	 = _systems.handles();
		_systemToRefresh = std::unordered_set<Desc::Handle>( handles.begin(), handles.end() );

		setNeedUpdate( true );
	}

#pragma endregion

#pragma region Geometries

	void Renderer::setSystems( const std::vector<SystemData> & p_systems )
	{
		Util::ScopedChrono timer( "[RENDERER] setSystems" );

		Builder::Context buildContext {
			_context, _systems, _cacheRepresentations, _cacheCamera, _layouts, _geometries, _systemToRefresh,
		};

		Builder::DefaultPipeline		  pipeline;
		const std::span<const SystemData> systems( p_systems );
		pipeline.clear( buildContext );
		pipeline.registerSystems( buildContext, systems );
		pipeline.allocateInputs( buildContext );
		pipeline.allocateOutputs( buildContext );
		pipeline.uploadInputs( buildContext, systems );
		pipeline.buildDerived( buildContext, systems );
		pipeline.writeOutputs( buildContext, systems );

		Builder::SystemModels::upload( buildContext );

		// Rebuild commands immediately, including the empty-scene case.
		pipeline.buildDrawRanges( buildContext );
		_markSESDirty();

		// Build draw ranges.
		auto handles	 = _systems.handles();
		_systemToRefresh = std::unordered_set<Desc::Handle>( handles.begin(), handles.end() );

		setNeedUpdate( true );
	}

	void Renderer::setSystemTransform( const SystemUID p_uid, const Mat4f & p_transform )
	{
		assert( _systems.contains( p_uid ) );

		Util::ScopedChrono timer( "[RENDERER] setSystemTransform" );

		const Desc::Handle h = _systems.handle( p_uid );
		assert( _systems.contains( h ) );
		_systems.get( h ).transform = p_transform;

		Builder::Context buildContext {
			_context, _systems, _cacheRepresentations, _cacheCamera, _layouts, _geometries, _systemToRefresh,
		};
		Builder::SystemModels::upload( buildContext );
		setNeedUpdate( true );
	}

	void Renderer::setSystemPosition( const SystemUID p_uid, std::span<const Vec3f> p_positions )
	{
		assert( _systems.contains( p_uid ) );

		const Desc::Handle h = _systems.handle( p_uid );
		Builder::Context buildContext {
			_context, _systems, _cacheRepresentations, _cacheCamera, _layouts, _geometries, _systemToRefresh,
		};

		Builder::AtomLayout::uploadPositions( buildContext, h, p_positions );
		Builder::RibbonGeometry::uploadPositions( buildContext, h, p_positions );

		setNeedUpdate( true );
	}

	void Renderer::setSystemColors( const SystemUID p_uid, std::span<const ColorIndex> p_colors )
	{
		assert( _systems.contains( p_uid ) );

		const Desc::Handle h = _systems.handle( p_uid );
		Builder::Context buildContext {
			_context, _systems, _cacheRepresentations, _cacheCamera, _layouts, _geometries, _systemToRefresh,
		};
		Builder::AtomLayout::uploadColors( buildContext, h, p_colors );

		setNeedUpdate( true );
	}

	void Renderer::setSystemRepresentation(
		const SystemUID						 p_uid,
		const MapRepresentationRanges &		 p_representations,
		std::span<const RepresentationIndex> p_atomRepresentations
	)
	{
		assert( _systems.contains( p_uid ) );

		Util::ScopedChrono timer( "[RENDERER] setSystemRepresentation" );

		const Desc::Handle h = _systems.handle( p_uid );
		Builder::Context buildContext {
			_context, _systems, _cacheRepresentations, _cacheCamera, _layouts, _geometries, _systemToRefresh,
		};
		Builder::AtomLayout::uploadRepresentations( buildContext, h, p_representations, p_atomRepresentations );
		Builder::ResidueLayout::uploadRepresentations( buildContext, h, p_atomRepresentations );

		setNeedUpdate( true );
	}

	void Renderer::setSystemVisibility( const SystemUID p_uid, const Util::Math::BitSet & p_visibility )
	{
		assert( _systems.contains( p_uid ) );

		Util::ScopedChrono timer( "[RENDERER] setSystemVisibility" );

		const Desc::Handle h = _systems.handle( p_uid );
		Builder::Context buildContext {
			_context, _systems, _cacheRepresentations, _cacheCamera, _layouts, _geometries, _systemToRefresh,
		};
		Builder::SystemVisibility::uploadVisibility( buildContext, h, p_visibility );

		setNeedUpdate( true );
	}

	void Renderer::setSystemSelection( const SystemUID p_uid, std::span<const Flag> p_atomFlags )
	{
		assert( _systems.contains( p_uid ) );

		Util::ScopedChrono timer( "[RENDERER] setSystemSelection" );

		const Desc::Handle h = _systems.handle( p_uid );
		Builder::Context buildContext {
			_context, _systems, _cacheRepresentations, _cacheCamera, _layouts, _geometries, _systemToRefresh,
		};
		Builder::AtomLayout::uploadSelection( buildContext, h, p_atomFlags );
		Builder::ResidueLayout::uploadSelection( buildContext, h, p_atomFlags );

		setNeedUpdate( true );
	}

	void Renderer::setVoxels( std::span<const Vec3f> p_mins, std::span<const Vec3f> p_maxs )
	{
		assert( p_mins.size() == p_maxs.size() );

		_geometries.grid.setVoxelCount( p_mins.size() );
		_layouts.voxels.resizeStorage( _context, _geometries.grid.voxelCount() );

		_layouts.voxels.upload<Layout::VOXEL_ATTR::MINS, Vec3f>( _context, Desc::NO_HANDLE, p_mins );
		_layouts.voxels.upload<Layout::VOXEL_ATTR::MAXS, Vec3f>( _context, Desc::NO_HANDLE, p_maxs );

		Builder::Context buildContext {
			_context, _systems, _cacheRepresentations, _cacheCamera, _layouts, _geometries, _systemToRefresh,
		};
		Builder::DrawRanges::buildDrawRanges( buildContext );

		setNeedUpdate( true );
	}

#pragma endregion

#pragma region Internals

	bool Renderer::_refreshGraph( const GraphicsConfig & p_config )
	{
		Util::ScopedChrono timer( "[RENDERER] _refreshGraph" );

		RenderGraph::PipelineConfig config;

		config.enableSSAO	   = p_config.activeSSAO;
		config.enableOutline   = p_config.activeOutline;
		config.enableSelection = p_config.activeSelection;

		auto & configOpt = _graph.getPipelineConfig();
		if ( configOpt && *configOpt == config )
		{
			return false;
		}

		_graph.createDefaultPipeline( config, _layouts, _geometries );
		_queue = _graph.build();
		return true;
	}

#pragma endregion
} // namespace VTX::Renderer
