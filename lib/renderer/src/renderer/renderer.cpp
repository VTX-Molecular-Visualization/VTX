#include "renderer/renderer.hpp"
#include "renderer/binary_buffer.hpp"
#include <util/chrono.hpp>

namespace
{
	using namespace VTX;
	using namespace VTX::Renderer;

	constexpr Flag _toFlag( std::byte p_b ) { return static_cast<Flag>( std::to_integer<uint8_t>( p_b ) & 1u ); }
} // namespace

namespace VTX::Renderer
{
	Renderer::Renderer( const size_t p_width, const size_t p_height ) : _width( p_width ), _height( p_height ) {}

#pragma region Contexts

	void Renderer::setDefault()
	{
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

	void Renderer::setOpenGL45( const FilePath & p_shaderIncludePath )
	{
		_context.setOpenGL45( _width, _height, p_shaderIncludePath );
		try
		{
			_context.build( _queue, _graph.getResources() );
			std::cout << _context.commands() << std::endl;
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

#pragma region Build & render

	void Renderer::resize( const size_t p_width, const size_t p_height )
	{
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
		_needUpdate		 = false;
		_framesRemaining = 0;
	}

	void Renderer::render( const float p_deltaTime, const float p_elapsedTime ) noexcept
	{
		if ( _needUpdate || forceUpdate || _framesRemaining > 0 )
		{
			_render( p_deltaTime, p_elapsedTime );

			if ( not forceUpdate )
			{
				if ( _needUpdate )
				{
					setNeedUpdate( false );
				}
				else
				{
					_framesRemaining--;
				}
			}
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
		// If graph changed from config, rebuild backend.
		if ( _refreshGraph( p_config ) )
		{
			_context.build( _queue, _graph.getResources() );
			std::cout << _context.commands() << std::endl;
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
		_context.setShaderBuffer<Util::Color::Rgba>( "ColorLayout", p_layout.colors );

		setNeedUpdate( true );
	}

	void Renderer::setRepresentation( const Representation & p_representation )
	{
		// Aply logic.
		bool showSphere	  = p_representation.hasSphere;
		bool showCylinder = p_representation.hasCylinder;
		bool showRibbon	  = p_representation.hasRibbon;
		bool showSes	  = p_representation.hasSes;

		// Asked SES, hide all others.
		if ( showSes )
		{
			showSphere	 = false;
			showCylinder = false;
			showRibbon	 = false;
		}
		else
		{
			bool		isSphereRadiusFixed = p_representation.isRadiusSphereFixed;
			const float cylinderRadius		= p_representation.radiusCylinder;
			float		sphereRadiusFixed	= p_representation.radiusSphereFixed;

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

			BinaryBuffer<E_LAYOUT_TYPE::Std140> buffer;
			buffer.write( sphereRadiusFixed );
			buffer.write( p_representation.radiusSphereAdd );
			buffer.write( uint( isSphereRadiusFixed ) );
			buffer.write( p_representation.radiusCylinder );
			buffer.write( uint( p_representation.cylinderColorBlending ) );
			buffer.write( uint( p_representation.ribbonColorBlending ) );
			buffer.write( p_representation.sesProbeRadius );
			buffer.close();

			_context.setShaderBuffer( "Representations", buffer );

			setNeedUpdate( true );
		}

		/*
		if ( showSphere && _geometries.spheres.rangeList.sizeRange() )
		{
			_geometries.spheres.drawRanges.firsts = { 0 };
			_geometries.spheres.drawRanges.counts = { uint32_t( _geometries.spheres.rangeList.getLast() ) };
		}
		else
		{
			_geometries.spheres.drawRanges = {};
		}

		if ( showCylinder && _geometries.cylinders.rangeList.sizeRange() )
		{
			_geometries.cylinders.drawRanges.offsets = { 0 };
			_geometries.cylinders.drawRanges.counts	 = { uint32_t( _geometries.cylinders.rangeList.getLast() ) };
		}
		else
		{
			_geometries.cylinders.drawRanges = {};
		}
		*/
	}

#pragma endregion

#pragma region Geometries

	void Renderer::setVoxels( const std::vector<Vec3f> & p_mins, const std::vector<Vec3f> & p_maxs )
	{
		assert( p_mins.size() == p_maxs.size() );

		_context.setPipelineBuffer<Vec3f>( "Voxels.Mins", p_mins );
		_context.setPipelineBuffer<Vec3f>( "Voxels.Maxs", p_maxs );

		_geometries.voxels.drawRanges.firsts = { 0 };
		_geometries.voxels.drawRanges.counts = { uint( p_mins.size() ) };

		setNeedUpdate( true );
	}

	void Renderer::setSystems( const std::vector<SystemData> & p_systems )
	{
		Util::Chrono timer;
		timer.start();

		// Compute total size and check integrity.
		size_t totalAtoms = 0;
		size_t totalBonds = 0;
		for ( const auto & systemData : p_systems )
		{
			const size_t countAtoms = systemData.frame.size();
			assert( systemData.atomUids.size() == countAtoms );
			assert( systemData.radii.size() == countAtoms );
			assert( systemData.colorIndexes.size() == countAtoms );
			assert( systemData.representationIndexes.size() == countAtoms );
			assert( systemData.visibleAtoms.size() == countAtoms );
			assert( systemData.selectedAtoms.size() == countAtoms );
			totalAtoms += countAtoms;
			totalBonds += systemData.data.bondPairAtomIndexes.size();
		}

		// Check.
		assert( totalAtoms > 0 );
		assert( totalBonds > 0 );

		if ( totalAtoms > TypeMax<Index> )
		{
			throw GraphicException( "Total atom count exceeds maximum supported value." );
		}
		if ( totalBonds > TypeMax<Index> )
		{
			throw GraphicException( "Total bond count exceeds maximum supported value." );
		}

		// Reserve data.
		_context.setPipelineBuffer<Vec3f>( "Atoms.Positions", totalAtoms );
		_context.setPipelineBuffer<Index>( "Bonds", totalBonds );
		_context.setPipelineBuffer<float>( "Atoms.Radii", totalAtoms );
		_context.setPipelineBuffer<PickingUID>( "Atoms.Ids", totalAtoms );
		_context.setPipelineBuffer<ColorIndex>( "Atoms.Colors", totalAtoms );
		_context.setPipelineBuffer<RepresentationIndex>( "Atoms.Representations", totalAtoms );
		_context.setPipelineBuffer<ModelIndex>( "Atoms.Models", totalAtoms );
		_context.setPipelineBuffer<Flag>( "Atoms.Flags", totalAtoms );

		_cacheSystems.clear();

		ModelIndex modelIndex  = 0;
		size_t	   offsetAtoms = 0;
		size_t	   offsetBonds = 0;
		for ( const auto & systemData : p_systems )
		{
			const size_t  countAtoms = systemData.frame.size();
			const size_t  countBonds = systemData.data.bondPairAtomIndexes.size();
			const RootUID uid		 = systemData.uid;

			// Move bonds.
			auto bonds = systemData.data.bondPairAtomIndexes;
			for ( auto & bondIndex : bonds )
			{
				bondIndex += static_cast<Index>( offsetAtoms );
			}

			// Upload data.
			_context.setPipelineBuffer<Vec3f>( "Atoms.Positions", systemData.frame, offsetAtoms );
			_context.setPipelineBuffer<Index>( "Bonds", bonds, offsetBonds );
			_context.setPipelineBuffer<float>( "Atoms.Radii", systemData.radii, offsetAtoms );
			_context.setPipelineBuffer<PickingUID>( "Atoms.Ids", systemData.atomUids, offsetAtoms );
			_context.setPipelineBuffer<ColorIndex>( "Atoms.Colors", systemData.colorIndexes, offsetAtoms );
			_context.setPipelineBuffer<RepresentationIndex>(
				"Atoms.Representations", systemData.representationIndexes, offsetAtoms
			);
			_context.setPipelineBuffer<ModelIndex>(
				"Atoms.Models", std::vector<ModelIndex>( countAtoms, modelIndex ), offsetAtoms
			);

			std::vector<Flag> flags( countAtoms );
			for ( size_t i = 0; i < countAtoms; ++i )
			{
				Flag flag = 0;
				flag |= _toFlag( systemData.visibleAtoms[ i ] ) << toUnderlying( E_ELEMENT_FLAGS::VISIBILITY );
				flag |= _toFlag( systemData.selectedAtoms[ i ] ) << toUnderlying( E_ELEMENT_FLAGS::SELECTION );
				flags[ i ] = flag;
			}
			_context.setPipelineBuffer<Flag>( "Atoms.Flags", flags, offsetAtoms );

			// Cache.
			_cacheSystems[ uid ] = Caches::System { systemData.transform, modelIndex };

			// Geometry ranges.
			_geometries.spheres.ranges[ uid ] = Geometry::IndexRange::fromFirstCount(
				static_cast<Index>( offsetAtoms ), static_cast<Index>( countAtoms )
			);
			_geometries.cylinders.ranges[ uid ] = Geometry::IndexRange::fromFirstCount(
				static_cast<Index>( offsetBonds ), static_cast<Index>( countBonds )
			);

			offsetAtoms += countAtoms;
			offsetBonds += countBonds;
			modelIndex++;
		}

		// Set models.
		_refreshDataModels();

		// Build draw ranges.
		_geometries.spheres.buildDrawRanges();
		_geometries.cylinders.buildDrawRanges();

		VTX_INFO( "Systems GPU upload: {} ms", timer.elapsedTime() );
	}
	void Renderer::setSystemPosition( const RootUID p_appId, std::span<const Vec3f> p_positions )
	{
		_context.setPipelineBuffer<Vec3f>( "Atoms.Positions", p_positions );
	}

#pragma endregion

	void Renderer::_refreshDataModels()
	{
		if ( _cacheSystems.empty() )
		{
			return;
		}

		BinaryBuffer430 buffer;

		// Sort.
		std::vector<Mat4f> transforms( _cacheSystems.size() );
		for ( const auto & [ _, cacheSystem ] : _cacheSystems )
		{
			transforms[ cacheSystem.modelIndex ] = cacheSystem.transform;
		}

		for ( const auto & transform : transforms )
		{
			const Mat4f matrixModelView	   = _cacheCamera.matView * transform;
			const Mat4f matrixModelViewInv = Util::Math::inverse( matrixModelView );
			const Mat4f matrixNormal	   = Util::Math::transpose( matrixModelViewInv );

			buffer.write( matrixModelView );
			buffer.write( matrixModelViewInv );
			buffer.write( matrixNormal );
		}

		buffer.close();

		_context.setShaderBuffer( "Models", buffer );
	}

	void Renderer::snapshot(
		std::vector<uchar> & p_outImage,
		const size_t		 p_width,
		const size_t		 p_height,
		const float			 p_fov,
		const float			 p_near,
		const float			 p_far
	)
	{
		/*
		const Mat4f & matrixProjectionOld = *_proxyCamera->matrixProjection;
		Mat4f		  matrixProjection	  = Util::Math::perspective(
			   Util::Math::radians( p_fov ), float( p_width ) / float( p_height ), p_near, p_far
		   );
		setValue( matrixProjection, "CameraMatrixProjection" );
		_context.snapshot( p_outImage, _graph.getRenderQueue(), _instructions, p_width, p_height );
		setValue( matrixProjectionOld, "CameraMatrixProjection" );
		*/
	}

	Vec2i Renderer::getPickedIds( const size_t p_x, const size_t p_y ) const
	{
		std::vector<std::byte> data = _context.getTextureData( "Picking", p_x, _height - p_y );

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

	bool Renderer::_refreshGraph( const GraphicsConfig & p_config )
	{
		RenderGraph::PipelineConfig config;

		config.enableSSAO	   = p_config.activeSSAO;
		config.enableOutline   = p_config.activeOutline;
		config.enableSelection = p_config.activeSelection;

		auto & configOpt = _graph.getPipelineConfig();
		if ( configOpt && *configOpt == config )
		{
			return false;
		}

		_graph.createDefaultPipeline( config, _geometries );
		_queue = _graph.build();
		return true;
	}
} // namespace VTX::Renderer
