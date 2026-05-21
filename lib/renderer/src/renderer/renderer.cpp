#include "renderer/renderer.hpp"
#include "renderer/binary_buffer.hpp"
#include "renderer/builder/render_graph_build.hpp"
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
			_rebuildCommandBuffer();
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
			_rebuildCommandBuffer();
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
		_dirty.clear();
		_needUpdate = false;
	}

	bool Renderer::render( const float p_deltaTime, const float p_elapsedTime ) noexcept
	{
		if ( not _dirty.empty() )
		{
			_flushDirty();
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
		_rebuildCommandBuffer();
		_render( 0.f, 0.f );

		// TODO: get last pass instead of hardcoding FXAA.
		std::vector<std::byte> data = _context.getTextureData( "FXAA", Desc::E_FORMAT::RGBA8UI );

		_context.setRenderTarget( Desc::E_RENDER_TARGET::SCREEN );
		_rebuildCommandBuffer();
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
		if ( _context.containsPass( Geometry::SES::PASS_COMPUTE ) && _geometries.ses.hasPendingCompute() )
		{
			_context.markPassDirty( Geometry::SES::PASS_COMPUTE );
		}
	}

	void Renderer::_flushDirty()
	{
		std::vector<Desc::Handle> invalidDirtySystems;
		const auto collectInvalidDirtySystems = [ & ]( const auto & p_handles )
		{
			for ( const Desc::Handle handle : p_handles )
			{
				if ( not _systems.contains( handle ) )
				{
					invalidDirtySystems.emplace_back( handle );
				}
			}
		};

		collectInvalidDirtySystems( _dirty.addedSystems );
		collectInvalidDirtySystems( _dirty.atomPositions );
		collectInvalidDirtySystems( _dirty.atomColors );
		collectInvalidDirtySystems( _dirty.atomRepresentations );
		collectInvalidDirtySystems( _dirty.atomSelection );
		collectInvalidDirtySystems( _dirty.systemModels );
		collectInvalidDirtySystems( _dirty.geometrySystems );

		for ( const Desc::Handle handle : invalidDirtySystems )
		{
			_dirty.removeSystem( handle );
		}

		Builder::Context buildContext {
			_context, _systems, _cacheRepresentations, _cacheCamera, _layouts, _geometries, _dirty.geometrySystems,
		};
		Builder::DefaultPipeline pipeline;

		if ( not _dirty.addedSystems.empty() )
		{
			std::vector<SystemData> cachedSystems;
			const auto				handles = _systems.handles();
			_dirty.markGeometries( handles );

			cachedSystems.reserve( handles.size() );
			for ( const Desc::Handle handle : handles )
			{
				cachedSystems.emplace_back( Builder::SystemVisibility::systemData( buildContext, handle ) );
			}
			const std::span<const SystemData> allSystems( cachedSystems );

			pipeline.allocateInputs( buildContext );
			pipeline.uploadInputs( buildContext, allSystems );
			pipeline.buildDerived( buildContext, allSystems );
			pipeline.allocateOutputs( buildContext );
			pipeline.writeOutputs( buildContext, allSystems );

			Builder::SystemModels::upload( buildContext );
		}

		for ( const Desc::Handle system : _dirty.atomPositions )
		{
			const Cache::System & cache = _systems.get( system );
			Builder::AtomLayout::uploadPositions( buildContext, system, cache.trajectory );
			Builder::RibbonGeometry::uploadPositions( buildContext, system, cache.trajectory );
		}
		for ( const Desc::Handle system : _dirty.atomColors )
		{
			Builder::AtomLayout::uploadColors( buildContext, system, _systems.get( system ).atomColors );
		}
		for ( const Desc::Handle system : _dirty.atomRepresentations )
		{
			const Cache::System & cache = _systems.get( system );
			Builder::AtomLayout::uploadRepresentations(
				buildContext, system, cache.representations, cache.atomRepresentations
			);
			Builder::ResidueLayout::uploadRepresentations( buildContext, system, cache.atomRepresentations );
		}
		for ( const Desc::Handle system : _dirty.atomSelection )
		{
			const Cache::System & cache = _systems.get( system );
			Builder::AtomLayout::uploadSelection( buildContext, system, cache.atomFlags );
			Builder::ResidueLayout::uploadSelection( buildContext, system, cache.atomFlags );
		}
		if ( not _dirty.systemModels.empty() )
		{
			Builder::SystemModels::upload( buildContext );
		}

		for ( const auto & system : _dirty.geometrySystems )
		{
			Builder::SystemVisibility::refreshGeometryVisibility( buildContext, system );
		}
		for ( const auto & system : _dirty.geometrySystems )
		{
			_geometries.uploadIndexes( _context, system );
		}

		if ( _dirty.drawRanges )
		{
			Builder::DrawRanges::buildDrawRanges( buildContext );
		}
		if ( _dirty.geometryChunks && _syncGeometryChunks() )
		{
			_rebuildCommandBuffer();
		}
		if ( _dirty.externalPasses )
		{
			_markSESDirty();
		}

		_dirty.clear();
		setNeedUpdate( true );
	}

	bool Renderer::_syncGeometryChunks()
	{
		bool changed = false;

		auto sync = [ & ]( const Desc::Key & p_geometry, const Desc::Geometry & p_source )
		{
			if ( not _graph.getResources().geometries.contains( p_geometry ) )
			{
				return;
			}

			changed = _graph.setGeometryChunks( p_geometry, p_source.chunks ) || changed;
		};

		sync( Geometry::SES::GEOMETRY_CONVEX_PATCHES, _geometries.ses.convexPatches );
		sync( Geometry::SES::GEOMETRY_CIRCLE_PATCHES, _geometries.ses.circlePatches );
		sync( Geometry::SES::GEOMETRY_SEGMENT_PATCHES, _geometries.ses.segmentPatches );
		sync( Geometry::SES::GEOMETRY_CONCAVE_PATCHES, _geometries.ses.concavePatches );

		return changed;
	}

	void Renderer::_rebuildCommandBuffer()
	{
		_context.build( _queue, _graph.getResources() );
		_bindExternalPasses();
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

		_context.setBuffer( { "Camera" }, buffer );

		_cacheCamera = { p_camera, p_position, p_matView, p_matProj };

		Builder::Context buildContext {
			_context, _systems, _cacheRepresentations, _cacheCamera, _layouts, _geometries, _dirty.geometrySystems,
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
			_rebuildCommandBuffer();
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
		_context.setBuffer( { "Shading" }, bufferShading );

		if ( p_config.activeSSAO )
		{
			BinaryBuffer140 bufferSSAO;
			bufferSSAO.write( p_config.ssaoIntensity );
			bufferSSAO.close();
			_context.setBuffer( { "SSAO" }, bufferSSAO );

			BinaryBuffer140 bufferBlurX;
			bufferBlurX.write( Vec2i( 1, 0 ) );
			bufferBlurX.write( p_config.blurSize );
			bufferBlurX.close();
			_context.setBuffer( { "BlurX" }, bufferBlurX );
			BinaryBuffer140 bufferBlurY;
			bufferBlurY.write( Vec2i( 0, 1 ) );
			bufferBlurY.write( p_config.blurSize );
			bufferBlurY.close();
			_context.setBuffer( { "BlurY" }, bufferBlurY );
		}
		if ( p_config.activeOutline )
		{
			BinaryBuffer140 bufferOutline;
			bufferOutline.write( p_config.colorOutline );
			bufferOutline.write( p_config.outlineSensitivity );
			bufferOutline.write( p_config.outlineThickness );
			bufferOutline.close();
			_context.setBuffer( { "Outline" }, bufferOutline );
		}
		if ( p_config.activeSelection )
		{
			BinaryBuffer140 bufferSelection;
			bufferSelection.write( p_config.colorSelection );
			bufferSelection.close();
			_context.setBuffer( { "Selection" }, bufferSelection );
		}

		setNeedUpdate( true );
	}

	void Renderer::setColorLayout( const Color::Layout & p_layout )
	{
		Util::ScopedChrono timer( "[RENDERER] setColorLayout" );

		_context.setBuffer<Util::Color::Rgba>( { "ColorLayout" }, p_layout.colors );

		setNeedUpdate( true );
	}

	void Renderer::setRepresentations( const std::vector<const Representation *> & p_representations )
	{
		Util::ScopedChrono timer( "[RENDERER] setRepresentations" );

		Builder::Context buildContext {
			_context, _systems, _cacheRepresentations, _cacheCamera, _layouts, _geometries, _dirty.geometrySystems,
		};
		Builder::RepresentationState::upload( buildContext, p_representations );

		auto handles = _systems.handles();
		_dirty.markGeometries( handles );
		setNeedUpdate( true );
	}

#pragma endregion

#pragma region Geometries

	void Renderer::addSystem( const SystemData & p_system )
	{
		Util::ScopedChrono timer( "[RENDERER] addSystem" );

		Builder::Context buildContext {
			_context, _systems, _cacheRepresentations, _cacheCamera, _layouts, _geometries, _dirty.geometrySystems,
		};

		Builder::DefaultPipeline pipeline;
		const std::span<const SystemData> systems( &p_system, 1 );
		pipeline.registerSystems( buildContext, systems );

		const Desc::Handle handle = _systems.handle( p_system.uid );
		_dirty.markAddedSystem( handle );

		setNeedUpdate( true );
	}

	void Renderer::removeSystem( const SystemUID p_uid )
	{
		Util::ScopedChrono timer( "[RENDERER] removeSystem" );

		if ( not _systems.contains( p_uid ) )
		{
			return;
		}

		Builder::Context buildContext {
			_context, _systems, _cacheRepresentations, _cacheCamera, _layouts, _geometries, _dirty.geometrySystems,
		};

		_systems.erase( p_uid );

		std::vector<SystemData>		   remainingSystems;
		std::vector<std::vector<Vec3f>> remainingTrajectories;
		const auto					   remainingHandles = _systems.handles();
		remainingSystems.reserve( remainingHandles.size() );
		remainingTrajectories.reserve( remainingHandles.size() );
		for ( const Desc::Handle handle : remainingHandles )
		{
			const Cache::System & cache = _systems.get( handle );
			assert( cache.data != nullptr );

			remainingTrajectories.emplace_back( cache.trajectory );
			remainingSystems.emplace_back(
				SystemData { cache.uid,
							 cache.transform,
							 *cache.data,
							 remainingTrajectories.back(),
							 cache.atomUids,
							 cache.residueUids,
							 cache.atomColors,
							 cache.representations,
							 cache.atomRepresentations,
							 cache.visibility,
							 cache.atomFlags }
			);
		}

		Builder::DefaultPipeline pipeline;
		pipeline.clear( buildContext );

		const std::span<const SystemData> systems( remainingSystems );
		pipeline.registerSystems( buildContext, systems );

		_dirty.clear();
		for ( const Desc::Handle handle : _systems.handles() )
		{
			_dirty.markAddedSystem( handle );
		}
		_dirty.markGeometryStructure();

		setNeedUpdate( true );
	}

	bool Renderer::ensureBufferChunk( const Desc::BufferRef & p_ref )
	{
		const bool created = _context.ensureBufferChunk( p_ref );
		if ( not created )
		{
			return false;
		}

		_rebuildCommandBuffer();
		setNeedUpdate( true );

		return true;
	}

	bool Renderer::releaseBufferChunk( const Desc::BufferRef & p_ref )
	{
		const bool released = _context.releaseBufferChunk( p_ref );
		if ( not released )
		{
			return false;
		}

		_rebuildCommandBuffer();
		setNeedUpdate( true );

		return true;
	}

	void Renderer::setSystemTransform( const SystemUID p_uid, const Mat4f & p_transform )
	{
		if ( not _systems.contains( p_uid ) )
		{
			return;
		}

		Util::ScopedChrono timer( "[RENDERER] setSystemTransform" );

		const Desc::Handle h = _systems.handle( p_uid );
		assert( _systems.contains( h ) );
		_systems.get( h ).transform = p_transform;
		_dirty.markSystemModels( h );
		setNeedUpdate( true );
	}

	void Renderer::setSystemPosition( const SystemUID p_uid, std::span<const Vec3f> p_positions )
	{
		if ( not _systems.contains( p_uid ) )
		{
			return;
		}

		const Desc::Handle h = _systems.handle( p_uid );
		_systems.get( h ).trajectory.assign( p_positions.begin(), p_positions.end() );
		_geometries.ses.invalidate( h );
		_dirty.markAtomPositions( h );

		setNeedUpdate( true );
	}

	void Renderer::setSystemColors( const SystemUID p_uid, std::span<const ColorIndex> p_colors )
	{
		if ( not _systems.contains( p_uid ) )
		{
			return;
		}

		const Desc::Handle h = _systems.handle( p_uid );
		_systems.get( h ).atomColors.assign( p_colors.begin(), p_colors.end() );
		_dirty.markAtomColors( h );

		setNeedUpdate( true );
	}

	void Renderer::setSystemRepresentation(
		const SystemUID						 p_uid,
		const MapRepresentationRanges &		 p_representations,
		std::span<const RepresentationIndex> p_atomRepresentations
	)
	{
		if ( not _systems.contains( p_uid ) )
		{
			return;
		}

		Util::ScopedChrono timer( "[RENDERER] setSystemRepresentation" );

		const Desc::Handle h = _systems.handle( p_uid );
		Cache::System &	   cache = _systems.get( h );
		cache.representations	  = p_representations;
		cache.atomRepresentations.assign( p_atomRepresentations.begin(), p_atomRepresentations.end() );
		_dirty.markAtomRepresentations( h );
		setNeedUpdate( true );
	}

	void Renderer::setSystemVisibility( const SystemUID p_uid, const Util::Math::BitSet & p_visibility )
	{
		if ( not _systems.contains( p_uid ) )
		{
			return;
		}

		Util::ScopedChrono timer( "[RENDERER] setSystemVisibility" );

		const Desc::Handle h = _systems.handle( p_uid );
		_systems.get( h ).visibility = p_visibility;
		_dirty.markGeometry( h );
		setNeedUpdate( true );
	}

	void Renderer::setSystemSelection( const SystemUID p_uid, std::span<const Flag> p_atomFlags )
	{
		if ( not _systems.contains( p_uid ) )
		{
			return;
		}

		Util::ScopedChrono timer( "[RENDERER] setSystemSelection" );

		const Desc::Handle h = _systems.handle( p_uid );
		_systems.get( h ).atomFlags.assign( p_atomFlags.begin(), p_atomFlags.end() );
		_dirty.markAtomSelection( h );

		setNeedUpdate( true );
	}

	void Renderer::setVoxels( std::span<const Vec3f> p_mins, std::span<const Vec3f> p_maxs )
	{
		assert( p_mins.size() == p_maxs.size() );

		_geometries.grid.setVoxelCount( p_mins.size() );
		_layouts.voxels.resizeStorage( _context, _geometries.grid.voxelCount() );

		_layouts.voxels.upload<Layout::VOXEL_ATTR::MINS, Vec3f>( _context, Desc::NO_HANDLE, p_mins );
		_layouts.voxels.upload<Layout::VOXEL_ATTR::MAXS, Vec3f>( _context, Desc::NO_HANDLE, p_maxs );

		_dirty.markDrawRanges();

		setNeedUpdate( true );
	}

#pragma endregion

#pragma region Internals

	bool Renderer::_refreshGraph( const GraphicsConfig & p_config )
	{
		Util::ScopedChrono timer( "[RENDERER] _refreshGraph" );

		Builder::PipelineConfig config;

		config.enableSSAO	   = p_config.activeSSAO;
		config.enableOutline   = p_config.activeOutline;
		config.enableSelection = p_config.activeSelection;

		if ( _config && *_config == config )
		{
			return false;
		}

		_config = config;
		_graph.set( Builder::DefaultRenderGraph::build( config, _layouts, _geometries ) );
		_queue = _graph.build();
		return true;
	}

#pragma endregion
} // namespace VTX::Renderer
