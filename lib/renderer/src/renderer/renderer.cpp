#include "renderer/renderer.hpp"
#include "renderer/binary_buffer.hpp"
#include "renderer/builder/render_graph_build.hpp"
#include "renderer/builder/system_build.hpp"
#include <unordered_map>
#include <util/chrono.hpp>
#include <util/enum.hpp>

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
			Builder::RenderGraphRuntime::rebuildCommandBuffer(
				_context,
				_queue,
				_graph.getResources(),
				reinterpret_cast<uintptr_t>( &Renderer::_executeSESExternalPass ),
				reinterpret_cast<uintptr_t>( this )
			);
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
			Builder::RenderGraphRuntime::rebuildCommandBuffer(
				_context,
				_queue,
				_graph.getResources(),
				reinterpret_cast<uintptr_t>( &Renderer::_executeSESExternalPass ),
				reinterpret_cast<uintptr_t>( this )
			);
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
		_dirtyRenderer = Cache::E_RENDERER_DIRTY::NONE;
		_dirtySystems.clear();
		_dirtyRepresentations.clear();
		_needUpdate = false;
	}

	bool Renderer::render( const float p_deltaTime, const float p_elapsedTime ) noexcept
	{
		if ( _dirtyRenderer != Cache::E_RENDERER_DIRTY::NONE || not _dirtySystems.empty()
			 || not _dirtyRepresentations.empty() )
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
		Builder::RenderGraphRuntime::rebuildCommandBuffer(
			_context,
			_queue,
			_graph.getResources(),
			reinterpret_cast<uintptr_t>( &Renderer::_executeSESExternalPass ),
			reinterpret_cast<uintptr_t>( this )
		);
		_render( 0.f, 0.f );

		// TODO: get last pass instead of hardcoding FXAA.
		std::vector<std::byte> data = _context.getTextureData( "FXAA", Desc::E_FORMAT::RGBA8UI );

		_context.setRenderTarget( Desc::E_RENDER_TARGET::SCREEN );
		Builder::RenderGraphRuntime::rebuildCommandBuffer(
			_context,
			_queue,
			_graph.getResources(),
			reinterpret_cast<uintptr_t>( &Renderer::_executeSESExternalPass ),
			reinterpret_cast<uintptr_t>( this )
		);
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

	void Renderer::_flushDirty()
	{
		using RendererDirty = Cache::E_RENDERER_DIRTY;
		using SystemDirty	= Cache::E_SYSTEM_DIRTY;

		auto hasDirty = []( const auto p_flags, const auto p_bit ) { return Util::Enum::hasAnyBit( p_flags, p_bit ); };

		std::unordered_map<Desc::Handle, SystemDirty> dirtySystems;
		for ( const DirtySystem & dirty : _dirtySystems )
		{
			if ( not _systems.contains( dirty.handle ) )
			{
				continue;
			}

			dirtySystems[ dirty.handle ] |= dirty.flags;
		}

		const bool fullRefresh = hasDirty( _dirtyRenderer, RendererDirty::ALL );
		if ( fullRefresh )
		{
			const auto handles = _systems.handles();
			dirtyGeometrySystems.insert( handles.begin(), handles.end() );

			Builder::AtomLayout::allocateInputs( _context, _layouts );
			Builder::ResidueLayout::allocateInputs( _context, _layouts );

			for ( const Desc::Handle system : handles )
			{
				Builder::AtomLayout::uploadInput( _context, _systems, _layouts, system, dirtyGeometrySystems );
				Builder::SystemVisibility::uploadInput( _systems, system, dirtyGeometrySystems );
				Builder::ResidueLayout::uploadInput( _context, _layouts, _geometries, system, _systems.get( system ) );
			}

			Builder::GeometryBuffers::allocateOutputs( _context, _geometries );

			for ( const Desc::Handle system : handles )
			{
				Builder::RibbonGeometry::writeOutput( _context, _geometries, system );
				Builder::GeometryBuffers::writeOutput( _context, _geometries, system );
			}

			Builder::SystemModels::upload( _context, _systems, _camera );
		}

		bool updateModels		  = fullRefresh;
		bool updateDrawRanges	  = fullRefresh || hasDirty( _dirtyRenderer, RendererDirty::DRAW_RANGES );
		bool updateGeometryChunks = fullRefresh || hasDirty( _dirtyRenderer, RendererDirty::GEOMETRY_CHUNKS );
		bool updateExternalPasses = fullRefresh || hasDirty( _dirtyRenderer, RendererDirty::EXTERNAL_PASSES );
		bool updateCommandBuffer  = fullRefresh || hasDirty( _dirtyRenderer, RendererDirty::COMMAND_BUFFER );

		if ( not fullRefresh )
		{
			for ( const auto & [ system, flags ] : dirtySystems )
			{
				const Cache::System & cache = _systems.get( system );

				if ( hasDirty( flags, SystemDirty::TRANSFORM ) )
				{
					updateModels = true;
				}
				if ( hasDirty( flags, SystemDirty::TRAJECTORY ) )
				{
					Builder::AtomLayout::uploadPositions( _context, _systems, _layouts, system, cache.trajectory );
					Builder::RibbonGeometry::uploadPositions(
						_context, _layouts, _geometries, system, cache.trajectory
					);
					dirtyGeometrySystems.insert( system );
				}
				if ( hasDirty( flags, SystemDirty::COLOR ) )
				{
					Builder::AtomLayout::refreshColors( _context, _systems, _layouts, system );
				}
				if ( hasDirty( flags, SystemDirty::REPRESENTATION ) )
				{
					// TODO: resolve Cache::System preset entities to renderer representation handles.
					dirtyGeometrySystems.insert( system );
				}
				if ( hasDirty( flags, SystemDirty::VISIBILITY ) )
				{
					Builder::SystemVisibility::uploadVisibility( _systems, dirtyGeometrySystems, system );
					dirtyGeometrySystems.insert( system );
				}
				if ( hasDirty( flags, SystemDirty::SELECTION ) )
				{
					const std::vector<Flag> atomFlags = Builder::AtomLayout::buildAtomFlags( cache );
					Builder::AtomLayout::uploadSelection( _context, _layouts, system, atomFlags );
					Builder::ResidueLayout::uploadSelection( _context, _layouts, _geometries, system, atomFlags );
					dirtyGeometrySystems.insert( system );
				}
			}
		}

		if ( updateModels )
		{
			Builder::SystemModels::upload( _context, _systems, _camera );
		}

		for ( const auto & system : dirtyGeometrySystems )
		{
			Builder::SystemVisibility::refreshGeometryVisibility(
				_context, _systems, _representations, _layouts, _geometries, system
			);
		}
		for ( const auto & system : dirtyGeometrySystems )
		{
			_geometries.uploadIndexes( _context, system );
		}

		if ( not dirtyGeometrySystems.empty() )
		{
			updateDrawRanges	 = true;
			updateGeometryChunks = true;
			updateExternalPasses = true;
		}

		if ( updateDrawRanges )
		{
			Builder::DrawRanges::buildDrawRanges( _context, _geometries );
		}
		if ( updateGeometryChunks && Builder::RenderGraphRuntime::syncGeometryChunks( _graph, _geometries ) )
		{
			updateCommandBuffer = true;
		}
		if ( updateCommandBuffer )
		{
			Builder::RenderGraphRuntime::rebuildCommandBuffer(
				_context,
				_queue,
				_graph.getResources(),
				reinterpret_cast<uintptr_t>( &Renderer::_executeSESExternalPass ),
				reinterpret_cast<uintptr_t>( this )
			);
		}
		if ( updateExternalPasses )
		{
			Builder::RenderGraphRuntime::markSESDirty( _context, _geometries );
		}

		_dirtyRenderer = RendererDirty::NONE;
		_dirtySystems.clear();
		_dirtyRepresentations.clear();
		setNeedUpdate( true );
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

		_camera = { p_camera, p_position, p_matView, p_matProj };

		Builder::SystemModels::upload( _context, _systems, _camera );

		setNeedUpdate( true );
	}

	void Renderer::setGraphicsConfig( const GraphicsConfig & p_config )
	{
		Util::ScopedChrono timer( "[RENDERER] setGraphicsConfig" );

		// If graph changed from config, rebuild backend.
		if ( Builder::RenderGraphRuntime::refreshGraph( p_config, _config, _graph, _queue, _layouts, _geometries ) )
		{
			Builder::RenderGraphRuntime::rebuildCommandBuffer(
				_context,
				_queue,
				_graph.getResources(),
				reinterpret_cast<uintptr_t>( &Renderer::_executeSESExternalPass ),
				reinterpret_cast<uintptr_t>( this )
			);
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

	Desc::Handle Renderer::addRepresentation( const Representation & p_representation )
	{
		//
		return 0;
	}

	void Renderer::removeRepresentation( const Desc::Handle )
	{
		//
	}

#pragma endregion

	Desc::Handle Renderer::addSystem( Cache::System && p_system )
	{
		Util::ScopedChrono timer( "[RENDERER] addSystem" );

		const Desc::Handle handle
			= Builder::SystemRegistry::registerSystem( _systems, _geometries, _layouts, std::move( p_system ) );
		_dirtyRenderer = Cache::E_RENDERER_DIRTY::ALL;
		_dirtySystems.emplace_back( handle, Cache::E_SYSTEM_DIRTY::ALL );

		setNeedUpdate( true );
		return handle;
	}

	void Renderer::removeSystem( const Desc::Handle p_handle )
	{
		Util::ScopedChrono timer( "[RENDERER] removeSystem" );

		if ( not _systems.contains( p_handle ) )
		{
			return;
		}

		_systems.erase( p_handle );

		std::vector<Cache::System> remainingSystems;
		const auto				   remainingHandles = _systems.handles();
		remainingSystems.reserve( remainingHandles.size() );
		for ( const Desc::Handle handle : remainingHandles )
		{
			remainingSystems.emplace_back( _systems.get( handle ) );
		}

		std::unordered_set<Desc::Handle> dirtyGeometrySystems;
		Builder::SystemRegistry::clear( _systems, _layouts, _geometries, dirtyGeometrySystems );

		for ( Cache::System & system : remainingSystems )
		{
			Builder::SystemRegistry::registerSystem( _systems, _geometries, _layouts, std::move( system ) );
		}

		_dirtyRenderer = Cache::E_RENDERER_DIRTY::ALL;
		_dirtySystems.clear();
		for ( const Desc::Handle handle : _systems.handles() )
		{
			_dirtySystems.emplace_back( handle, Cache::E_SYSTEM_DIRTY::ALL );
		}

		setNeedUpdate( true );
	}

	bool Renderer::ensureBufferChunk( const Desc::BufferRef & p_ref )
	{
		const bool created = _context.ensureBufferChunk( p_ref );
		if ( not created )
		{
			return false;
		}

		Builder::RenderGraphRuntime::rebuildCommandBuffer(
			_context,
			_queue,
			_graph.getResources(),
			reinterpret_cast<uintptr_t>( &Renderer::_executeSESExternalPass ),
			reinterpret_cast<uintptr_t>( this )
		);
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

		Builder::RenderGraphRuntime::rebuildCommandBuffer(
			_context,
			_queue,
			_graph.getResources(),
			reinterpret_cast<uintptr_t>( &Renderer::_executeSESExternalPass ),
			reinterpret_cast<uintptr_t>( this )
		);
		setNeedUpdate( true );

		return true;
	}

	void Renderer::setSystemTransform( const Desc::Handle p_handle, const Mat4f & p_transform )
	{
		Util::ScopedChrono timer( "[RENDERER] setSystemTransform" );

		assert( _systems.contains( p_handle ) );
		_systems.get( p_handle ).transform = p_transform;
		_dirtySystems.emplace_back( p_handle, Cache::E_SYSTEM_DIRTY::TRANSFORM );
		setNeedUpdate( true );
	}

	void Renderer::setSystemDirty( const Desc::Handle p_handle, const Cache::E_SYSTEM_DIRTY p_flags )
	{
		assert( _systems.contains( p_handle ) );
		_dirtySystems.emplace_back( p_handle, p_flags );
		setNeedUpdate( true );
	}

	void Renderer::setVoxels( std::span<const Vec3f> p_mins, std::span<const Vec3f> p_maxs )
	{
		assert( p_mins.size() == p_maxs.size() );

		_geometries.grid.setVoxelCount( p_mins.size() );
		_layouts.voxels.resizeStorage( _context, _geometries.grid.voxelCount() );

		_layouts.voxels.upload<Layout::VOXEL_ATTR::MINS, Vec3f>( _context, Desc::NO_HANDLE, p_mins );
		_layouts.voxels.upload<Layout::VOXEL_ATTR::MAXS, Vec3f>( _context, Desc::NO_HANDLE, p_maxs );

		_dirtyRenderer = Cache::E_RENDERER_DIRTY::DRAW_RANGES;

		setNeedUpdate( true );
	}

} // namespace VTX::Renderer
