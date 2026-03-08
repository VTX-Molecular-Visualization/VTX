#include "renderer/renderer.hpp"
#include "renderer/binary_buffer.hpp"
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
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "{}", p_e.what() );
			VTX_ERROR( "Can not build default backend" );
		}
	}

	void Renderer::setOpenGL( const FilePath & p_shaderIncludePath )
	{
		Util::ScopedChrono timer( "[RENDERER] setOpenGL45" );

		_context.setOpenGL( _width, _height, p_shaderIncludePath );
		try
		{
			_context.build( _queue, _graph.getResources() );
			_context.fillInfos( _infos );
			onReady();
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "{}", p_e.what() );
			VTX_ERROR( "Can not build openGL 4.5 backend" );
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
			for ( const auto & system : _systemToRefresh )
			{
				_refreshSystemVisibility( system );
			}
			_geometries.buildDrawRanges( _context );
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
		_render( 0.f, 0.f );

		std::vector<std::byte> data = _context.getTextureData( "FXAA", Desc::E_FORMAT::RGBA8UI );

		_context.setRenderTarget( Desc::E_RENDER_TARGET::SCREEN );
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

		_context.setShaderBuffer( "Camera", buffer );

		_cacheCamera = { p_camera, p_position, p_matView, p_matProj };

		_refreshDataModels();

		setNeedUpdate( true );
	}

	void Renderer::setGraphicsConfig( const GraphicsConfig & p_config )
	{
		Util::ScopedChrono timer( "[RENDERER] setGraphicsConfig" );

		// If graph changed from config, rebuild backend.
		if ( _refreshGraph( p_config ) )
		{
			_context.build( _queue, _graph.getResources() );
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
		_context.setShaderBuffer( "Shading", bufferShading );

		if ( p_config.activeSSAO )
		{
			BinaryBuffer140 bufferSSAO;
			bufferSSAO.write( p_config.ssaoIntensity );
			bufferSSAO.close();
			_context.setShaderBuffer( "SSAO", bufferSSAO );

			BinaryBuffer140 bufferBlurX;
			bufferBlurX.write( Vec2i( 1, 0 ) );
			bufferBlurX.write( p_config.blurSize );
			bufferBlurX.close();
			_context.setShaderBuffer( "BlurX", bufferBlurX );
			BinaryBuffer140 bufferBlurY;
			bufferBlurY.write( Vec2i( 0, 1 ) );
			bufferBlurY.write( p_config.blurSize );
			bufferBlurY.close();
			_context.setShaderBuffer( "BlurY", bufferBlurY );
		}
		if ( p_config.activeOutline )
		{
			BinaryBuffer140 bufferOutline;
			bufferOutline.write( p_config.colorOutline );
			bufferOutline.write( p_config.outlineSensitivity );
			bufferOutline.write( p_config.outlineThickness );
			bufferOutline.close();
			_context.setShaderBuffer( "Outline", bufferOutline );
		}
		if ( p_config.activeSelection )
		{
			BinaryBuffer140 bufferSelection;
			bufferSelection.write( p_config.colorSelection );
			bufferSelection.close();
			_context.setShaderBuffer( "Selection", bufferSelection );
		}

		setNeedUpdate( true );
	}

	void Renderer::setColorLayout( const Color::Layout & p_layout )
	{
		Util::ScopedChrono timer( "[RENDERER] setColorLayout" );

		_context.setShaderBuffer<Util::Color::Rgba>( "ColorLayout", p_layout.colors );

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
				// Hide ribbon if VdW radius.
				if ( not isSphereRadiusFixed )
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

			// Cache.
			_cacheRepresentations[ index ] = Cache::Representation { showSphere, showCylinder, showRibbon, showSes };

			index++;
		}

		buffer.close();

		_context.setShaderBuffer( "Representations", buffer );

		auto handles	 = _systems.handles();
		_systemToRefresh = std::unordered_set<Desc::Handle>( handles.begin(), handles.end() );

		setNeedUpdate( true );
	}

#pragma endregion

