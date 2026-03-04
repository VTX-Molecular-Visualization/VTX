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
		if ( _needBuildDrawRanges )
		{
			_refreshDataRepresentations();
			_geometries.buildDrawRanges( _context );
			_needBuildDrawRanges = false;
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

		_needBuildDrawRanges = true;

		setNeedUpdate( true );
	}

#pragma endregion

#pragma region Geometries

	void Renderer::setSystems( const std::vector<SystemData> & p_systems )
	{
		Util::ScopedChrono timer( "[RENDERER] setSystems" );

		for ( const auto & systemData : p_systems )
		{
			// Compute geometries.
			_geometries.construct( systemData );

			// Register ranges in layouts.
			//_layouts.atoms.add( systemData.uid, _geometries.spheres.range( systemData.uid ) );
			//_layouts.residues.add( systemData.uid, _geometries.ribbons.rangeItems( systemData.uid ) );
		}

		// Reserve data.
		//_layouts.resize( _context );

		const size_t totalAtoms			= _geometries.spheres.size;
		const size_t totalBonds			= _geometries.cylinders.size;
		const size_t totalRibbonItems	= _geometries.ribbons.sizeItems;
		const size_t totalRibbonIndices = _geometries.ribbons.size;

		// Reserve data.
		_context.setPipelineBuffer<Vec3f>( "Atoms.Positions", totalAtoms );
		_context.setPipelineBuffer<float>( "Atoms.Radii", totalAtoms );
		_context.setPipelineBuffer<PickingUID>( "Atoms.Ids", totalAtoms );
		_context.setPipelineBuffer<ColorIndex>( "Atoms.Colors", totalAtoms );
		_context.setPipelineBuffer<RepresentationIndex>( "Atoms.Representations", totalAtoms );
		_context.setPipelineBuffer<ModelIndex>( "Atoms.Models", totalAtoms );
		_context.setPipelineBuffer<Flag>( "Atoms.Flags", totalAtoms );
		_context.setPipelineBuffer<Vec4f>( "Residues.Positions", totalRibbonItems );
		_context.setPipelineBuffer<Vec3f>( "Residues.Directions", totalRibbonItems );
		_context.setPipelineBuffer<uint8_t>( "Residues.Types", totalRibbonItems );
		_context.setPipelineBuffer<ColorIndex>( "Residues.Colors", totalRibbonItems );
		_context.setPipelineBuffer<PickingUID>( "Residues.Ids", totalRibbonItems );
		_context.setPipelineBuffer<Flag>( "Residues.Flags", totalRibbonItems );
		_context.setPipelineBuffer<ModelIndex>( "Residues.Models", totalRibbonItems );
		_context.setPipelineBuffer<RepresentationIndex>( "Residues.Representations", totalRibbonItems );

		//_context.setPipelineBuffer<Index>( "Index.Atoms", totalAtoms );
		_context.setPipelineBuffer<Index>( "Index.Bonds", totalBonds );
		_context.setPipelineBuffer<Index>( "Index.Ribbons", totalRibbonIndices );

		_cacheSystems.clear();

		ModelIndex modelIndex		   = 0;
		size_t	   offsetAtoms		   = 0;
		size_t	   offsetBonds		   = 0;
		size_t	   offsetRibbonItems   = 0;
		size_t	   offsetRibbonIndices = 0;
		for ( const auto & systemData : p_systems )
		{
			const Index		countAtoms = systemData.data.getAtomCount();
			const Index		countBonds = systemData.data.getBondCount() * 2;
			const SystemUID uid		   = systemData.uid;

			// Upload data.
			_context.setPipelineBuffer<Index>( "Index.Bonds", systemData.data.bondPairAtomIndexes, offsetBonds );
			_context.setPipelineBuffer<float>( "Atoms.Radii", systemData.radii, offsetAtoms );
			_context.setPipelineBuffer<PickingUID>( "Atoms.Ids", systemData.atomUids, offsetAtoms );
			_context.setPipelineBuffer<ModelIndex>(
				"Atoms.Models", std::vector<ModelIndex>( countAtoms, modelIndex ), offsetAtoms
			);

			offsetAtoms += countAtoms;
			offsetBonds += countBonds;

			if ( not _geometries.ribbons.empty( uid ) )
			{
				const auto & construction		= _geometries.ribbons.construction( uid );
				const Index	 countRibbonItems	= static_cast<Index>( construction.residues.size() );
				const Index	 countRibbonIndices = static_cast<Index>( construction.indices.size() );

				std::vector<PickingUID> residueIds( countRibbonItems );
				std::vector<uint8_t>	residueTypes( countRibbonItems );
				for ( Index i = 0; i < countRibbonItems; ++i )
				{
					const Index residueIndex = _geometries.ribbons.construction( systemData.uid ).residues[ i ].index;
					residueIds[ i ]			 = systemData.residueUids[ residueIndex ];
					residueTypes[ i ] = toUnderlying( systemData.data.residueSecondaryStructureTypes[ residueIndex ] );
				}

				_context.setPipelineBuffer<Index>( "Index.Ribbons", construction.indices, offsetRibbonIndices );
				_context.setPipelineBuffer<PickingUID>( "Residues.Ids", residueIds, offsetRibbonItems );
				_context.setPipelineBuffer<uint8_t>( "Residues.Types", residueTypes, offsetRibbonItems );
				_context.setPipelineBuffer<ModelIndex>(
					"Residues.Models", std::vector<ModelIndex>( countRibbonItems, modelIndex ), offsetRibbonItems
				);

				offsetRibbonItems += countRibbonItems;
				offsetRibbonIndices += countRibbonIndices;
			}

			// Cache.
			_cacheSystems[ uid ] = Cache::System { systemData.transform, modelIndex };

			modelIndex++;
		}

		// Set models.
		_refreshDataModels();

		// Build draw ranges.
		_needBuildDrawRanges = true;

		setNeedUpdate( true );
	}

	void Renderer::setSystemTransform( const SystemUID p_uid, const Mat4f & p_transform )
	{
		Util::ScopedChrono timer( "[RENDERER] setSystemTransform" );

		assert( _cacheSystems.contains( p_uid ) );
		_cacheSystems[ p_uid ].transform = p_transform;

		// TODO: refresh only needed!
		_refreshDataModels();
		setNeedUpdate( true );
	}

	void Renderer::setSystemPosition( const SystemUID p_uid, std::span<const Vec3f> p_positions )
	{
		// Push atom positions.
		_context.setPipelineBuffer<Vec3f>( "Atoms.Positions", p_positions, _geometries.spheres.range( p_uid ).first );

		// Compute ribbon positions and directions.
		const auto & construction = _geometries.ribbons.construction( p_uid );
		if ( construction.isEmpty )
		{
			return;
		}

		const Index		   countRibbonItems = static_cast<Index>( construction.residues.size() );
		std::vector<Vec4f> ribbonPositions( countRibbonItems );
		std::vector<Vec3f> ribbonDirections( countRibbonItems );

		for ( Index i = 0; i < countRibbonItems; ++i )
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

		_context.setPipelineBuffer<Vec4f>(
			"Residues.Positions", ribbonPositions, _geometries.ribbons.rangeItems( p_uid ).first
		);
		_context.setPipelineBuffer<Vec3f>(
			"Residues.Directions", ribbonDirections, _geometries.ribbons.rangeItems( p_uid ).first
		);

		setNeedUpdate( true );
	}

	void Renderer::setSystemColors( const SystemUID p_uid, std::span<const ColorIndex> p_colors )
	{
		_context.setPipelineBuffer<ColorIndex>( "Atoms.Colors", p_colors, _geometries.spheres.range( p_uid ).first );

		if ( not _geometries.ribbons.empty( p_uid ) )
		{
			const auto &			construction	 = _geometries.ribbons.construction( p_uid );
			const Index				countRibbonItems = static_cast<Index>( construction.residues.size() );
			std::vector<ColorIndex> ribbonColors( countRibbonItems );
			for ( Index i = 0; i < countRibbonItems; ++i )
			{
				ribbonColors[ i ] = p_colors[ construction.residues[ i ].ca ];
			}
			_context.setPipelineBuffer<ColorIndex>(
				"Residues.Colors", ribbonColors, _geometries.ribbons.rangeItems( p_uid ).first
			);
		}

		setNeedUpdate( true );
	}

	void Renderer::setSystemRepresentation(
		const SystemUID					p_uid,
		const MapRepresentationRanges & p_representations,
		const std::vector<Index> &		p_bonds
	)
	{
		Util::ScopedChrono timer( "[RENDERER] setSystemRepresentation" );

		Cache::System & systemCache			  = _cacheSystems[ p_uid ];
		systemCache.representationAtomsRanges = p_representations;
		systemCache.representationBondsRanges.clear();
		const Index						 countAtoms = _geometries.spheres.range( p_uid ).getCount();
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

			// Bonds.
			for ( Index i = 0; i < p_bonds.size(); i += 2 )
			{
				if ( ranges.contains( p_bonds[ i ] ) && ranges.contains( p_bonds[ i + 1 ] ) )
				{
					systemCache.representationBondsRanges[ index ].addRange( IndexRange::fromFirstCount( i, 2 ) );
				}
			}
		}

		_context.setPipelineBuffer<RepresentationIndex>(
			"Atoms.Representations", atoms, _geometries.spheres.range( p_uid ).first
		);

		// Residues.
		if ( not _geometries.ribbons.empty( p_uid ) )
		{
			const auto &					 construction	  = _geometries.ribbons.construction( p_uid );
			const Index						 countRibbonItems = static_cast<Index>( construction.residues.size() );
			std::vector<RepresentationIndex> ribbonItems( countRibbonItems );
			systemCache.representationResiduesRanges.clear();

			for ( Index i = 0; i < countRibbonItems; ++i )
			{
				const RepresentationIndex representationIndex = atoms[ construction.residues[ i ].ca ];
				ribbonItems[ i ]							  = atoms[ representationIndex ];
				systemCache.representationResiduesRanges[ representationIndex ].addRange(
					IndexRange::fromFirstCount( ribbonItems[ i ], 1 )
				);
			}

			assert( count == countAtoms );

			_context.setPipelineBuffer<RepresentationIndex>(
				"Residues.Representations", ribbonItems, _geometries.ribbons.rangeItems( p_uid ).first
			);
		}

		_needBuildDrawRanges = true;
		setNeedUpdate( true );
	}

	void Renderer::setSystemFlags(
		const SystemUID						 p_uid,
		const Core::Struct::IndexRangeList & p_visible,
		const Core::Struct::IndexRangeList & p_selection,
		const std::vector<Index> &			 p_bonds

	)
	{
		Util::ScopedChrono timer( "[RENDERER] setSystemFlags" );

		const Index offsetAtoms = _geometries.spheres.range( p_uid ).first;
		const Index countAtoms	= _geometries.spheres.range( p_uid ).getCount();

		assert( p_selection.size() <= countAtoms );
		assert( p_visible.size() <= countAtoms );

		static constexpr Flag VIS = 1 << toUnderlying( E_ELEMENT_FLAGS::VISIBILITY );
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

		applyOr( atomFlags, p_visible, VIS );
		applyOr( atomFlags, p_selection, SEL );

		_context.setPipelineBuffer<Flag>( "Atoms.Flags", atomFlags, offsetAtoms );

		// Build draw ranges.
		IndexRange rangeAtoms						= { 0, static_cast<Index>( countAtoms ) };
		_geometries.spheres.visibilityMask[ p_uid ] = { rangeAtoms };
		_geometries.spheres.visibilityMask[ p_uid ].substractInPlace( p_visible );

		const size_t offsetBonds = _geometries.cylinders.range( p_uid ).first;
		const size_t countBonds	 = _geometries.cylinders.range( p_uid ).getCount();

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

		_needBuildDrawRanges = true;
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
		if ( _cacheSystems.empty() )
		{
			return;
		}

		BinaryBuffer430 buffer;

		// Sort.
		std::vector<Cache::System *> sorted( _cacheSystems.size() );
		for ( auto & [ _, cacheSystem ] : _cacheSystems )
		{
			sorted[ cacheSystem.modelIndex ] = &cacheSystem;
		}

		for ( const Cache::System * cacheSystem : sorted )
		{
			const Mat4f matrixModelView	   = _cacheCamera.matView * cacheSystem->transform;
			const Mat4f matrixModelViewInv = Util::Math::inverse( matrixModelView );
			const Mat4f matrixNormal	   = Util::Math::transpose( matrixModelViewInv );

			buffer.write( matrixModelView );
			buffer.write( matrixModelViewInv );
			buffer.write( matrixNormal );
		}

		buffer.close();

		_context.setShaderBuffer( "Models", buffer );
	}

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
