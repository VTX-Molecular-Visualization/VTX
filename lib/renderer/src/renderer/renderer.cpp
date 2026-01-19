#include "renderer/renderer.hpp"
#include "renderer/binary_buffer.hpp"
#include <execution>
#include <util/chrono.hpp>
#include <util/math.hpp>
#include <util/math/aabb.hpp>
#include <util/math/grid.hpp>
#include <util/math/range.hpp>
#include <util/string.hpp>

namespace VTX::Renderer
{
	Renderer::Renderer( const size_t p_width, const size_t p_height ) : _width( p_width ), _height( p_height )
	{
		_refreshGraph( GraphicsConfigs::DEFAULT );
	}

#pragma region Contexts

	void Renderer::setDefault()
	{
		_context.setNull();
		try
		{
			build();
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
			build();
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

	void Renderer::build()
	{
		// Build renderer _graph.
		float buildTime = Util::CHRONO_CPU(
			[ this ]()
			{
				const Desc::RenderQueue queue = _graph.build();
				_context.build( queue, _graph.getResources() );
				std::cout << _context.commands() << std::endl;
			}
		);

		VTX_DEBUG( "Renderer graph setup total time: {}", Util::String::durationToStr( buildTime ) );
	}

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

		_matrixView = p_matView;
		_refreshDataModels();

		setNeedUpdate( true );
	}

	void Renderer::setGraphicsConfig( const GraphicsConfig & p_config )
	{
		_refreshGraph( p_config );
		build();

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

			BinaryBuffer140 bufferBlur;
			bufferBlur.write( p_config.blurSize );
			bufferBlur.close();
			_context.setShaderBuffer( "BlurX", bufferBlur );
			_context.setShaderBuffer( "BlurY", bufferBlur );
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
		BinaryBuffer<E_LAYOUT_TYPE::Std140> buffer;
		buffer.write( p_representation.radiusSphereFixed );
		buffer.write( p_representation.radiusSphereAdd );
		buffer.write( uint( p_representation.isRadiusSphereFixed ) );
		buffer.write( p_representation.radiusCylinder );
		buffer.write( uint( p_representation.cylinderColorBlending ) );
		buffer.write( uint( p_representation.ribbonColorBlending ) );
		buffer.write( p_representation.sesProbeRadius );
		buffer.close();

		_context.setShaderBuffer( "Representations", buffer );

		setNeedUpdate( true );

		// Aply logic.
		_geometries.spheres.show   = p_representation.hasSphere;
		_geometries.cylinders.show = p_representation.hasCylinder;
		_geometries.ribbons.show   = p_representation.hasRibbon;
		_geometries.ses.show	   = p_representation.hasSes;

		// Asked SES, hide all others.
		if ( _geometries.ses.show )
		{
			_geometries.spheres.show   = false;
			_geometries.cylinders.show = false;
			_geometries.ribbons.show   = false;
			return;
		}

		const bool	isSphereRadiusFixed = p_representation.isRadiusSphereFixed;
		const float cylinderRadius		= p_representation.radiusCylinder;
		const float sphereRadiusFixed	= p_representation.radiusSphereFixed;
		const float sphereRadiusAdd		= p_representation.radiusSphereAdd;

		// Hide ribbon if VdW radius.
		if ( not isSphereRadiusFixed )
		{
			_geometries.cylinders.show = false;
			_geometries.ribbons.show   = false;
			return;
		}

		// If B&S.
		if ( _geometries.spheres.show && _geometries.cylinders.show )
		{
			// Scale sphere radius to cylinder radius if not VdW.
			if ( isSphereRadiusFixed && sphereRadiusFixed < cylinderRadius )
			{
				setValue( cylinderRadius, "RepresentationsSphereRadiusFixed" );
			}
		}

		// If sticks only, force sphere at cylinder radius.
		else if ( not _geometries.spheres.show && _geometries.cylinders.show )
		{
			_geometries.spheres.show = true;
			setValue( uint( true ), "RepresentationsIsSphereRadiusFixed" );
			setValue( cylinderRadius, "RepresentationsSphereRadiusFixed" );
		}
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

	void Renderer::addSystem(
		const RootUID				 p_appId,
		const Mat4f &				 p_transform,
		const Core::Struct::System & p_data,
		std::span<const PickingUID>	 p_uid
	)
	{
		_context.setPipelineBuffer<Vec3f>( "Atoms.Positions", p_data.trajectory.getCurrentFrame() );

		std::vector<float> radii;
		radii.resize( p_data.atomNames.size(), 1.f );
		_context.setPipelineBuffer<float>( "Atoms.Radii", radii );

		_context.setPipelineBuffer<PickingUID>( "Atoms.Ids", p_uid );

		std::vector<uchar> flags;
		flags.resize( p_data.atomNames.size(), 0 );
		_context.setPipelineBuffer<uchar>( "Atoms.Flags", flags );

		std::vector<ushort> models;
		models.resize( p_data.atomNames.size(), 0 );
		_context.setPipelineBuffer<ushort>( "Atoms.Models", models );

		_context.setPipelineBuffer<Index>( "Bonds", p_data.bondPairAtomIndexes );

		_transform = p_transform;

		_refreshDataModels();

		_geometries.spheres.drawRanges.firsts = { 0 };
		_geometries.spheres.drawRanges.counts = { uint( p_data.atomNames.size() ) };

		//_geometries.cylinders.drawRanges.offsets = { 0 };
		//_geometries.cylinders.drawRanges.counts	 = { uint( p_data.bondPairAtomIndexes.size() ) };
	}

#pragma endregion

	void Renderer::_refreshDataModels()
	{
		BinaryBuffer430 buffer;

		const Mat4f matrixModelView	   = _matrixView * _transform;
		const Mat4f matrixModelViewInv = Util::Math::inverse( matrixModelView );
		const Mat4f matrixNormal	   = Util::Math::transpose( matrixModelViewInv );

		buffer.write( matrixModelView );
		buffer.write( matrixModelViewInv );
		buffer.write( matrixNormal );
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

	const StructInfos & Renderer::getInfos( const bool p_refresh )
	{
		if ( not p_refresh )
		{
			return _infos;
		}

		_context.fillInfos( _infos );

		return _infos;
	}

	void Renderer::_refreshGraph( const GraphicsConfig & p_config )
	{
		RenderGraph::PipelineConfig config;

		config.enableSSAO	   = p_config.activeSSAO;
		config.enableOutline   = p_config.activeOutline;
		config.enableSelection = p_config.activeSelection;

		_graph.createDefaultPipeline( config, _geometries );
	}
} // namespace VTX::Renderer