#pragma region Geometries

	void Renderer::setSystems( const std::vector<SystemData> & p_systems )
	{
		Util::ScopedChrono timer( "[RENDERER] setSystems" );

		// Register new systems.
		for ( const auto & systemData : p_systems )
		{
			if ( not _systems.contains( systemData.uid ) )
			{
				_systems.emplace( systemData.uid, {}, Cache::System { systemData.transform } );
				const Desc::Handle h = _systems.handle( systemData.uid );

				// Compute geometries.
				_geometries.construct( h, systemData );

				// Register ranges in layouts.
				_layouts.atoms.add( h, _geometries.spheres.size( h ) );
				_layouts.residues.add(
					h, static_cast<uint32_t>( _geometries.ribbons.construction( h ).residues.size() )
				);
			}
		}

		// Reserve data.
		_layouts.resize( _context );
		_geometries.resize( _context );

		for ( const auto & systemData : p_systems )
		{
			using namespace Layout;

			const Desc::Handle h = _systems.handle( systemData.uid );

			// Upload layouts and geometries data.
			_layouts.atoms.upload<ATOM_ATTR::SYMBOL, Symbol>( _context, h, systemData.data.atomSymbols );
			_layouts.atoms.upload<ATOM_ATTR::ID, PickingUID>( _context, h, systemData.atomUids );

			_geometries.spheres.uploadIndexes( _context, h );
			_geometries.cylinders.uploadIndexes( _context, h );

			if ( not _geometries.ribbons.empty( h ) )
			{
				const auto &			construction  = _geometries.ribbons.construction( h );
				const Index				countResidues = _layouts.residues.size( h );
				std::vector<PickingUID> residueIds( countResidues );
				std::vector<uint8_t>	residueTypes( countResidues );
				for ( Index i = 0; i < countResidues; ++i )
				{
					const Index residueIndex = construction.residues[ i ].index;
					residueIds[ i ]			 = systemData.residueUids[ residueIndex ];
					residueTypes[ i ] = toUnderlying( systemData.data.residueSecondaryStructureTypes[ residueIndex ] );
				}

				_layouts.residues.upload<RESIDUE_ATTR::ID, PickingUID>( _context, h, residueIds );
				_layouts.residues.upload<RESIDUE_ATTR::TYPE, uint8_t>( _context, h, residueTypes );
				_geometries.ribbons.uploadIndexes( _context, h );
			}

			setSystemPosition( systemData.uid, systemData.trajectory );
		}

		// Set models.
		_refreshDataModels();

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

		// TODO: refresh only needed!
		_refreshDataModels();
		setNeedUpdate( true );
	}

	void Renderer::setSystemPosition( const SystemUID p_uid, std::span<const Vec3f> p_positions )
	{
		assert( _systems.contains( p_uid ) );

		const Desc::Handle h = _systems.handle( p_uid );

		// Push atom positions.
		_layouts.atoms.upload<Layout::ATOM_ATTR::POSITION, Vec3f>( _context, h, p_positions );

		// Compute ribbon positions and directions.
		const auto & construction = _geometries.ribbons.construction( h );
		if ( not construction.isEmpty )
		{
			const Index		   countResidues = _layouts.residues.size( h );
			std::vector<Vec4f> ribbonPositions( countResidues );
			std::vector<Vec3f> ribbonDirections( countResidues );

			for ( Index i = 0; i < countResidues; ++i )
			{
				// Compute direction between carbon alpha and oxygen.

				const Vec3f & positionCA   = p_positions[ construction.residues[ i ].ca ];
				const Vec3f & positionO	   = p_positions[ construction.residues[ i ].o ];
				const Vec3f	  directionCAO = Util::Math::normalize( positionO - positionCA );

				ribbonPositions[ i ]  = Vec4f( positionCA, 1.f );
				ribbonDirections[ i ] = directionCAO;

				// TODO: better on GPU ?
				// CheckOrientationAndFlip.
				// size_t i;
				// for ( i = 1; i < p_caODirections.size(); ++i )
				//{
				//	if ( Util::Math::dot( p_caODirections[ i ], p_caODirections[ i - 1 ] ) < 0.f )
				//	{
				//		p_caODirections[ i ] = -p_caODirections[ i ];
				//	}
				// }
			}

			_layouts.residues.upload<Layout::RESIDUE_ATTR::POSITION, Vec4f>( _context, h, ribbonPositions );
			_layouts.residues.upload<Layout::RESIDUE_ATTR::DIRECTION, Vec3f>( _context, h, ribbonDirections );
		}

		setNeedUpdate( true );
	}

	void Renderer::setSystemColors( const SystemUID p_uid, std::span<const ColorIndex> p_colors )
	{
		assert( _systems.contains( p_uid ) );

		const Desc::Handle h = _systems.handle( p_uid );
		_layouts.atoms.upload<Layout::ATOM_ATTR::COLOR, ColorIndex>( _context, h, p_colors );

		const auto &			construction  = _geometries.ribbons.construction( h );
		const Index				countResidues = _layouts.residues.size( h );
		std::vector<ColorIndex> ribbonColors( countResidues );
		for ( Index i = 0; i < countResidues; ++i )
		{
			ribbonColors[ i ] = p_colors[ construction.residues[ i ].ca ];
		}

		_layouts.residues.upload<Layout::RESIDUE_ATTR::COLOR, ColorIndex>( _context, h, ribbonColors );

		setNeedUpdate( true );
	}

	void Renderer::setSystemRepresentation( const SystemUID p_uid, const MapRepresentationRanges & p_representations )
	{
		assert( _systems.contains( p_uid ) );

		Util::ScopedChrono timer( "[RENDERER] setSystemRepresentation" );

		const Desc::Handle h		   = _systems.handle( p_uid );
		Cache::System &	   systemCache = _systems.get( p_uid );

		systemCache.representations = p_representations;

		const Index						 countAtoms = _layouts.atoms.size( h );
		std::vector<RepresentationIndex> atoms( countAtoms );
		Index							 count = 0;

		for ( const auto & [ index, ranges ] : p_representations )
		{
			// Atoms.
			for ( auto it = ranges.rangeBegin(); it != ranges.rangeEnd(); ++it )
			{
				std::fill_n( atoms.begin() + it->getFirst(), it->getCount(), index );
			}
			count += ranges.count();
		}

		assert( count == countAtoms );

		_layouts.atoms.upload<Layout::ATOM_ATTR::REPRESENTATION, RepresentationIndex>( _context, h, atoms );

		/*
		// Residues.
		const auto &					 construction  = _geometries.ribbons.construction( h );
		const Index						 countResidues = _layouts.residues.size( h );
		std::vector<RepresentationIndex> ribbonItems( countResidues );
		systemCache.representationResiduesRanges.clear();

		for ( Index i = 0; i < countResidues; ++i )
		{
			const RepresentationIndex representationIndex = atoms[ construction.residues[ i ].ca ];
			ribbonItems[ i ]							  = atoms[ representationIndex ];
			systemCache.representationResiduesRanges[ representationIndex ].addRange(
				IndexRange::fromFirstCount( ribbonItems[ i ], 1 )
			);
		}

		assert( count == countAtoms );

		_layouts.residues.upload<Layout::RESIDUE_ATTR::REPRESENTATION, RepresentationIndex>(
			_context, h, ribbonItems
		);
		*/

		_systemToRefresh.insert( h );

		setNeedUpdate( true );
	}

	void Renderer::setSystemVisibility(
		const SystemUID						 p_uid,
		const Core::Struct::IndexRangeList & p_visibility

	)
	{
		assert( _systems.contains( p_uid ) );

		Util::ScopedChrono timer( "[RENDERER] setSystemVisibility" );

		const Desc::Handle h		   = _systems.handle( p_uid );
		Cache::System &	   systemCache = _systems.get( p_uid );

		systemCache.visibility = p_visibility;

		_systemToRefresh.insert( h );

		setNeedUpdate( true );
	}

	void Renderer::setSystemSelection(
		const SystemUID						 p_uid,
		const Core::Struct::IndexRangeList & p_selection

	)
	{
		assert( _systems.contains( p_uid ) );

		Util::ScopedChrono timer( "[RENDERER] setSystemSelection" );

		const Desc::Handle h		   = _systems.handle( p_uid );
		Cache::System &	   systemCache = _systems.get( p_uid );
		const Index		   countAtoms  = _layouts.atoms.size( h );

		assert( p_selection.size() <= countAtoms );

		systemCache.selection = p_selection;

		static constexpr Flag SEL = 1 << toUnderlying( E_ELEMENT_FLAGS::SELECTION );

		std::vector<Flag> atomFlags( countAtoms, 0 );

		auto applyOr
			= [ & ]( std::vector<Flag> & p_flags, const Core::Struct::IndexRangeList & p_ranges, const Flag p_mask )
		{
			for ( auto it = p_ranges.rangeBegin(); it != p_ranges.rangeEnd(); ++it )
			{
				const Index begin = it->first;
				const Index end	  = it->last;

				assert( end <= countAtoms );

				for ( size_t i = begin; i < end; ++i )
				{
					p_flags[ i ] |= p_mask;
				}
			}
		};

		applyOr( atomFlags, p_selection, SEL );

		_layouts.atoms.upload<Layout::ATOM_ATTR::FLAG, Flag>( _context, h, atomFlags );

		// Build draw ranges.
		/*
		IndexRange rangeAtoms						= { 0, static_cast<Index>( countAtoms ) };
		_geometries.spheres.visibilityMask[ p_uid ] = { rangeAtoms };
		_geometries.spheres.visibilityMask[ p_uid ].substractInPlace( p_visible );

		const size_t countBonds = _geometries.cylinders.size( p_uid );

		assert( p_bonds.size() == countBonds );

		IndexRange rangeBonds = { 0, static_cast<Index>( countBonds ) };

		Core::Struct::IndexRangeList bondsVisible;
		for ( Index i = 0; i < p_bonds.size(); i += 2 )
		{
			if ( p_visible.contains( p_bonds[ i ] ) && p_visible.contains( p_bonds[ i + 1 ] ) )
			{
				bondsVisible.addRange( IndexRange::fromFirstCount( i, 2 ) );
			}
		}

		_geometries.cylinders.visibilityMask[ p_uid ] = { rangeBonds };
		_geometries.cylinders.visibilityMask[ p_uid ].substractInPlace( bondsVisible );
		*/

		_systemToRefresh.insert( h );

		setNeedUpdate( true );
	}

	void Renderer::setVoxels( const std::vector<Vec3f> & p_mins, const std::vector<Vec3f> & p_maxs )
	{
		assert( p_mins.size() == p_maxs.size() );

		_context.setPipelineBuffer<Vec3f>( "Voxels.Mins", p_mins );
		_context.setPipelineBuffer<Vec3f>( "Voxels.Maxs", p_maxs );

		//_geometries.voxels.drawRanges.firsts = { 0 };
		//_geometries.voxels.drawRanges.counts = { uint( p_mins.size() ) };

		setNeedUpdate( true );
	}

