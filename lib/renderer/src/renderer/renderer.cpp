#include "renderer/renderer.hpp"
#include "renderer/binary_buffer.hpp"
#include "renderer/geometry/cylinder.hpp"
#include "renderer/geometry/ribbon.hpp"
#include "renderer/geometry/sphere.hpp"
#include "renderer/geometry/voxel.hpp"
#include <execution>
#include <util/math.hpp>
#include <util/math/aabb.hpp>
#include <util/math/grid.hpp>
#include <util/math/range.hpp>
#include <util/string.hpp>

namespace VTX::Renderer
{
	Renderer::Renderer( const size_t p_width, const size_t p_height ) : _width( p_width ), _height( p_height )
	{
		// Passes.
		_refreshGraph( GraphicsConfig() );
	}

	void Renderer::setDefault()
	{
		_context.setNull();
		build();
	}

	void Renderer::setOpenGL45( const FilePath & p_shaderIncludePath )
	{
		_context.setOpenGL45( _width, _height, p_shaderIncludePath );
		build();
	}

	void Renderer::build()
	{
		// Build renderer _graph.
		float buildTime = Util::CHRONO_CPU(
			[ this ]()
			{
				const RenderQueue queue = _graph.build();
				_context.build( queue, _graph.getResources() );
			}
		);

		VTX_DEBUG( "Renderer graph setup total time: {}", Util::String::durationToStr( buildTime ) );
	}

	void Renderer::resize( const size_t p_width, const size_t p_height )
	{
		VTX_TRACE( "Resizing renderer to {}x{}", p_width, p_height );

		_width	= p_width;
		_height = p_height;

		//_context.resize( _graph.getRenderQueue(), p_width, p_height );

		setNeedUpdate( true );
	}

	void Renderer::clear()
	{
		_context.clear();
		//_instructions.clear();
		//_instructionsDurationRanges.clear();
		_graph.clear();
		_needUpdate		 = false;
		_framesRemaining = 0;

		_proxiesSystems.clear();
		_proxyVoxels = nullptr;

		_cacheSpheresCylinders.clear();
		_cacheRibbons.clear();

		/*
		drawRangeSpheres.counts.clear();
		drawRangeSpheres.offsets.clear();
		drawRangeCylinders.counts.clear();
		drawRangeCylinders.offsets.clear();
		drawRangeRibbons.counts.clear();
		drawRangeRibbons.offsets.clear();
		*/

#ifdef VTX_CUDA_ENABLED
		_sesData.reset();
#endif
	}

