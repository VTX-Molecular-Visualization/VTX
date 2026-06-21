#include "renderer/renderer.hpp"
#include "renderer/builder/mesh_build.hpp"
#include "renderer/builder/model_build.hpp"
#include "renderer/builder/render_graph_build.hpp"
#include "renderer/builder/system_build.hpp"
#include <unordered_map>
#include <util/chrono.hpp>
#include <util/enum.hpp>

namespace VTX::Renderer
{
	Renderer::Renderer( const size_t p_width, const size_t p_height ) : _width( p_width ), _height( p_height ) {}

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
			_dirtyRenderer |= Cache::E_RENDERER_DIRTY::ENVIRONMENT;
			_context.fillInfos( _infos );
			onReady( _infos );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "{}", p_e.what() );
			VTX_ERROR( "Can not build openGL 4.6 backend" );
			setDefault();
		}
	}

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
		_dirtyRenderer |= Cache::E_RENDERER_DIRTY::COMMAND_BUFFER | Cache::E_RENDERER_DIRTY::CAMERA
						  | Cache::E_RENDERER_DIRTY::NEED_UPDATE;
	}

	void Renderer::clear()
	{
		_context.clear();
		_graph.clear();
		_voxels		   = {};
		_dirtyRenderer = Cache::E_RENDERER_DIRTY::NONE;
		_dirtySystems.clear();
		_dirtyRepresentations.clear();
	}

	bool Renderer::render( const float p_deltaTime, const float p_elapsedTime ) noexcept
	{
		bool hasDirty = _dirtyRenderer != Cache::E_RENDERER_DIRTY::NONE || not _dirtySystems.empty()
						|| not _dirtyRepresentations.empty();

		if ( hasDirty )
		{
			try
			{
				_flushDirty();
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Renderer update failed: {}", p_e.what() );
				setDefault();
				_dirtyRenderer = Cache::E_RENDERER_DIRTY::NONE;
				_dirtySystems.clear();
				_dirtyRepresentations.clear();
				return false;
			}
			catch ( ... )
			{
				VTX_ERROR( "Unknown renderer update failure" );
				setDefault();
				_dirtyRenderer = Cache::E_RENDERER_DIRTY::NONE;
				_dirtySystems.clear();
				_dirtyRepresentations.clear();
				return false;
			}
		}

		if ( hasDirty || _forceUpdate )
		{
			_render( p_deltaTime, p_elapsedTime );
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

		// TODO: get last pass instead of hardcoding post-process output.
		const Desc::Key		   outputTexture = _graphicsConfig.data.crt ? "CRT" : "FXAA";
		std::vector<std::byte> data			 = _context.getTextureData( outputTexture, Desc::E_FORMAT::RGBA8UI );

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
		using RendererDirty		  = Cache::E_RENDERER_DIRTY;
		using RepresentationDirty = Cache::E_REPRESENTATION_DIRTY;
		using SystemDirty		  = Cache::E_SYSTEM_DIRTY;

		auto hasDirty = []( const auto p_flags, const auto p_bit ) { return Util::Enum::hasAnyBit( p_flags, p_bit ); };

		std::unordered_map<Desc::Handle, SystemDirty>		  dirtySystems;
		std::unordered_map<Desc::Handle, RepresentationDirty> dirtyRepresentations;
		std::unordered_set<Desc::Handle>					  geometryRefreshSystems;
		std::vector<Desc::Handle>							  deletingSystems;
		for ( const DirtySystem & dirty : _dirtySystems )
		{
			if ( not _systems.contains( dirty.handle ) )
			{
				continue;
			}

			dirtySystems[ dirty.handle ] |= dirty.flags;
		}

		for ( const auto & [ system, flags ] : dirtySystems )
		{
			if ( hasDirty( flags, SystemDirty::DELETING ) )
			{
				deletingSystems.emplace_back( system );
			}
		}

		if ( not deletingSystems.empty() )
		{
			for ( const Desc::Handle system : deletingSystems )
			{
				Builder::SystemRegistry::removeSystemConstruction( _context, _geometries, system );
				_systems.erase( system );
				dirtySystems.erase( system );
			}

			_dirtyRenderer |= RendererDirty::ALL;
		}
		for ( const DirtyRepresentation & dirty : _dirtyRepresentations )
		{
			if ( not _representations.contains( dirty.handle ) )
			{
				continue;
			}

			dirtyRepresentations[ dirty.handle ] |= dirty.flags;
		}

		for ( const auto & [ system, flags ] : dirtySystems )
		{
			if ( hasDirty( flags, SystemDirty::STRUCTURE ) )
			{
				_dirtyRenderer |= RendererDirty::ALL;
				break;
			}
		}

		const bool fullRefresh				   = Util::Enum::hasAllBits( _dirtyRenderer, RendererDirty::ALL );
		bool	   geometryVisibilityRefreshed = false;
		bool	   geometryIndexesUploaded	   = false;
		if ( fullRefresh )
		{
			Builder::SystemRegistry::clearSystemRanges( _layouts, _geometries, geometryRefreshSystems );
			Builder::MeshRegistry::clear( _layouts, _geometries );

			for ( const auto entry : _systems.entries() )
			{
				Builder::SystemRegistry::registerSystem( _systems, _geometries, _layouts, entry.handle );
				geometryRefreshSystems.insert( entry.handle );
			}
			for ( const auto entry : _meshes.entries() )
			{
				Builder::MeshRegistry::registerMesh( _meshes, _geometries, _layouts, entry.handle );
			}
		}

		bool graphChanged = false;
		if ( fullRefresh || hasDirty( _dirtyRenderer, RendererDirty::GRAPH ) )
		{
			graphChanged = Builder::RenderGraphRuntime::refreshGraph(
				_graphicsConfig.data, _config, _graph, _queue, _layouts, _geometries
			);
			if ( graphChanged )
			{
				Builder::RenderGraphRuntime::rebuildCommandBuffer(
					_context,
					_queue,
					_graph.getResources(),
					reinterpret_cast<uintptr_t>( &Renderer::_executeSESExternalPass ),
					reinterpret_cast<uintptr_t>( this )
				);
			}
		}
		const bool updateGraphicsConfig
			= fullRefresh || graphChanged || hasDirty( _dirtyRenderer, RendererDirty::GRAPHICS_CONFIG );
		const bool updateColorLayout
			= fullRefresh || graphChanged || hasDirty( _dirtyRenderer, RendererDirty::COLOR_LAYOUT );
		if ( fullRefresh )
		{
			dirtyRepresentations.clear();
			for ( const auto entry : _representations.entries() )
			{
				dirtyRepresentations.emplace( entry.handle, RepresentationDirty::ALL );
			}
		}
		const bool updateRepresentations = fullRefresh || hasDirty( _dirtyRenderer, RendererDirty::REPRESENTATIONS )
										   || not dirtyRepresentations.empty();

		if ( not dirtyRepresentations.empty() )
		{
			for ( const auto & [ representation, flags ] : dirtyRepresentations )
			{
				Builder::RepresentationState::refreshCache( _representations.get( representation ) );
			}
		}

		if ( updateRepresentations )
		{
			for ( const auto entry : _systems.entries() )
			{
				dirtySystems[ entry.handle ] |= SystemDirty::REPRESENTATION;
			}
		}
		if ( fullRefresh )
		{
			Builder::AtomLayout::allocateInputs( _context, _layouts );

			for ( const auto entry : _systems.entries() )
			{
				const Desc::Handle system = entry.handle;
				Builder::AtomLayout::uploadInput( _context, _systems, _layouts, system, geometryRefreshSystems );
				Builder::SystemVisibility::uploadInput( _systems, system, geometryRefreshSystems );
			}

			for ( const auto & system : geometryRefreshSystems )
			{
				Builder::SystemVisibility::refreshGeometryVisibility(
					_context, _systems, _representations, _layouts, _geometries, system
				);
			}
			geometryVisibilityRefreshed = true;

			Builder::ResidueLayout::allocateInputs( _context, _layouts );

			for ( const auto entry : _systems.entries() )
			{
				const Desc::Handle system = entry.handle;
				Builder::ResidueLayout::uploadInput( _context, _layouts, _geometries, system, _systems.get( system ) );
				Builder::RibbonGeometry::uploadPositions(
					_context, _layouts, _geometries, system, _systems.get( system ).data.trajectory
				);
			}

			Builder::GeometryBuffers::allocateOutputs( _context, _geometries );

			for ( const auto entry : _systems.entries() )
			{
				const Desc::Handle system = entry.handle;
				Builder::RibbonGeometry::writeOutput( _context, _geometries, system );
				Builder::GeometryBuffers::writeOutput( _context, _geometries, system );
			}

			for ( const auto & system : geometryRefreshSystems )
			{
				_geometries.uploadIndexes( _context, system );
			}
			geometryIndexesUploaded = true;

			Builder::MeshRegistry::upload( _context, _meshes, _geometries, _layouts );

			Builder::Models::upload( _context, _camera, _systems, _meshes );
		}

		bool updateModels		  = fullRefresh;
		bool updateDrawRanges	  = fullRefresh || hasDirty( _dirtyRenderer, RendererDirty::DRAW_RANGES );
		bool updateGeometryChunks = fullRefresh || hasDirty( _dirtyRenderer, RendererDirty::GEOMETRY_CHUNKS );
		bool updateExternalPasses = fullRefresh || hasDirty( _dirtyRenderer, RendererDirty::EXTERNAL_PASSES );
		bool updateCommandBuffer  = fullRefresh || hasDirty( _dirtyRenderer, RendererDirty::COMMAND_BUFFER );
		if ( fullRefresh || hasDirty( _dirtyRenderer, RendererDirty::VOXELS ) )
		{
			_geometries.grid.setVoxelCount( _voxels.mins.size() );
			_layouts.voxels.resizeStorage( _context, _geometries.grid.voxelCount() );

			_layouts.voxels.upload<Layout::VOXEL_ATTR::MINS, Vec3f>(
				_context, Desc::NO_HANDLE, std::span<const Vec3f> { _voxels.mins }
			);
			_layouts.voxels.upload<Layout::VOXEL_ATTR::MAXS, Vec3f>(
				_context, Desc::NO_HANDLE, std::span<const Vec3f> { _voxels.maxs }
			);

			updateDrawRanges = true;
		}
		if ( fullRefresh || hasDirty( _dirtyRenderer, RendererDirty::CAMERA ) )
		{
			Builder::CameraState::upload( _context, _camera, width(), height() );
			updateModels = true;
		}
		if ( hasDirty( _dirtyRenderer, RendererDirty::MODELS ) )
		{
			updateModels = true;
		}

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
					Builder::AtomLayout::uploadPositions( _context, _systems, _layouts, system, cache.data.trajectory );
					Builder::RibbonGeometry::uploadPositions(
						_context, _layouts, _geometries, system, cache.data.trajectory
					);
					if ( _geometries.ses.built( system ) )
					{
						_geometries.ses.invalidateForRecompute( _context, system );
						geometryRefreshSystems.insert( system );
					}
				}
				if ( hasDirty( flags, SystemDirty::COLOR ) )
				{
					Builder::AtomLayout::refreshColors( _context, _systems, _layouts, system );
				}
				if ( hasDirty( flags, SystemDirty::REPRESENTATION ) )
				{
					const std::vector<RepresentationIndex> atomRepresentations
						= Builder::AtomLayout::buildAtomRepresentations( cache );
					Builder::AtomLayout::uploadRepresentations(
						_context, _layouts, system, atomRepresentations, geometryRefreshSystems
					);
					Builder::ResidueLayout::uploadRepresentations(
						_context, _layouts, _geometries, system, atomRepresentations
					);
					geometryRefreshSystems.insert( system );
				}
				if ( hasDirty( flags, SystemDirty::VISIBILITY ) )
				{
					Builder::SystemVisibility::uploadVisibility( _systems, geometryRefreshSystems, system );
					geometryRefreshSystems.insert( system );
				}
				if ( hasDirty( flags, SystemDirty::SELECTION ) )
				{
					const std::vector<Flag> atomFlags = Builder::AtomLayout::buildAtomFlags( cache );
					Builder::AtomLayout::uploadSelection( _context, _layouts, system, atomFlags );
					Builder::ResidueLayout::uploadSelection( _context, _layouts, _geometries, system, atomFlags );
					geometryRefreshSystems.insert( system );
				}
			}
		}

		if ( updateModels )
		{
			Builder::Models::upload( _context, _camera, _systems, _meshes );
		}

		if ( not geometryVisibilityRefreshed )
		{
			for ( const auto & system : geometryRefreshSystems )
			{
				Builder::SystemVisibility::refreshGeometryVisibility(
					_context, _systems, _representations, _layouts, _geometries, system
				);
			}
		}
		if ( not geometryIndexesUploaded )
		{
			if ( not geometryRefreshSystems.empty() )
			{
				Builder::ResidueLayout::allocateInputs( _context, _layouts );

				for ( const auto entry : _systems.entries() )
				{
					const Desc::Handle system = entry.handle;
					Builder::ResidueLayout::uploadInput(
						_context, _layouts, _geometries, system, _systems.get( system )
					);
					Builder::RibbonGeometry::uploadPositions(
						_context, _layouts, _geometries, system, _systems.get( system ).data.trajectory
					);
				}

				Builder::GeometryBuffers::allocateOutputs( _context, _geometries );

				for ( const auto entry : _systems.entries() )
				{
					const Desc::Handle system = entry.handle;
					Builder::RibbonGeometry::writeOutput( _context, _geometries, system );
					Builder::GeometryBuffers::writeOutput( _context, _geometries, system );
					_geometries.uploadIndexes( _context, system );
				}
			}
		}

		if ( not geometryRefreshSystems.empty() )
		{
			updateDrawRanges	 = true;
			updateGeometryChunks = true;
			updateExternalPasses = true;
		}

		if ( updateDrawRanges )
		{
			_geometries.buildDrawRanges( _context, _systems, _meshes );
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
		if ( hasDirty( _dirtyRenderer, RendererDirty::ENVIRONMENT ) && _graphicsConfig.data.shading.environmentPath )
		{
			Builder::PostProcess::Shading::loadEnvironment( _context, _graphicsConfig.data.shading );
		}
		if ( updateRepresentations )
		{
			Builder::RepresentationState::upload( _context, _representations );
		}
		if ( updateGraphicsConfig )
		{
			Builder::GraphicsConfigState::upload( _context, _graphicsConfig.data );
		}
		if ( updateColorLayout )
		{
			Builder::ColorLayoutState::upload( _context, _colorLayout.data );
		}
		if ( updateExternalPasses )
		{
			Builder::RenderGraphRuntime::markSESDirty( _context, _geometries );
		}

		_dirtyRenderer = RendererDirty::NONE;
		_dirtySystems.clear();
		_dirtyRepresentations.clear();
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

	void Renderer::setCamera(
		const Camera & p_camera,
		const Vec3f &  p_position,
		const Mat4f &  p_matView,
		const Mat4f &  p_matProj
	)
	{
		_camera = { p_camera, p_position, p_matView, p_matProj };
		_dirtyRenderer |= Cache::E_RENDERER_DIRTY::CAMERA;
	}

	void Renderer::setGraphicsConfig( const GraphicsConfig & p_config )
	{
		const ShadingConfig & currentShading = _graphicsConfig.data.shading;
		const bool environmentChanged = currentShading.environmentPath != p_config.shading.environmentPath
										|| currentShading.environmentFaceSize != p_config.shading.environmentFaceSize;
		const Builder::PipelineConfig pipelineConfig = Builder::RenderGraphRuntime::pipelineConfig( p_config );
		const bool					  graphReady	 = _config.has_value();
		const bool					  graphChanged	 = not graphReady || *_config != pipelineConfig;

		_graphicsConfig.data = p_config;
		_dirtyRenderer |= Cache::E_RENDERER_DIRTY::GRAPHICS_CONFIG;
		if ( graphChanged )
		{
			_dirtyRenderer |= Cache::E_RENDERER_DIRTY::GRAPH | Cache::E_RENDERER_DIRTY::COMMAND_BUFFER;
		}
		if ( environmentChanged )
		{
			_dirtyRenderer |= Cache::E_RENDERER_DIRTY::ENVIRONMENT;
		}
	}

	void Renderer::setColorLayout( const Color::Layout & p_layout )
	{
		_colorLayout.data = p_layout;
		_dirtyRenderer |= Cache::E_RENDERER_DIRTY::COLOR_LAYOUT;
	}

	Desc::Handle Renderer::addRepresentation( const Representation & p_representation )
	{
		const Desc::Handle handle
			= _representations.emplace( Builder::RepresentationState::buildCache( p_representation ) );

		_dirtyRenderer |= Cache::E_RENDERER_DIRTY::REPRESENTATIONS;

		return handle;
	}

	void Renderer::patchRepresentation( const Desc::Handle p_handle, Cache::Representation::Data && p_representation )
	{
		assert( _representations.contains( p_handle ) );

		_representations.get( p_handle ).data = std::move( p_representation );
	}

	void Renderer::removeRepresentation( const Desc::Handle p_handle )
	{
		assert( _representations.contains( p_handle ) );

		_representations.erase( p_handle );

		_dirtyRenderer |= Cache::E_RENDERER_DIRTY::REPRESENTATIONS;
	}

	void Renderer::setRepresentationDirty( const Desc::Handle p_handle, const Cache::E_REPRESENTATION_DIRTY p_flags )
	{
		assert( _representations.contains( p_handle ) );

		_dirtyRepresentations.emplace_back( p_handle, p_flags );
	}

	Desc::Handle Renderer::addSystem( Cache::System && p_system )
	{
		const Desc::Handle handle = _systems.emplace( std::move( p_system ) );
		Builder::SystemRegistry::registerSystem( _systems, _geometries, _layouts, handle );

		_dirtyRenderer |= Cache::E_RENDERER_DIRTY::ALL;
		_dirtySystems.emplace_back( handle, Cache::E_SYSTEM_DIRTY::ALL );

		return handle;
	}

	void Renderer::patchSystem( const Desc::Handle p_handle, Cache::System::Data && p_system )
	{
		assert( _systems.contains( p_handle ) );

		_systems.get( p_handle ).data = std::move( p_system );
	}

	void Renderer::removeSystem( const Desc::Handle p_handle )
	{
		assert( _systems.contains( p_handle ) );

		_dirtySystems.emplace_back( p_handle, Cache::E_SYSTEM_DIRTY::DELETING );
	}

	Desc::Handle Renderer::addMesh( Cache::Mesh && p_mesh )
	{
		const Desc::Handle handle = _meshes.emplace( std::move( p_mesh ) );
		_dirtyRenderer |= Cache::E_RENDERER_DIRTY::ALL;
		return handle;
	}

	void Renderer::patchMesh( const Desc::Handle p_handle, const Core::Struct::Mesh & p_mesh )
	{
		assert( _meshes.contains( p_handle ) );
		_meshes.get( p_handle ).data = &p_mesh;
	}

	void Renderer::removeMesh( const Desc::Handle p_handle )
	{
		assert( _meshes.contains( p_handle ) );
		_meshes.erase( p_handle );
		_dirtyRenderer |= Cache::E_RENDERER_DIRTY::ALL;
	}

	void Renderer::setMeshTransform( const Desc::Handle p_handle, const Mat4f & p_transform )
	{
		assert( _meshes.contains( p_handle ) );
		_meshes.get( p_handle ).transform = p_transform;
		_dirtyRenderer |= Cache::E_RENDERER_DIRTY::MODELS;
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

		return true;
	}

	void Renderer::setSystemTransform( const Desc::Handle p_handle, const Mat4f & p_transform )
	{
		assert( _systems.contains( p_handle ) );

		_systems.get( p_handle ).transform = p_transform;
		_dirtySystems.emplace_back( p_handle, Cache::E_SYSTEM_DIRTY::TRANSFORM );
	}

	void Renderer::setSystemPositions( const Desc::Handle p_handle, std::span<const Vec3f> p_positions )
	{
		assert( _systems.contains( p_handle ) );

		_systems.get( p_handle ).data.trajectory = p_positions;
		_dirtySystems.emplace_back( p_handle, Cache::E_SYSTEM_DIRTY::TRAJECTORY );
	}

	void Renderer::setSystemDirty( const Desc::Handle p_handle, const Cache::E_SYSTEM_DIRTY p_flags )
	{
		assert( _systems.contains( p_handle ) );

		_dirtySystems.emplace_back( p_handle, p_flags );
	}

	void Renderer::setVoxels( std::span<const Vec3f> p_mins, std::span<const Vec3f> p_maxs )
	{
		assert( p_mins.size() == p_maxs.size() );

		_voxels.mins.assign( p_mins.begin(), p_mins.end() );
		_voxels.maxs.assign( p_maxs.begin(), p_maxs.end() );
		_dirtyRenderer |= Cache::E_RENDERER_DIRTY::VOXELS;
	}

} // namespace VTX::Renderer