#pragma endregion

#pragma region Internals

	void Renderer::_refreshDataModels()
	{
		if ( _systems.empty() )
		{
			return;
		}

		BinaryBuffer430 buffer;
		for ( const auto & system : _systems )
		{
			const Mat4f matrixModelView	   = _cacheCamera.matView * system.transform;
			const Mat4f matrixModelViewInv = Util::Math::inverse( matrixModelView );
			const Mat4f matrixNormal	   = Util::Math::transpose( matrixModelViewInv );

			buffer.write( matrixModelView );
			buffer.write( matrixModelViewInv );
			buffer.write( matrixNormal );
		}

		buffer.close();

		_context.setShaderBuffer( "Models", buffer );
	}

	void Renderer::_refreshSystemVisibility( const Desc::Handle p_handle )
	{
		const Cache::System & systemCache = _systems.get( p_handle );

		auto visibleSpheres	  = systemCache.visibility;
		auto visibleCylinders = systemCache.visibility;

		for ( const auto & [ representationIndex, ranges ] : systemCache.representations )
		{
			assert( _cacheRepresentations.contains( representationIndex ) );

			const auto & cacheRepresentation = _cacheRepresentations[ representationIndex ];
			if ( not cacheRepresentation.showSphere )
			{
				visibleSpheres.substractInPlace( ranges );
			}
			if ( not cacheRepresentation.showCylinder )
			{
				visibleCylinders.substractInPlace( ranges );
			}
		}

		_geometries.spheres.setVisibility( p_handle, visibleSpheres );
		_geometries.cylinders.setVisibility( p_handle, visibleCylinders );

		_geometries.uploadIndexes( _context, p_handle );
	}

	/*
	void Renderer::_refreshDataRepresentations()
	{
		Util::ScopedChrono timer( "[RENDERER] _refreshDataRepresentations" );


		_geometries.spheres.representationMask.clear();
		_geometries.cylinders.representationMask.clear();

		for ( const auto & [ systemUid, cacheSystem ] : _cacheSystems )
		{
			// Sphere.
			for ( const auto & [ representationIndex, ranges ] : cacheSystem.representationAtomsRanges )
			{
				assert( _cacheRepresentations.contains( representationIndex ) );

				const auto & cacheRepresentation = _cacheRepresentations[ representationIndex ];
				if ( not cacheRepresentation.showSphere )
				{
					_geometries.spheres.representationMask[ systemUid ].mergeInPlace( ranges );
				}
			}

			// Cylinder.
			for ( const auto & [ representationIndex, ranges ] : cacheSystem.representationBondsRanges )
			{
				assert( _cacheRepresentations.contains( representationIndex ) );

				const auto & cacheRepresentation = _cacheRepresentations[ representationIndex ];
				if ( not cacheRepresentation.showCylinder )
				{
					_geometries.cylinders.representationMask[ systemUid ].mergeInPlace( ranges );
				}
			}
		}

	}
	*/

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