	void Renderer::render( const float p_deltaTime, const float p_elapsedTime )
	{
		if ( _needUpdate || forceUpdate || _framesRemaining > 0 )
		{
			if ( logDurations )
			{
				_renderLog( p_deltaTime, p_elapsedTime );
			}
			else
			{
				_render( p_deltaTime, p_elapsedTime );
			}

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

#pragma region Proxy systems

	void Renderer::addProxySystem( Proxy::System & p_proxy )
	{
		_addProxySystem( p_proxy );
		_refreshDataSystems();

		if ( showSES )
		{
			_createSes( p_proxy );
		}
	}

	void Renderer::_createSes( Proxy::System & p_proxy )
	{
#ifdef VTX_CUDA_ENABLED
		std::vector<Vec4f> molecule( p_proxy.atomPositions->size() );
		Util::Math::AABB   aabb;
		// Fill molecule with atom positions and radius in the last component.
		for ( size_t i = 0; i < p_proxy.atomPositions->size(); ++i )
		{
			molecule[ i ] = Vec4f( ( *p_proxy.atomPositions )[ i ], p_proxy.atomRadii[ i ] );
			aabb.extend( ( *p_proxy.atomPositions )[ i ] );
		}
		bcs::Aabb aabbBCS( aabb.getMin(), aabb.getMax() );
		_sesData	= std::make_unique<bcs::Sesdf>( molecule, aabbBCS );
		_sesSurface = _sesData->getGraphics();
		VTX_DEBUG( "CUDA DONE" );

		_sesProgramConcave = _pm.createProgram( "SESConcave", "ses/sesdf/concave" );
		_sesProgramSegment = _pm.createProgram( "SESSegment", "ses/sesdf/segment" );
		_sesProgramCircle  = _pm.createProgram( "SESCircle", "ses/sesdf/circle" );
		_sesProgramConvex  = _pm.createProgram( "SESConvex", "ses/sesdf/convex" );

		glCreateVertexArrays( 1, &_sesVao );
		if ( _sesSurface.segmentPatches.size > 0 )
		{
			glCreateVertexArrays( 1, &_sesSegmentVao );

			glBindVertexArray( _sesSegmentVao );
			glBindBuffer( GL_ARRAY_BUFFER, _sesSurface.segmentPatches.handle );
			glEnableVertexAttribArray( 0 );

			const std::size_t offset = _sesSurface.segmentPatches.offset;
			glVertexAttribIPointer( 0, 4, GL_UNSIGNED_INT, sizeof( glm::uvec4 ), reinterpret_cast<void *>( offset ) );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
		}

		if ( _sesSurface.convexPatches.size > 0 )
		{
			glCreateVertexArrays( 1, &_sesCircleVao );
			glBindVertexArray( _sesCircleVao );

			glBindBuffer( GL_ARRAY_BUFFER, _sesSurface.circlePatches.handle );
			glEnableVertexAttribArray( 0 );

			const std::size_t offset = _sesSurface.circlePatches.offset;
			glVertexAttribIPointer( 0, 2, GL_UNSIGNED_INT, sizeof( glm::uvec2 ), reinterpret_cast<void *>( offset ) );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
		}

		if ( _sesSurface.convexPatches.size > 0 )
		{
			glCreateVertexArrays( 1, &_sesConvexVao );
			glBindVertexArray( _sesConvexVao );

			glBindBuffer( GL_ARRAY_BUFFER, _sesSurface.convexPatches.handle );
			glEnableVertexAttribArray( 0 );

			const std::size_t offset = _sesSurface.convexPatches.offset;
			glVertexAttribIPointer( 0, 2, GL_UNSIGNED_INT, sizeof( glm::uvec2 ), reinterpret_cast<void *>( offset ) );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
		}

		glBindVertexArray( 0 );

		uint sesMaxProbeNeighborNb = _sesData->getMaxNeighborPerAtom();
		setValue( sesMaxProbeNeighborNb, "RepresentationsSESMaxProbeNeighborNb", 0 );
		float sesProbeRadius = _proxyRepresentation->data.sesProbeRadius;
		setValue( sesProbeRadius, "RepresentationsSESProbeRadius", 0 );
#endif
	}

	void Renderer::removeProxySystem( Proxy::System & p_proxy )
	{
		VTX_INFO( "Remove proxy systems" );
		_removeProxySystem( p_proxy );
		_refreshDataSystems();
	}

	void Renderer::addProxySystems( std::vector<Proxy::System *> & p_proxies )
	{
		for ( Proxy::System * proxy : p_proxies )
		{
			_addProxySystem( *proxy );
		}
		_refreshDataSystems();
	}

	void Renderer::removeProxySystems( std::vector<Proxy::System *> & p_proxies )
	{
		for ( Proxy::System * proxy : p_proxies )
		{
			_removeProxySystem( *proxy );
		}
		_refreshDataSystems();
	}

	void Renderer::_addProxySystem( Proxy::System & p_proxy )
	{
		assert( p_proxy.idDefaultRepresentation == 0 );

		// If size max reached, do not add.
		if ( _proxiesSystems.size() >= TypeMax<RootUID> )
		{
			throw GraphicException( "Max system count reached" );
		}

		_proxiesSystems.push_back( &p_proxy );
		_cacheSpheresCylinders.emplace( &p_proxy, Cache::SphereCylinder() );
		_cacheRibbons.emplace( &p_proxy, Cache::Ribbon() );

		// Set up callbacks.
		// Transform.
		/*
		p_proxy.onTransform += [ this, &p_proxy ]()
		{
			const Mat4f matrixModelView	   = *_proxyCamera->matrixView * *p_proxy.transform;
			const Mat4f matrixModelViewInv = Util::Math::inverse( matrixModelView );
			const Mat4f matrixNormal	   = Util::Math::transpose( matrixModelViewInv );

			BinaryBuffer buffer;
			buffer.write( matrixModelView );
			buffer.write( matrixModelViewInv );
			buffer.write( matrixNormal );
			buffer.close();

			_context.setSub( buffer, "Models", _getProxyId( &p_proxy ) );
		};
		*/

		// Representation.
		p_proxy.onRepresentation += [ this, &p_proxy ]( const uchar p_representation )
		{
			Cache::SphereCylinder & cacheSC = _cacheSpheresCylinders[ &p_proxy ];
			Cache::Ribbon &			cacheR	= _cacheRibbons[ &p_proxy ];

			cacheSC.representations = std::vector<uchar>( cacheSC.rangeSpheres.getCount(), p_representation );
			cacheR.representations	= std::vector<uchar>( cacheR.range.getCount(), p_representation );

			/*
			_context.setSub(
				cacheSC.representations, "SpheresCylindersRepresentations", cacheSC.rangeSpheres.getFirst()
			);
			_context.setSub( cacheR.representations, "RibbonsRepresentations", cacheR.range.getFirst() );
			*/
		};

		// Remove.
		p_proxy.onRemove += [ this, &p_proxy ]() { removeProxySystem( p_proxy ); };

		// Positions.
		p_proxy.onAtomPositions += [ this, &p_proxy ]()
		{
			Cache::SphereCylinder & cacheSC = _cacheSpheresCylinders[ &p_proxy ];
			//_context.setSub( *p_proxy.atomPositions, "SpheresCylindersPositions", cacheSC.rangeSpheres.getFirst() );
		};

		// Colors.
		p_proxy.onAtomColors += [ this, &p_proxy ]( const std::vector<uchar> & p_colors )
		{
			Cache::SphereCylinder & cacheSC = _cacheSpheresCylinders[ &p_proxy ];
			//_context.setSub( p_colors, "SpheresCylindersColors", cacheSC.rangeSpheres.getFirst() );
		};

		// Residue colors.
		p_proxy.onResidueColors += [ this, &p_proxy ]( const std::vector<uchar> & p_colors )
		{
			Cache::Ribbon & cacheR = _cacheRibbons[ &p_proxy ];
			//_context.setSub( p_colors, "RibbonsColors", cacheR.range.getFirst() );
		};

		// Selection.
		// TODO: optimize.
		p_proxy.onSelect += [ this, &p_proxy ]( const bool p_select )
		{
			Cache::SphereCylinder & cacheSC = _cacheSpheresCylinders[ &p_proxy ];
			Cache::Ribbon &			cacheR	= _cacheRibbons[ &p_proxy ];
			uchar					mask	= 1 << E_ELEMENT_FLAGS::SELECTION;

			for ( size_t i = 0; i < cacheSC.rangeSpheres.getCount(); ++i )
			{
				cacheSC.flags[ i ] &= ~mask;
				cacheSC.flags[ i ] |= p_select << E_ELEMENT_FLAGS::SELECTION;
			}
			//_context.setSub( cacheSC.flags, "SpheresCylindersFlags", cacheSC.rangeSpheres.getFirst() );

			for ( size_t i = 0; i < cacheR.range.getCount(); ++i )
			{
				cacheR.flags[ i ] &= ~mask;
				cacheR.flags[ i ] |= p_select << E_ELEMENT_FLAGS::SELECTION;
			}

			//_context.setSub( cacheR.flags, "RibbonsFlags", cacheR.range.getFirst(), cacheR.range.getCount() );
		};

		// TODO:
		// - compare with map/unmap
		// - compare with ssbo/compute shader
		p_proxy.onAtomSelections +=
			[ this, &p_proxy ]( const Util::Math::RangeList<uint> & p_atomIds, const bool p_select )
		{
			Cache::SphereCylinder & cacheSC = _cacheSpheresCylinders[ &p_proxy ];
			Cache::Ribbon &			cacheR	= _cacheRibbons[ &p_proxy ];
			uchar					mask	= 1 << E_ELEMENT_FLAGS::SELECTION;

			const auto begin = cacheSC.flags.begin();
			for ( auto it = p_atomIds.rangeBegin(); it != p_atomIds.rangeEnd(); ++it )
			{
				const uint first = it->getFirst();
				const uint last	 = it->getLast();

				std::for_each(
					std::execution::par_unseq,
					begin + first,
					begin + last + 1,
					[ & ]( uchar & p_flag )
					{ p_flag = ( p_flag & ~mask ) | ( p_select << E_ELEMENT_FLAGS::SELECTION ); }
				);
			}

			/*
			for ( const uint i : p_atomIds )
			{
				cacheSC.flags[ i ] &= ~mask;
				cacheSC.flags[ i ] |= p_select << E_ELEMENT_FLAGS::SELECTION;
			}
			*/

			const size_t offset = cacheSC.rangeSpheres.getFirst();

			/*
			_context.setSub(
				cacheSC.flags,
				"SpheresCylindersFlags",
				offset + p_atomIds.getFirst(),
				p_atomIds.getFirst(),
				p_atomIds.getLast() - p_atomIds.getFirst() + 1
			);
			*/

			// TODO: ribbons and SES.
		};

		// Visibility.
		p_proxy.onVisible += [ this, &p_proxy ]( const bool p_visible )
		{
			auto & rangeSpheres	  = _cacheSpheresCylinders[ &p_proxy ].rangeSpheres;
			auto & rangeCylinders = _cacheSpheresCylinders[ &p_proxy ].rangeCylinders;
			auto & rangeRibbons	  = _cacheRibbons[ &p_proxy ].range;

			if ( p_visible )
			{
				drawRangeSpheresRL.addRange( rangeSpheres );
				drawRangeCylindersRL.addRange( rangeCylinders );
				drawRangeRibbonsRL.addRange( rangeRibbons );
			}
			else
			{
				drawRangeSpheresRL.removeRange( rangeSpheres );
				drawRangeCylindersRL.removeRange( rangeCylinders );
				drawRangeRibbonsRL.removeRange( rangeRibbons );
			}

			/*
			drawRangeSpheresRL.toStdVectorsFirstCount<void *, uint>(
				drawRangeSpheres.offsets, drawRangeSpheres.counts
			);
			drawRangeCylindersRL.toStdVectorsFirstCount<void *, uint>(
				drawRangeCylinders.offsets, drawRangeCylinders.counts
			);
			drawRangeRibbonsRL.toStdVectorsFirstCount<void *, uint>(
				drawRangeRibbons.offsets, drawRangeRibbons.counts
			);
			*/
		};

		// TODO: threshold to switch between multiple draw calls and single draw call.
		p_proxy.onAtomVisibilities +=
			[ this, &p_proxy ]( const Util::Math::RangeList<uint> & p_atomIds, const bool p_visible )
		{
			Cache::SphereCylinder & cacheSC = _cacheSpheresCylinders[ &p_proxy ];
			Cache::Ribbon &			cacheR	= _cacheRibbons[ &p_proxy ];
			uchar					mask	= 1 << E_ELEMENT_FLAGS::VISIBILITY;

			const auto begin = cacheSC.flags.begin();
			for ( auto it = p_atomIds.rangeBegin(); it != p_atomIds.rangeEnd(); ++it )
			{
				const uint first = it->getFirst();
				const uint last	 = it->getLast();

				std::for_each(
					std::execution::par_unseq,
					begin + first,
					begin + last + 1,
					[ & ]( uchar & p_flag )
					{ p_flag = ( p_flag & ~mask ) | ( p_visible << E_ELEMENT_FLAGS::VISIBILITY ); }
				);
			}

			const size_t offset = cacheSC.rangeSpheres.getFirst();

			/*
			_context.setSub(
				cacheSC.flags,
				"SpheresCylindersFlags",
				offset + p_atomIds.getFirst(),
				p_atomIds.getFirst(),
				p_atomIds.getLast() - p_atomIds.getFirst() + 1
			);
			*/

			// TODO: ribbons.
		};

		// TODO:
		// onAtomVisibilities
		// onAtomSelections
		// onAtomRepresentations
		// onAtomColorsRange
		// onResidueColorsRange

		/*
		p_proxy.onVisible += [ this, &p_proxy ]( const bool p_visible )
		{
			Cache::SphereCylinder & cacheSC = _cacheSpheresCylinders[ &p_proxy ];
			Cache::Ribbon &			cacheR	= _cacheRibbons[ &p_proxy ];
			uchar					mask	= 1 << E_ELEMENT_FLAGS::VISIBILITY;

			for ( size_t i = 0; i < cacheSC.size; ++i )
			{
				cacheSC.flags[ i ] &= ~mask;
				cacheSC.flags[ i ] |= p_visible << E_ELEMENT_FLAGS::VISIBILITY;
			}
			_context.setSub( cacheSC.flags, "SpheresCylindersFlags", cacheSC.offset );

			for ( size_t i = 0; i < cacheR.size; ++i )
			{
				cacheR.bufferFlags[ i ] &= ~mask;
				cacheR.bufferFlags[ i ] |= p_visible << E_ELEMENT_FLAGS::VISIBILITY;
			}
			_context.setSub( cacheR.bufferFlags, "RibbonsFlags", cacheR.offset );
		};
		*/
	}

	void Renderer::_removeProxySystem( Proxy::System & p_proxy )
	{
		std::erase( _proxiesSystems, &p_proxy );
		_cacheSpheresCylinders.erase( &p_proxy );
		_cacheRibbons.erase( &p_proxy );
	}

#pragma endregion Proxy systems

#pragma region Proxy representations

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

		//_context.set( buffer, "Representations" );

		setNeedUpdate( true );

		// Aply logic.
		showAtoms	= p_representation.hasSphere;
		showBonds	= p_representation.hasCylinder;
		showRibbons = p_representation.hasRibbon;
		showSES		= p_representation.hasSes;

		// Asked SES, hide all others.
		if ( showSES )
		{
#ifdef VTX_CUDA_ENABLED
			if ( not _sesData and _proxiesSystems.size() > 0 )
			{
				_createSes( *_proxiesSystems[ 0 ] );
			}
#endif

			showAtoms	= false;
			showBonds	= false;
			showRibbons = false;
			return;
		}
#ifdef VTX_CUDA_ENABLED
		else if ( _sesData )
		{
			_sesData.reset();
		}
#endif

		const bool	isSphereRadiusFixed = p_representation.isRadiusSphereFixed;
		const float cylinderRadius		= p_representation.radiusCylinder;
		const float sphereRadiusFixed	= p_representation.radiusSphereFixed;
		const float sphereRadiusAdd		= p_representation.radiusSphereAdd;

		// Hide ribbon if VdW radius.
		if ( not isSphereRadiusFixed )
		{
			showBonds	= false;
			showRibbons = false;
			return;
		}

		// If B&S.
		if ( showAtoms && showBonds )
		{
			// Scale sphere radius to cylinder radius if not VdW.
			if ( isSphereRadiusFixed && sphereRadiusFixed < cylinderRadius )
			{
				setValue( cylinderRadius, "RepresentationsSphereRadiusFixed" );
			}
		}

		// If sticks only, force sphere at cylinder radius.
		else if ( not showAtoms && showBonds )
		{
			showAtoms = true;
			setValue( uint( true ), "RepresentationsIsSphereRadiusFixed" );
			setValue( cylinderRadius, "RepresentationsSphereRadiusFixed" );
		}
	}

#pragma endregion Proxy representations

	void Renderer::setCamera(
		const Camera & p_camera,
		const Vec3f &  p_position,
		const Mat4f &  p_matView,
		const Mat4f &  p_matProj
	)
	{
		const Mat4f matrixViewInv	   = Util::Math::inverse( p_matView );
		const Mat4f matrixViewInvTrans = Util::Math::transpose( matrixViewInv );

		BinaryBuffer<E_LAYOUT_TYPE::Std140> buffer;
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

		//_context.set( buffer, "Camera" );

		setNeedUpdate( true );
	}

	void Renderer::setColorLayout( const Color::Layout & p_layout )
	{
		//_context.set( p_layout.colors, "ColorLayout" );

		setNeedUpdate( true );
	}

	void Renderer::setGraphicsConfig( const GraphicsConfig & p_config )
	{
		_refreshGraph( p_config );
		build();

		setValue( uint( p_config.shadingMode ), "ShadingShadingMode" );
		setValue( p_config.colorLight, "ShadingShadingLightColor" );
		setValue( p_config.colorBackground, "ShadingShadingBackgroundColor" );
		setValue( p_config.specularFactor, "ShadingShadingSpecularFactor" );
		setValue( p_config.shininess, "ShadingShadingShininess" );
		setValue( p_config.toonSteps, "ShadingShadingToonSteps" );
		if ( p_config.activeSSAO )
		{
			setValue( p_config.ssaoIntensity, "SSAOSSAOIntensity" );
			setValue( p_config.blurSize, "BlurXBlurSize" );
			setValue( p_config.blurSize, "BlurYBlurSize" );
		}
		if ( p_config.activeOutline )
		{
			setValue( p_config.colorOutline, "OutlineOutlineColor" );
			setValue( p_config.outlineSensitivity, "OutlineOutlineSensitivity" );
			setValue( p_config.outlineThickness, "OutlineOutlineThickness" );
		}
		setValue( p_config.colorFog, "ShadingShadingFogColor" );
		setValue( p_config.fogNear, "ShadingShadingFogNear" );
		setValue( p_config.fogFar, "ShadingShadingFogFar" );
		setValue( p_config.activeFog ? p_config.fogDensity : 0.f, "ShadingShadingFogDensity" );
		if ( p_config.activeSelection )
		{
			setValue( p_config.colorSelection, "SelectionSelectionColor" );
		}

		setNeedUpdate( true );
	}

	void Renderer::setProxyVoxels( Proxy::Voxels & p_proxy )
	{
		_proxyVoxels = &p_proxy;

		assert( p_proxy.mins );
		assert( p_proxy.maxs );
		assert( p_proxy.mins->size() == p_proxy.maxs->size() );

		//_context.set( *p_proxy.mins, "VoxelsMins" );
		//_context.set( *p_proxy.maxs, "VoxelsMaxs" );

		// drawRangeVoxels.offsets = { 0 };
		// drawRangeVoxels.counts	= { uint( p_proxy.mins->size() ) };

		setNeedUpdate( true );
	}

	void Renderer::_refreshDataSpheresCylinders()
	{
		// Check data.
		size_t totalAtoms = 0;
		size_t totalBonds = 0;
		for ( const Proxy::System * const proxy : _proxiesSystems )
		{
			// Check sizes.
			assert( proxy->atomPositions );

			assert( proxy->atomIds.size() == proxy->atomPositions->size() );
			assert( proxy->atomIds.size() == proxy->atomColors.size() );
			assert( proxy->atomIds.size() == proxy->atomRadii.size() );

			totalAtoms += proxy->atomPositions->size();
			totalBonds += proxy->bonds->size();
		}

		// Create buffers.
		/*
		_context.reserveData<Vec3f>( totalAtoms, "SpheresCylindersPositions" );
		_context.reserveData<uchar>( totalAtoms, "SpheresCylindersColors" );
		_context.reserveData<float>( totalAtoms, "SpheresCylindersRadii" );
		_context.reserveData<uint>( totalAtoms, "SpheresCylindersIds" );
		_context.reserveData<uchar>( totalAtoms, "SpheresCylindersFlags" );
		_context.reserveData<ushort>( totalAtoms, "SpheresCylindersModels" );
		_context.reserveData<uchar>( totalAtoms, "SpheresCylindersRepresentations" );
		_context.reserveData<uint>( totalBonds, "SpheresCylindersIdx" );
		*/

		size_t offsetAtoms = 0;
		size_t offsetBonds = 0;
		ushort modelId	   = 0;
		for ( const Proxy::System * const proxy : _proxiesSystems )
		{
			Cache::SphereCylinder & cache = _cacheSpheresCylinders[ proxy ];

			const size_t atomCount = proxy->atomPositions->size();
			const size_t bondCount = proxy->bonds->size();

			// Fill buffers.
			/*
			_context.setSub( *proxy->atomPositions, "SpheresCylindersPositions", offsetAtoms );
			_context.setSub( proxy->atomColors, "SpheresCylindersColors", offsetAtoms );
			_context.setSub( proxy->atomRadii, "SpheresCylindersRadii", offsetAtoms );
			_context.setSub( proxy->atomIds, "SpheresCylindersIds", offsetAtoms );
			*/

			// Flags if not cached.
			if ( cache.flags.empty() )
			{
				std::vector<uchar> atomFlags( atomCount );
				for ( size_t i = 0; i < atomFlags.size(); ++i )
				{
					uchar flag = 0;
					flag |= 1 << E_ELEMENT_FLAGS::VISIBILITY;
					flag |= 0 << E_ELEMENT_FLAGS::SELECTION;
					atomFlags[ i ] = flag;
				}
				cache.flags = atomFlags;
			}

			// Representations if not cached.
			if ( cache.representations.empty() )
			{
				cache.representations = std::vector<uchar>( atomCount, proxy->idDefaultRepresentation );
			}

			/*
			_context.setSub( cache.flags, "SpheresCylindersFlags", offsetAtoms );
			_context.setSub( std::vector<ushort>( atomCount, modelId ), "SpheresCylindersModels", offsetAtoms );
			_context.setSub( cache.representations, "SpheresCylindersRepresentations", offsetAtoms );
			*/

			// Move bonds.
			std::vector<uint> bonds( bondCount );
			for ( size_t i = 0; i < bondCount; ++i )
			{
				bonds[ i ] = uint( ( *proxy->bonds )[ i ] + offsetAtoms );
			}
			//_context.setSub( bonds, "SpheresCylindersIdx", offsetBonds );

			// Offsets.
			cache.rangeSpheres	 = Util::Math::Range<size_t> { offsetAtoms, atomCount };
			cache.rangeCylinders = Util::Math::Range<size_t> { offsetBonds, bondCount };
			offsetAtoms += atomCount;
			offsetBonds += bondCount;
			modelId++;
		}

		// Ranges.
		drawRangeSpheresRL.clear();
		drawRangeCylindersRL.clear();

		// TODO: refresh with cache, with a threshold to switch between multi call and shader variable.

		drawRangeSpheresRL.addRange( Util::Math::Range<size_t> { 0, uint( totalAtoms ) } );
		drawRangeCylindersRL.addRange( Util::Math::Range<size_t> { 0, uint( totalBonds ) } );

		/*
		drawRangeSpheresRL.toStdVectorsFirstCount<void *, uint>( drawRangeSpheres.offsets, drawRangeSpheres.counts );
		drawRangeCylindersRL.toStdVectorsFirstCount<void *, uint>(
			drawRangeCylinders.offsets, drawRangeCylinders.counts
		);
		*/
	}

	void Renderer::_refreshDataRibbons()
	{
		size_t totalCaPositions = 0;
		size_t totalIndices		= 0;

		for ( const Proxy::System * const proxy : _proxiesSystems )
		{
			assert( proxy->atomNames );
			assert( proxy->residueSecondaryStructureTypes );
			assert( proxy->residueFirstAtomIndexes );
			assert( proxy->residueAtomCounts );
			assert( proxy->chainFirstResidues );
			assert( proxy->chainResidueCounts );

			assert( proxy->atomNames->size() == proxy->atomPositions->size() );
			assert( proxy->residueIds.size() == proxy->residueSecondaryStructureTypes->size() );
			assert( proxy->residueIds.size() == proxy->residueColors.size() );
			assert( proxy->residueIds.size() == proxy->residueFirstAtomIndexes->size() );
			assert( proxy->residueIds.size() == proxy->residueAtomCounts->size() );
			assert( proxy->chainFirstResidues->size() == proxy->chainResidueCounts->size() );

			// Compute data if not cached.
			Cache::Ribbon & cache = _cacheRibbons[ proxy ];
			if ( not cache.positions.empty() || cache.isEmpty )
			{
				// ??
				totalCaPositions += cache.positions.size();
				totalIndices += cache.indices.size();
				continue;
			}

			// Check if data.
			if ( proxy->residueSecondaryStructureTypes->empty() )
			{
				cache.isEmpty = true;
				continue;
			}

			// Carbon alpha (Ca) positions.
			// Add an extra float increasing along the backbone (to determine direction for two sided ss).
			std ::vector<Vec4f> & bufferCaPositions = cache.positions;
			// Ca -> O directions.
			std::vector<Vec3f> & bufferCaODirections = cache.directions;
			// Secondary structure types.
			std::vector<uchar> &				bufferSSTypes	   = cache.ssTypes;
			std::vector<uchar> &				bufferColors	   = cache.colors;
			std::vector<uint> &					bufferIds		   = cache.ids;
			std::vector<uchar> &				bufferFlags		   = cache.flags;
			std::vector<uint> &					bufferIndices	   = cache.indices;
			std::map<uint, uint> &				residueToIndices   = cache.residueToIndices;
			std::map<uint, uint> &				residueToPositions = cache.residueToPositions;
			std::map<uint, std::vector<uint>> & data			   = cache.data;

			auto _tryConstruct = [ & ](
									 const uint					p_chainIdx,
									 const std::vector<uint> &	p_residueIndex,
									 const std::vector<Vec4f> & p_caPositions,
									 std::vector<Vec3f> &		p_caODirections,
									 const std::vector<uchar> & p_ssTypes,
									 const std::vector<uchar> & p_colors,
									 const std::vector<uint> &	p_ids,
									 const std::vector<uchar> & p_flags

								 )
			{
				if ( p_caPositions.size() >= 4 )
				{
					const size_t nbControlPoints = p_caPositions.size();

					residueToPositions.emplace( p_residueIndex[ 0 ], uint( bufferCaPositions.size() ) );
					residueToIndices.emplace( p_residueIndex[ 0 ], uint( bufferIndices.size() ) );

					const uint offset = uint( bufferCaPositions.size() );

					// Add segment with duplicate first index to evaluate B-spline at 0-1.
					bufferIndices.emplace_back( offset );
					bufferIndices.emplace_back( offset );
					bufferIndices.emplace_back( offset + 1 );
					bufferIndices.emplace_back( offset + 2 );

					for ( uint i = 1; i < nbControlPoints - 2; ++i )
					{
						residueToPositions.emplace( p_residueIndex[ i ], uint( bufferCaPositions.size() + i ) );
						residueToIndices.emplace( p_residueIndex[ i ], uint( bufferIndices.size() ) );

						bufferIndices.emplace_back( offset + i - 1 );
						bufferIndices.emplace_back( offset + i );
						bufferIndices.emplace_back( offset + i + 1 );
						bufferIndices.emplace_back( offset + i + 2 );
					}

					// TODO: better on GPU ?
					// CheckOrientationAndFlip.
					size_t i;
					for ( i = 1; i < p_caODirections.size(); ++i )
					{
						if ( Util::Math::dot( p_caODirections[ i ], p_caODirections[ i - 1 ] ) < 0.f )
						{
							p_caODirections[ i ] = -p_caODirections[ i ];
						}
					}

					// Merge buffers.
					auto it = data.find( p_chainIdx );
					if ( it == data.end() )
					{
						data.emplace( p_chainIdx, std::vector<uint>() );
					}
					data[ p_chainIdx ].insert(
						std::end( data[ p_chainIdx ] ), std::begin( p_residueIndex ), std::end( p_residueIndex )
					);

					bufferCaPositions.insert( bufferCaPositions.end(), p_caPositions.cbegin(), p_caPositions.cend() );
					bufferCaODirections.insert(
						bufferCaODirections.end(), p_caODirections.cbegin(), p_caODirections.cend()
					);
					bufferSSTypes.insert( bufferSSTypes.end(), p_ssTypes.cbegin(), p_ssTypes.cend() );
					bufferColors.insert( bufferColors.end(), p_colors.cbegin(), p_colors.cend() );
					bufferIds.insert( bufferIds.end(), p_ids.cbegin(), p_ids.cend() );
					bufferFlags.insert( bufferFlags.end(), p_flags.cbegin(), p_flags.cend() );
				}
			};

			const std::vector<Vec3f> & positions = *proxy->atomPositions;

			// Temporary vectors, merged with buffers if constructed.
			std::vector<Vec4f> caPositions;
			std::vector<Vec3f> caODirections;
			std::vector<uchar> types;
			std::vector<uchar> colors;
			std::vector<uint>  ids;
			std::vector<uchar> flags;
			std::vector<uint>  residueIndex;

			for ( uint chainIdx = 0; chainIdx < proxy->chainFirstResidues->size(); ++chainIdx )
			{
				/*
				const Chain * const chain = _system->getChain( chainIdx );
				if ( chain == nullptr )
				{
					continue;
				}
				*/

				uint residueCount	 = uint( ( *proxy->chainResidueCounts )[ chainIdx ] );
				uint idxFirstResidue = uint( ( *proxy->chainFirstResidues )[ chainIdx ] );

				// No enought residues.
				if ( residueCount < 4 ) // TODO: what to do ?
				{
					VTX_DEBUG( "Chain residue count < 4" );
					continue;
				}

				bool createVectors = true;
				int	 residueLast   = -1;
				for ( uint residueIdx = idxFirstResidue; residueIdx < idxFirstResidue + residueCount; ++residueIdx )
				{
					if ( createVectors )
					{
						caPositions	  = std::vector<Vec4f>();
						caODirections = std::vector<Vec3f>();
						types		  = std::vector<uchar>();
						colors		  = std::vector<uchar>();
						ids			  = std::vector<uint>();
						flags		  = std::vector<uchar>();

						residueIndex = std::vector<uint>();

						createVectors = false;
					}

					/*
					if ( residue == nullptr )
					{
						continue;
					}
					*/

					auto findFirstAtomByName = [ &proxy ]( const uint p_residueIdx, const std::string & p_name )
					{
						uint atomCount	  = ( *proxy->residueAtomCounts )[ p_residueIdx ];
						uint idxFirstAtom = ( *proxy->residueFirstAtomIndexes )[ p_residueIdx ];

						for ( int i = idxFirstAtom; i < int( idxFirstAtom + atomCount ); ++i )
						{
							if ( ( *proxy->atomNames )[ i ] == p_name )
							{
								return i;
							}
						}

						return -1;
					};

					// Use backbone to compute spline data.
					// Find alpha carbon.
					int CA = findFirstAtomByName( residueIdx, "CA" );

					// Not an amine acid (water, heme, or phosphate groupment).
					if ( CA == -1 ) // TODO: what to do ?
					{
						continue;
					}

					// Find oxygen.
					int O = findFirstAtomByName( residueIdx, "O" );

					// Missing oxygen atom.
					if ( O == -1 ) // TODO: what to do?
					{
						continue;
					}
					/// TODO: For all these "what to do ?" I think we should render it with spheres or b&s...

					// Compute direction between carbon alpha and oxygen.
					const Vec3f & positionCA   = positions[ CA ];
					const Vec3f & positionO	   = positions[ O ];
					const Vec3f	  directionCAO = Util::Math::normalize( positionO - positionCA );

					// Store residue index for later.
					residueIndex.emplace_back( residueIdx );

					// Add carbon alpha (CA) position and CA-O direction.
					caPositions.emplace_back(
						Vec4f( positionCA, float( bufferCaPositions.size() + caPositions.size() ) )
					);
					caODirections.emplace_back( directionCAO );

					// Add secondary structure type.
					types.emplace_back( ( *proxy->residueSecondaryStructureTypes )[ residueIdx ] );

					/*
					switch ( residue->getRepresentation()->getRibbonData().colorMode )
					{
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::JMOL:
						colors.emplace_back( Generic::COLORS_JMOL[ uint( residue->getSecondaryStructure() ) ] );
						break;
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN:
						colors.emplace_back( residue->getSystemPtr()->getColor() );
						break;
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM:
						colors.emplace_back( residue->getRepresentation()->getColor() );
						break;
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CHAIN:
						colors.emplace_back( residue->getChainPtr()->getColor() );
						break;
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::RESIDUE:
						colors.emplace_back( residue->getColor() );
						break;
					default: colors.emplace_back( Color::Rgba::WHITE ); break;
					}
					*/

					// Generate number between 0 and 255.
					// int color = ( ( residueIdx * 7 ) % 256 );
					// colors.emplace_back( color );

					colors.emplace_back( proxy->residueColors[ residueIdx ] );
					ids.emplace_back( proxy->residueIds[ residueIdx ] );

					// Flag.
					// TODO.
					flags.emplace_back( 1 );

					/*
					visibilities.emplace_back( uint(
						_system->isVisible() && chain->isVisible() && residue->isVisible() && CA->isVisible()
						&& O->isVisible()
					) );
					*/

					/*
					if ( residueLast != -1
						 && residue->getIndexInOriginalChain() != residueLast->getIndexInOriginalChain() + 1 )
					{
						_tryConstruct( chainIdx, residueIndex, caPositions, caODirections, types, colors, flags, ids
					); createVectors = true;
					}
					*/

					residueLast = residueIdx;
				}

				// Update buffers and index mapping if SS is constructed.
				_tryConstruct( chainIdx, residueIndex, caPositions, caODirections, types, colors, ids, flags );
			}

			assert( bufferCaPositions.size() == bufferCaODirections.size() );
			assert( bufferCaPositions.size() == bufferSSTypes.size() );
			assert( bufferCaPositions.size() == bufferColors.size() );
			assert( bufferCaPositions.size() == bufferIds.size() );
			assert( bufferCaPositions.size() == bufferFlags.size() );

			if ( bufferCaPositions.empty() )
			{
				cache.isEmpty = true;
				continue;
			}

			totalCaPositions += bufferCaPositions.size();
			totalIndices += bufferIndices.size();
		}

		size_t offsetIndices = 0;
		if ( _proxiesSystems.empty() || totalCaPositions == 0 )
		{
			assert( totalIndices == 0 );
		}

		/*
		_context.reserveData<Vec4f>( totalCaPositions, "RibbonsPositions" );
		_context.reserveData<Vec3f>( totalCaPositions, "RibbonsDirections" );
		_context.reserveData<uchar>( totalCaPositions, "RibbonsTypes" );
		_context.reserveData<uchar>( totalCaPositions, "RibbonsColors" );
		_context.reserveData<uint>( totalCaPositions, "RibbonsIds" );
		_context.reserveData<uchar>( totalCaPositions, "RibbonsFlags" );
		_context.reserveData<ushort>( totalCaPositions, "RibbonsModels" );
		_context.reserveData<uchar>( totalCaPositions, "RibbonsRepresentations" );
		_context.reserveData<uint>( totalIndices, "RibbonsIdx" );
		*/

		size_t offsetCaPositions = 0;
		uchar  modelId			 = -1;
		for ( const Proxy::System * const proxy : _proxiesSystems )
		{
			modelId++;
			Cache::Ribbon & cache = _cacheRibbons[ proxy ];

			assert( cache.isEmpty || cache.positions.size() > 0 );

			if ( cache.positions.empty() == true )
			{
				continue;
			}

			// Move indices.
			// TODO: caches indices ?
			std::vector<uint> indices = cache.indices;
			for ( size_t i = 0; i < cache.indices.size(); ++i )
			{
				indices[ i ] += uint( offsetCaPositions );
			}

			if ( cache.representations.empty() )
			{
				cache.representations = std::vector<uchar>( cache.positions.size(), 0 );
			}

			/*
			_context.setSub( cache.positions, "RibbonsPositions", offsetCaPositions );
			_context.setSub( cache.directions, "RibbonsDirections", offsetCaPositions );
			_context.setSub( cache.ssTypes, "RibbonsTypes", offsetCaPositions );
			_context.setSub( cache.colors, "RibbonsColors", offsetCaPositions );
			_context.setSub( cache.ids, "RibbonsIds", offsetCaPositions );
			_context.setSub( cache.flags, "RibbonsFlags", offsetCaPositions );
			_context.setSub(
				std::vector<ushort>( cache.positions.size(), modelId ), "RibbonsModels", offsetCaPositions
			);
			_context.setSub( cache.representations, "RibbonsRepresentations", offsetCaPositions );
			_context.setSub( indices, "RibbonsIdx", offsetIndices );
			*/

			// Offsets.
			cache.range = Util::Math::Range<size_t> { offsetCaPositions, cache.positions.size() };
			offsetCaPositions += cache.positions.size();
			offsetIndices += cache.indices.size();
		}

		// Ranges.
		/*
		drawRangeRibbonsRL.clear();
		drawRangeRibbonsRL.addRange( Util::Math::Range<size_t> { 0, uint( offsetIndices ) } );
		drawRangeRibbonsRL.toStdVectorsFirstCount<void *, uint>( drawRangeRibbons.offsets, drawRangeRibbons.counts );
	*/
	}

	void Renderer::_refreshDataModels()
	{
		/*
		BinaryBuffer buffer;

		for ( const Proxy::System * const proxy : _proxiesSystems )
		{
			assert( proxy->transform );
			assert( _proxyCamera );

			const Mat4f matrixModelView	   = *_proxyCamera->matrixView * *proxy->transform;
			const Mat4f matrixModelViewInv = Util::Math::inverse( matrixModelView );
			const Mat4f matrixNormal	   = Util::Math::transpose( matrixModelViewInv );

			buffer.write( matrixModelView );
			buffer.write( matrixModelViewInv );
			buffer.write( matrixNormal );
		}

		buffer.close();

		_context.set( buffer, "Models" );
		*/
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

	void Renderer::_renderLog( const float p_deltaTime, const float p_elapsedTime )
	{
		/*
		for ( InstructionsDurationRange & instructionDurationRange : _instructionsDurationRanges )
		{
			instructionDurationRange.duration = _context.measureTaskDuration(

				[ this, &instructionDurationRange ]()
				{
					for ( size_t i = instructionDurationRange.first; i <= instructionDurationRange.last; ++i )
					{
						_instructions[ i ]();
					}
				}
			);
		}
		*/
	}

	/*
	StructInfos Renderer::getInfos() const
	{
		StructInfos infos;
		_context.fillInfos( infos );

		// Compute size of cached data.
		size_t sizeCache = 0;
		for ( const auto & [ proxy, cache ] : _cacheSpheresCylinders )
		{
			sizeCache += cache.currentSize();
		}
		for ( const auto & [ proxy, cache ] : _cacheRibbons )
		{
			sizeCache += cache.currentSize();
		}
		infos.currentSizeCPUCache = sizeCache;

		return infos;
	}
	*/

	void Renderer::_refreshGraph( const GraphicsConfig & p_config )
	{
		RenderGraph::PipelineConfig config;

		config.enableSSAO	   = p_config.activeSSAO;
		config.enableOutline   = p_config.activeOutline;
		config.enableSelection = p_config.activeSelection;

		_graph.createDefaultPipeline( config );

		/*
		RenderGraph::PipelinePasses passes = _graph.createDefaultPipeline( config );
		Pass *						geo	   = passes.geo;
		assert( geo );

		geo->programs[ 0 ].draw.value().ranges = &drawRangeSpheres;
		geo->programs[ 0 ].draw.value().needRenderFunc
			= [ this ]() { return showAtoms && !drawRangeSpheres.counts.empty(); };

		geo->programs[ 1 ].draw.value().ranges = &drawRangeCylinders;
		geo->programs[ 1 ].draw.value().needRenderFunc
			= [ this ]() { return showBonds && !drawRangeCylinders.counts.empty(); };

		geo->programs[ 2 ].draw.value().ranges = &drawRangeRibbons;
		geo->programs[ 2 ].draw.value().needRenderFunc
			= [ this ]() { return showRibbons && !drawRangeRibbons.counts.empty(); };

		geo->programs[ 3 ].draw.value().ranges = &drawRangeVoxels;
		geo->programs[ 3 ].draw.value().needRenderFunc
			= [ this ]() { return showVoxels && !drawRangeVoxels.counts.empty(); };
			*/
#ifdef VTX_CUDA_ENABLED
		geo->renderFunc = [ & ]()
		{
			constexpr auto bindBuffer = []( uint32_t bindingPoint, bcs::HandleSpan<GLuint> buffer )
			{
				if ( buffer.size > 0 )
					glBindBufferRange(
						GL_SHADER_STORAGE_BUFFER, bindingPoint, buffer.handle, buffer.offset, buffer.size
					);
			};

			if ( _sesData )
			{
				bindBuffer( 1, _sesSurface.atoms );
				bindBuffer( 2, _sesSurface.segmentPatches );
				bindBuffer( 3, _sesSurface.concavePatchesPosition );
				bindBuffer( 4, _sesSurface.concavePatchesId );
				bindBuffer( 5, _sesSurface.concavePatchesNeighbors );
				bindBuffer( 6, _sesSurface.sectors );

				if ( _sesSurface.concavePatchNb > 0 )
				{
					_sesProgramConcave->use();
					glBindVertexArray( _sesVao );
					glDrawArrays( GL_POINTS, 0, static_cast<GLsizei>( _sesSurface.concavePatchNb ) );
				}

				if ( _sesSurface.circlePatchNb )
				{
					_sesProgramCircle->use();
					glBindVertexArray( _sesCircleVao );
					glDrawArrays( GL_POINTS, 0, static_cast<GLsizei>( _sesSurface.circlePatchNb ) );
				}

				if ( _sesSurface.convexPatchNb > 0 )
				{
					_sesProgramConvex->use();
					glBindVertexArray( _sesConvexVao );
					glDrawArrays( GL_POINTS, 0, static_cast<GLsizei>( _sesSurface.convexPatchNb ) );
				}

				if ( _sesSurface.segmentPatchNb > 0 )
				{
					_sesProgramSegment->use();
					glBindVertexArray( _sesSegmentVao );
					glDrawArrays( GL_POINTS, 0, static_cast<GLsizei>( _sesSurface.segmentPatchNb ) );
				}

				glBindVertexArray( 0 );
			}
		};
#endif
	}
} // namespace VTX::Renderer
