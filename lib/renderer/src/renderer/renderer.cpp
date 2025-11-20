#include "renderer/renderer.hpp"
#include "renderer/binary_buffer.hpp"
#include "renderer/renderer.hpp"
#include "renderer/scheduler/depth_first_search.hpp"
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
		_refreshGraph();

		// Shared data.
		addGlobalData(
			{ "Camera",
			  15,
			  { { "MatrixView", E_TYPE::MAT4F, BufferValue<Mat4f> { MAT4F_ID } },
				{ "MatrixProjection", E_TYPE::MAT4F, BufferValue<Mat4f> { MAT4F_ID } },
				{ "MatrixViewInv", E_TYPE::MAT4F, BufferValue<Mat4f> { MAT4F_ID } },
				{ "MatrixViewInvTrans", E_TYPE::MAT4F, BufferValue<Mat4f> { MAT4F_ID } },
				{ "Position", E_TYPE::VEC3F, BufferValue<Vec3f> { VEC3F_ZERO } },
				{ "ClipInfos", // { _near * _far, _far, _far - _near, _near }
				  E_TYPE::VEC4F,
				  BufferValue<Vec4f> { VEC4F_ZERO } },
				{ "Resolution", E_TYPE::VEC2I, BufferValue<Vec2i> { Vec2i { p_width, p_height } } },
				{ "MousePosition", E_TYPE::VEC2I, BufferValue<Vec2i> { Vec2i { 0, 0 } } },
				{ "IsPerspective", E_TYPE::UINT, BufferValue<uint> { 1 } } },
			  0,
			  nullptr,
			  false,
			  true }
		);

		addGlobalData(
			{ "ColorLayout",
			  14,
			  { { "Colors", E_TYPE::COLOR4, BufferValue<Util::Color::Rgba> {} } },
			  4096,
			  nullptr,
			  false,
			  true }
		);

		addGlobalData(
			{ "Models",
			  13,
			  {
				  { "MatrixModelView", E_TYPE::MAT4F, BufferValue<Mat4f> { MAT4F_ID } },
				  { "MatrixModelViewInv", E_TYPE::MAT4F, BufferValue<Mat4f> { MAT4F_ID } },
				  { "MatrixNormal", E_TYPE::MAT4F, BufferValue<Mat4f> { MAT4F_ID } },
			  },
			  0,
			  nullptr,
			  true }
		);

		addGlobalData(
			{ "Representations",
			  12,
			  {
				  { "SphereRadiusFixed", E_TYPE::FLOAT, BufferValue<float> {} },
				  { "SphereRadiusAdd", E_TYPE::FLOAT, BufferValue<float> {} },
				  { "IsSphereRadiusFixed", E_TYPE::UINT, BufferValue<uint> {} },
				  { "CylinderRadius", E_TYPE::FLOAT, BufferValue<float> {} },
				  { "CylinderColorBlending", E_TYPE::UINT, BufferValue<uint> {} },
				  { "RibbonColorBlending", E_TYPE::UINT, BufferValue<uint> {} },
				  { "SESProbeRadius", E_TYPE::FLOAT, BufferValue<float> {} },
				  { "SESMaxProbeNeighborNb", E_TYPE::UINT, BufferValue<uint> {} },
			  },
			  0,
			  nullptr,
			  true }
		);
	}

	void Renderer::build()
	{
		// Build renderer _graph.
		float buildTime = Util::CHRONO_CPU(
			[ this ]()
			{
				const RenderQueue & queue = _graph.build<Scheduler::DepthFirstSearch>();

				_context.build( queue, _graph.getLinks(), _globalData, _instructions, _instructionsDurationRanges );
			}
		);

		VTX_DEBUG( "Renderer graph setup total time: {}", Util::String::durationToStr( buildTime ) );
	}

	void Renderer::resize( const size_t p_width, const size_t p_height )
	{
		VTX_TRACE( "Resizing renderer to {}x{}", p_width, p_height );

		_width	= p_width;
		_height = p_height;

		Vec2i size = { p_width, p_height };
		setValue( size, "CameraResolution" );

		_context.resize( _graph.getRenderQueue(), p_width, p_height );

		setNeedUpdate( true );
	}

	void Renderer::clean()
	{
		_context.clear();
		_instructions.clear();
		_instructionsDurationRanges.clear();
		_graph.clean();
		_needUpdate		 = false;
		_framesRemaining = 0;

		_proxiesSystems.clear();
		_proxyCamera		 = nullptr;
		_proxyColorLayout	 = nullptr;
		_proxyRepresentation = nullptr;
		_proxyRenderSettings = nullptr;
		_proxyVoxels		 = nullptr;

		_cacheSpheresCylinders.clear();
		_cacheRibbons.clear();

		drawRangeSpheres.counts.clear();
		drawRangeSpheres.offsets.clear();
		drawRangeCylinders.counts.clear();
		drawRangeCylinders.offsets.clear();
		drawRangeRibbons.counts.clear();
		drawRangeRibbons.offsets.clear();

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

		// Representation.
		p_proxy.onRepresentation += [ this, &p_proxy ]( const uchar p_representation )
		{
			Cache::SphereCylinder & cacheSC = _cacheSpheresCylinders[ &p_proxy ];
			Cache::Ribbon &			cacheR	= _cacheRibbons[ &p_proxy ];

			cacheSC.representations = std::vector<uchar>( cacheSC.rangeSpheres.getCount(), p_representation );
			cacheR.representations	= std::vector<uchar>( cacheR.range.getCount(), p_representation );

			_context.setSub(
				cacheSC.representations, "SpheresCylindersRepresentations", cacheSC.rangeSpheres.getFirst()
			);
			_context.setSub( cacheR.representations, "RibbonsRepresentations", cacheR.range.getFirst() );
		};

		// Remove.
		p_proxy.onRemove += [ this, &p_proxy ]() { removeProxySystem( p_proxy ); };

		// Positions.
		p_proxy.onAtomPositions += [ this, &p_proxy ]()
		{
			Cache::SphereCylinder & cacheSC = _cacheSpheresCylinders[ &p_proxy ];
			_context.setSub( *p_proxy.atomPositions, "SpheresCylindersPositions", cacheSC.rangeSpheres.getFirst() );
		};

		// Colors.
		p_proxy.onAtomColors += [ this, &p_proxy ]( const std::vector<uchar> & p_colors )
		{
			Cache::SphereCylinder & cacheSC = _cacheSpheresCylinders[ &p_proxy ];
			_context.setSub( p_colors, "SpheresCylindersColors", cacheSC.rangeSpheres.getFirst() );
		};

		// Residue colors.
		p_proxy.onResidueColors += [ this, &p_proxy ]( const std::vector<uchar> & p_colors )
		{
			Cache::Ribbon & cacheR = _cacheRibbons[ &p_proxy ];
			_context.setSub( p_colors, "RibbonsColors", cacheR.range.getFirst() );
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
			_context.setSub( cacheSC.flags, "SpheresCylindersFlags", cacheSC.rangeSpheres.getFirst() );

			for ( size_t i = 0; i < cacheR.range.getCount(); ++i )
			{
				cacheR.flags[ i ] &= ~mask;
				cacheR.flags[ i ] |= p_select << E_ELEMENT_FLAGS::SELECTION;
			}

			_context.setSub( cacheR.flags, "RibbonsFlags", cacheR.range.getFirst(), cacheR.range.getCount() );
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

			_context.setSub(
				cacheSC.flags,
				"SpheresCylindersFlags",
				offset + p_atomIds.getFirst(),
				p_atomIds.getFirst(),
				p_atomIds.getLast() - p_atomIds.getFirst() + 1
			);

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

			drawRangeSpheresRL.toStdVectorsFirstCount<void *, uint>(
				drawRangeSpheres.offsets, drawRangeSpheres.counts
			);
			drawRangeCylindersRL.toStdVectorsFirstCount<void *, uint>(
				drawRangeCylinders.offsets, drawRangeCylinders.counts
			);
			drawRangeRibbonsRL.toStdVectorsFirstCount<void *, uint>(
				drawRangeRibbons.offsets, drawRangeRibbons.counts
			);
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

			_context.setSub(
				cacheSC.flags,
				"SpheresCylindersFlags",
				offset + p_atomIds.getFirst(),
				p_atomIds.getFirst(),
				p_atomIds.getLast() - p_atomIds.getFirst() + 1
			);

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

	void Renderer::_applyRepresentationLogic()
	{
		using namespace Proxy;

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

		const bool	isSphereRadiusFixed = _proxyRepresentation->data.isRadiusSphereFixed;
		const float cylinderRadius		= _proxyRepresentation->data.radiusCylinder;
		const float sphereRadiusFixed	= _proxyRepresentation->data.radiusSphereFixed;
		const float sphereRadiusAdd		= _proxyRepresentation->data.radiusSphereAdd;

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

		setNeedUpdate( true );
	}

	void Renderer::setProxyRepresentation( Proxy::Representation & p_proxy )
	{
		using namespace Proxy;

		_proxyRepresentation = &p_proxy;

		auto _setAllFun = [ this ]()
		{
			BinaryBuffer buffer;
			buffer.write( _proxyRepresentation->data.radiusSphereFixed );
			buffer.write( _proxyRepresentation->data.radiusSphereAdd );
			buffer.write( uint( _proxyRepresentation->data.isRadiusSphereFixed ) );
			buffer.write( _proxyRepresentation->data.radiusCylinder );
			buffer.write( uint( _proxyRepresentation->data.cylinderColorBlending ) );
			buffer.write( uint( _proxyRepresentation->data.ribbonColorBlending ) );
			buffer.write( _proxyRepresentation->data.sesProbeRadius );
			buffer.close();

			_context.set( buffer, "Representations" );

			showAtoms	= _proxyRepresentation->data.hasSphere;
			showBonds	= _proxyRepresentation->data.hasCylinder;
			showRibbons = _proxyRepresentation->data.hasRibbon;
			showSES		= _proxyRepresentation->data.hasSes;
			_applyRepresentationLogic();
		};

		_setAllFun();

		// Callbacks.
		p_proxy.getCallback<E_REPRESENTATION_VALUES::HAS_SPHERE>() += [ _setAllFun ]() { _setAllFun(); };
		p_proxy.getCallback<E_REPRESENTATION_VALUES::IS_SPHERE_RADIUS_FIXED>() += [ _setAllFun ]() { _setAllFun(); };
		p_proxy.getCallback<E_REPRESENTATION_VALUES::RADIUS_SPHERE_FIXED>() += [ _setAllFun ]() { _setAllFun(); };
		p_proxy.getCallback<E_REPRESENTATION_VALUES::RADIUS_SPHERE_ADD>() += [ _setAllFun ]() { _setAllFun(); };
		p_proxy.getCallback<E_REPRESENTATION_VALUES::HAS_CYLINDER>() += [ _setAllFun ]() { _setAllFun(); };
		p_proxy.getCallback<E_REPRESENTATION_VALUES::RADIUS_CYLINDER>() += [ _setAllFun ]() { _setAllFun(); };
		p_proxy.getCallback<E_REPRESENTATION_VALUES::CYLINDER_COLOR_BLENDING>() += [ _setAllFun ]() { _setAllFun(); };
		p_proxy.getCallback<E_REPRESENTATION_VALUES::HAS_RIBBON>() += [ _setAllFun ]() { _setAllFun(); };
		p_proxy.getCallback<E_REPRESENTATION_VALUES::RIBBON_COLOR_BLENDING>() += [ _setAllFun ]() { _setAllFun(); };
		p_proxy.getCallback<E_REPRESENTATION_VALUES::HAS_SES>() += [ _setAllFun ]() { _setAllFun(); };

		// TODO: remove useless primitives with multi calls.
		// TODO: compute ss if needed
		// TODO: delete others ss from cache?

		setNeedUpdate( true );
	}

#pragma endregion Proxy representations

	void Renderer::setProxyCamera( Proxy::Camera & p_proxy )
	{
		assert( p_proxy.matrixView );
		assert( p_proxy.matrixProjection );

		_proxyCamera = &p_proxy;

		const Mat4f matrixViewInv	   = Util::Math::inverse( *p_proxy.matrixView );
		const Mat4f matrixViewInvTrans = Util::Math::transpose( matrixViewInv );

		BinaryBuffer buffer;
		buffer.write( *p_proxy.matrixView );
		buffer.write( *p_proxy.matrixProjection );
		buffer.write( matrixViewInv );
		buffer.write( matrixViewInvTrans );
		buffer.write( p_proxy.cameraPosition );
		buffer.write( Vec4f(
			p_proxy.cameraNear * p_proxy.cameraFar,
			p_proxy.cameraFar,
			p_proxy.cameraFar - p_proxy.cameraNear,
			p_proxy.cameraNear
		) );
		buffer.write( Vec2i( width(), height() ) );
		buffer.write( p_proxy.mousePosition );
		buffer.write( uint( p_proxy.isPerspective ) );
		buffer.close();

		_context.set( buffer, "Camera" );

		p_proxy.onMatrixView += [ this, &p_proxy ]()
		{
			setValue( *p_proxy.matrixView, "CameraMatrixView" );

			const Mat4f matrixViewInv = Util::Math::inverse( *p_proxy.matrixView );
			setValue( matrixViewInv, "CameraMatrixViewInv" );
			const Mat4f matrixViewInvTrans = Util::Math::transpose( matrixViewInv );
			setValue( matrixViewInvTrans, "CameraMatrixViewInvTrans" );

			_refreshDataModels();
		};

		p_proxy.onMatrixProjection +=
			[ this, &p_proxy ]() { setValue( *p_proxy.matrixProjection, "CameraMatrixProjection" ); };

		p_proxy.onCameraPosition +=
			[ this, &p_proxy ]( const Vec3f & p_position ) { setValue( p_position, "CameraPosition" ); };

		p_proxy.onCameraNearFar += [ this, &p_proxy ]( const float p_near, const float p_far )
		{ setValue( Vec4f( p_near * p_far, p_far, p_far - p_near, p_near ), "CameraClipInfos" ); };

		p_proxy.onMousePosition += [ this, &p_proxy ]( const Vec2i & p_position )
		{
			// setValue( Vec2i { p_position.x, height - p_position.y }, "Mouse position" );
		};

		p_proxy.onPerspective += [ this, &p_proxy ]( const bool p_perspective )
		{ setValue( uint( p_perspective ), "CameraIsPerspective" ); };
	}

	void Renderer::setProxyColorLayout( Proxy::ColorLayout & p_proxy )
	{
		_proxyColorLayout = &p_proxy;
		_context.set( p_proxy.layout.colors, "ColorLayout" );
		setNeedUpdate( true );

		p_proxy.onChangeAll += [ this, &p_proxy ]()
		{
			_context.set( p_proxy.layout.colors, "ColorLayout" );
			setNeedUpdate( true );
		};
		p_proxy.onChange += [ this, &p_proxy ]( const size_t p_index )
		{
			_context.setSub(
				std::vector<Util::Color::Rgba> { p_proxy.layout.colors[ p_index ] }, "ColorLayout", p_index
			);
			setNeedUpdate( true );
		};
	}

	void Renderer::setProxyRenderSettings( Proxy::RenderSettings & p_proxy )
	{
		using namespace Proxy;

		_proxyRenderSettings = &p_proxy;

		_refreshGraph();
		build();

		// Default values.
		// Shading.
		setValue( uint( p_proxy.data.shadingMode ), "ShadingShadingMode" );
		setValue( p_proxy.data.colorLight, "ShadingShadingLightColor" );
		setValue( p_proxy.data.colorBackground, "ShadingShadingBackgroundColor" );
		setValue( p_proxy.data.specularFactor, "ShadingShadingSpecularFactor" );
		setValue( p_proxy.data.shininess, "ShadingShadingShininess" );
		setValue( p_proxy.data.toonSteps, "ShadingShadingToonSteps" );
		// SSAO.
		if ( p_proxy.data.activeSSAO )
		{
			setValue( p_proxy.data.ssaoIntensity, "SSAOSSAOIntensity" );
			setValue( p_proxy.data.blurSize, "BlurXBlurSize" );
			setValue( p_proxy.data.blurSize, "BlurYBlurSize" );
		}
		// Outline.
		if ( p_proxy.data.activeOutline )
		{
			setValue( p_proxy.data.colorOutline, "OutlineOutlineColor" );
			setValue( p_proxy.data.outlineSensitivity, "OutlineOutlineSensitivity" );
			setValue( p_proxy.data.outlineThickness, "OutlineOutlineThickness" );
		}
		// Fog.
		setValue( p_proxy.data.colorFog, "ShadingShadingFogColor" );
		setValue( p_proxy.data.fogNear, "ShadingShadingFogNear" );
		setValue( p_proxy.data.fogFar, "ShadingShadingFogFar" );
		setValue( p_proxy.data.activeFog ? p_proxy.data.fogDensity : 0.f, "ShadingShadingFogDensity" );
		// Selection.
		if ( p_proxy.data.activeSelection )
		{
			setValue( p_proxy.data.colorSelection, "SelectionSelectionColor" );
		}

		// Callbacks.
		// Shading.
		p_proxy.getCallback<E_RENDER_SETTINGS::SHADING_MODE>() +=
			[ this, &p_proxy ]() { setValue( uint( p_proxy.data.shadingMode ), "ShadingShadingMode" ); };

		p_proxy.getCallback<E_RENDER_SETTINGS::COLOR_LIGHT>() +=
			[ this, &p_proxy ]() { setValue( p_proxy.data.colorLight, "ShadingShadingLightColor" ); };
		p_proxy.getCallback<E_RENDER_SETTINGS::COLOR_BACKGROUND>() +=
			[ this, &p_proxy ]() { setValue( p_proxy.data.colorBackground, "ShadingShadingBackgroundColor" ); };
		p_proxy.getCallback<E_RENDER_SETTINGS::SPECULAR_FACTOR>() +=
			[ this, &p_proxy ]() { setValue( p_proxy.data.specularFactor, "ShadingShadingSpecularFactor" ); };
		p_proxy.getCallback<E_RENDER_SETTINGS::SHININESS>() +=
			[ this, &p_proxy ]() { setValue( p_proxy.data.shininess, "ShadingShadingShininess" ); };
		p_proxy.getCallback<E_RENDER_SETTINGS::TOON_STEPS>() +=
			[ this, &p_proxy ]() { setValue( p_proxy.data.toonSteps, "ShadingShadingToonSteps" ); };
		// SSAO.
		p_proxy.getCallback<E_RENDER_SETTINGS::SSAO_INTENSITY>() +=
			[ this, &p_proxy ]() { setValue( p_proxy.data.ssaoIntensity, "SSAOSSAOIntensity" ); };
		p_proxy.getCallback<E_RENDER_SETTINGS::BLUR_SIZE>() += [ this, &p_proxy ]()
		{
			setValue( p_proxy.data.blurSize, "BlurXBlurSize" );
			setValue( p_proxy.data.blurSize, "BlurYBlurSize" );
		};
		// Outline.
		p_proxy.getCallback<E_RENDER_SETTINGS::COLOR_OUTLINE>() +=
			[ this, &p_proxy ]() { setValue( p_proxy.data.colorOutline, "OutlineOutlineColor" ); };
		p_proxy.getCallback<E_RENDER_SETTINGS::OUTLINE_SENSITIVITY>() +=
			[ this, &p_proxy ]() { setValue( p_proxy.data.outlineSensitivity, "OutlineOutlineSensitivity" ); };
		p_proxy.getCallback<E_RENDER_SETTINGS::OUTLINE_THICKNESS>() +=
			[ this, &p_proxy ]() { setValue( p_proxy.data.outlineThickness, "OutlineOutlineThickness" ); };
		// Fog.
		p_proxy.getCallback<E_RENDER_SETTINGS::COLOR_FOG>() +=
			[ this, &p_proxy ]() { setValue( p_proxy.data.colorFog, "ShadingShadingFogColor" ); };
		p_proxy.getCallback<E_RENDER_SETTINGS::FOG_NEAR>() +=
			[ this, &p_proxy ]() { setValue( p_proxy.data.fogNear, "ShadingShadingFogNear" ); };
		p_proxy.getCallback<E_RENDER_SETTINGS::FOG_FAR>() +=
			[ this, &p_proxy ]() { setValue( p_proxy.data.fogFar, "ShadingShadingFogFar" ); };
		p_proxy.getCallback<E_RENDER_SETTINGS::FOG_DENSITY>() +=
			[ this, &p_proxy ]() { setValue( p_proxy.data.fogDensity, "ShadingShadingFogDensity" ); };
		// Selection.
		p_proxy.getCallback<E_RENDER_SETTINGS::COLOR_SELECTION>() +=
			[ this, &p_proxy ]() { setValue( p_proxy.data.colorSelection, "SelectionSelectionColor" ); };

		// Active.
		p_proxy.getCallback<E_RENDER_SETTINGS::ACTIVE_FOG>() += [ this, &p_proxy ]()
		{ setValue( p_proxy.data.activeFog ? p_proxy.data.fogDensity : 0.f, "ShadingShadingFogDensity" ); };

		p_proxy.getCallback<E_RENDER_SETTINGS::ACTIVE_SSAO>() += [ this, &p_proxy ]()
		{
			_refreshGraph();
			build();
			if ( p_proxy.data.activeSSAO )
			{
				setValue( p_proxy.data.ssaoIntensity, "SSAOSSAOIntensity" );
				setValue( p_proxy.data.blurSize, "BlurXBlurSize" );
				setValue( p_proxy.data.blurSize, "BlurYBlurSize" );
			}
		};
		p_proxy.getCallback<E_RENDER_SETTINGS::ACTIVE_OUTLINE>() += [ this, &p_proxy ]()
		{
			_refreshGraph();
			build();
			if ( p_proxy.data.activeOutline )
			{
				setValue( p_proxy.data.colorOutline, "OutlineOutlineColor" );
				setValue( p_proxy.data.outlineSensitivity, "OutlineOutlineSensitivity" );
				setValue( p_proxy.data.outlineThickness, "OutlineOutlineThickness" );
			}
		};
		p_proxy.getCallback<E_RENDER_SETTINGS::ACTIVE_SELECTION>() += [ this, &p_proxy ]()
		{
			_refreshGraph();
			build();
			if ( p_proxy.data.activeSelection )
			{
				setValue( p_proxy.data.colorSelection, "SelectionSelectionColor" );
			}
		};

		setNeedUpdate( true );
	}

	void Renderer::setProxyVoxels( Proxy::Voxels & p_proxy )
	{
		_proxyVoxels = &p_proxy;

		assert( p_proxy.mins );
		assert( p_proxy.maxs );
		assert( p_proxy.mins->size() == p_proxy.maxs->size() );

		_context.set( *p_proxy.mins, "VoxelsMins" );
		_context.set( *p_proxy.maxs, "VoxelsMaxs" );

		drawRangeVoxels.offsets = { 0 };
		drawRangeVoxels.counts	= { uint( p_proxy.mins->size() ) };

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
		_context.reserveData<Vec3f>( totalAtoms, "SpheresCylindersPositions" );
		_context.reserveData<uchar>( totalAtoms, "SpheresCylindersColors" );
		_context.reserveData<float>( totalAtoms, "SpheresCylindersRadii" );
		_context.reserveData<uint>( totalAtoms, "SpheresCylindersIds" );
		_context.reserveData<uchar>( totalAtoms, "SpheresCylindersFlags" );
		_context.reserveData<ushort>( totalAtoms, "SpheresCylindersModels" );
		_context.reserveData<uchar>( totalAtoms, "SpheresCylindersRepresentations" );
		_context.reserveData<uint>( totalBonds, "SpheresCylindersIdx" );

		size_t offsetAtoms = 0;
		size_t offsetBonds = 0;
		ushort modelId	   = 0;
		for ( const Proxy::System * const proxy : _proxiesSystems )
		{
			Cache::SphereCylinder & cache = _cacheSpheresCylinders[ proxy ];

			const size_t atomCount = proxy->atomPositions->size();
			const size_t bondCount = proxy->bonds->size();

			// Fill buffers.
			_context.setSub( *proxy->atomPositions, "SpheresCylindersPositions", offsetAtoms );
			_context.setSub( proxy->atomColors, "SpheresCylindersColors", offsetAtoms );
			_context.setSub( proxy->atomRadii, "SpheresCylindersRadii", offsetAtoms );
			_context.setSub( proxy->atomIds, "SpheresCylindersIds", offsetAtoms );

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

			_context.setSub( cache.flags, "SpheresCylindersFlags", offsetAtoms );
			_context.setSub( std::vector<ushort>( atomCount, modelId ), "SpheresCylindersModels", offsetAtoms );
			_context.setSub( cache.representations, "SpheresCylindersRepresentations", offsetAtoms );

			// Move bonds.
			std::vector<uint> bonds( bondCount );
			for ( size_t i = 0; i < bondCount; ++i )
			{
				bonds[ i ] = uint( ( *proxy->bonds )[ i ] + offsetAtoms );
			}
			_context.setSub( bonds, "SpheresCylindersIdx", offsetBonds );

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

		drawRangeSpheresRL.toStdVectorsFirstCount<void *, uint>( drawRangeSpheres.offsets, drawRangeSpheres.counts );
		drawRangeCylindersRL.toStdVectorsFirstCount<void *, uint>(
			drawRangeCylinders.offsets, drawRangeCylinders.counts
		);
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

		_context.reserveData<Vec4f>( totalCaPositions, "RibbonsPositions" );
		_context.reserveData<Vec3f>( totalCaPositions, "RibbonsDirections" );
		_context.reserveData<uchar>( totalCaPositions, "RibbonsTypes" );
		_context.reserveData<uchar>( totalCaPositions, "RibbonsColors" );
		_context.reserveData<uint>( totalCaPositions, "RibbonsIds" );
		_context.reserveData<uchar>( totalCaPositions, "RibbonsFlags" );
		_context.reserveData<ushort>( totalCaPositions, "RibbonsModels" );
		_context.reserveData<uchar>( totalCaPositions, "RibbonsRepresentations" );
		_context.reserveData<uint>( totalIndices, "RibbonsIdx" );

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

			// Offsets.
			cache.range = Util::Math::Range<size_t> { offsetCaPositions, cache.positions.size() };
			offsetCaPositions += cache.positions.size();
			offsetIndices += cache.indices.size();
		}

		// Ranges.
		drawRangeRibbonsRL.clear();
		drawRangeRibbonsRL.addRange( Util::Math::Range<size_t> { 0, uint( offsetIndices ) } );
		drawRangeRibbonsRL.toStdVectorsFirstCount<void *, uint>( drawRangeRibbons.offsets, drawRangeRibbons.counts );
	}

	void Renderer::_refreshDataModels()
	{
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
		const Mat4f & matrixProjectionOld = *_proxyCamera->matrixProjection;
		Mat4f		  matrixProjection	  = Util::Math::perspective(
			   Util::Math::radians( p_fov ), float( p_width ) / float( p_height ), p_near, p_far
		   );
		setValue( matrixProjection, "CameraMatrixProjection" );
		_context.snapshot( p_outImage, _graph.getRenderQueue(), _instructions, p_width, p_height );
		setValue( matrixProjectionOld, "CameraMatrixProjection" );
	}

	void Renderer::_renderLog( const float p_deltaTime, const float p_elapsedTime )
	{
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
	}

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

	// TODO: not the best way to do it.
	void Renderer::_refreshGraph()
	{
		using namespace Proxy;

		static Pass * geo;
		static Pass * depth;
		static Pass * ssao;
		static Pass * blurX;
		static Pass * blurY;
		static Pass * shading;
		static Pass * outline;
		static Pass * selection;
		static Pass * fxaa;

		// Geometric.
		if ( not geo )
		{
			// TODO: refacto!
			geo									   = _graph.addPass( descPassGeometric );
			geo->programs[ 0 ].draw.value().ranges = &drawRangeSpheres;
			geo->programs[ 0 ].draw.value().needRenderFunc
				= [ this ]() { return showAtoms && drawRangeSpheres.counts.size() > 0; };
			geo->programs[ 1 ].draw.value().ranges = &drawRangeCylinders;
			geo->programs[ 1 ].draw.value().needRenderFunc
				= [ this ]() { return showBonds && drawRangeCylinders.counts.size() > 0; };
			geo->programs[ 2 ].draw.value().ranges = &drawRangeRibbons;
			geo->programs[ 2 ].draw.value().needRenderFunc
				= [ this ]() { return showRibbons && drawRangeRibbons.counts.size() > 0; };
			geo->programs[ 3 ].draw.value().ranges = &drawRangeVoxels;
			geo->programs[ 3 ].draw.value().needRenderFunc
				= [ this ]() { return showVoxels && drawRangeVoxels.counts.size() > 0; };
			// TODO: add SES range.
			/*
			geo->programs[ 4 ].draw.value().ranges = &drawRangeSESCircles;
			geo->programs[ 4 ].draw.value().needRenderFunc
				= [ this ]() { return showSESCircles && drawRangeSESCircles.counts.size() > 0; };
			geo->programs[ 5 ].draw.value().ranges = &drawRangeSESConcaves;
			geo->programs[ 5 ].draw.value().needRenderFunc
				= [ this ]() { return showSESConcaves && drawRangeSESConcaves.counts.size() > 0; };
			geo->programs[ 6 ].draw.value().ranges = &drawRangeSESConvexes;
			geo->programs[ 6 ].draw.value().needRenderFunc
				= [ this ]() { return showSESConvexes && drawRangeSESConvexes.counts.size() > 0; };
			geo->programs[ 7 ].draw.value().ranges = &drawRangeSESSegments;
			geo->programs[ 7 ].draw.value().needRenderFunc
				= [ this ]() { return showSESSegments && drawRangeSESSegments.counts.size() > 0; };
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

		// Depth.
		if ( not depth )
		{
			depth = _graph.addPass( descPassDepth );

			_graph.addLink( geo, depth, E_CHAN_OUT::DEPTH, E_CHAN_IN::_0 );
		}

		// SSAO.
		if ( not ssao )
		{
			if ( not _proxyRenderSettings or _proxyRenderSettings->data.activeSSAO )
			{
				ssao  = _graph.addPass( descPassSSAO );
				blurX = _graph.addPass( descPassBlur );
				blurY = _graph.addPass( descPassBlur );

				blurX->name							 = "BlurX";
				blurY->name							 = "BlurY";
				blurY->programs[ 0 ].data[ 0 ].value = BufferValue<Vec2i> { Vec2i( 0, 1 ) };

				_graph.addLink( geo, ssao, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
				_graph.addLink( depth, ssao, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_2 );
				_graph.addLink( ssao, blurX, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
				_graph.addLink( depth, blurX, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_1 );
				_graph.addLink( blurX, blurY, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
				_graph.addLink( depth, blurY, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_1 );
			}
		}
		else if ( _proxyRenderSettings and not _proxyRenderSettings->data.activeSSAO )
		{
			_graph.removePass( ssao );
			_graph.removePass( blurX );
			_graph.removePass( blurY );
			ssao  = nullptr;
			blurX = nullptr;
			blurY = nullptr;
		}

		// Shading.
		if ( not shading )
		{
			shading = _graph.addPass( descPassShading );

			_graph.addLink( geo, shading, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
			_graph.addLink( geo, shading, E_CHAN_OUT::COLOR_1, E_CHAN_IN::_1 );
		}
		if ( ssao )
		{
			_graph.addLink( blurY, shading, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_2 );
		}

		// Outline.
		if ( not outline )
		{
			if ( not _proxyRenderSettings or _proxyRenderSettings->data.activeOutline )
			{
				outline = _graph.addPass( descPassOutline );

				_graph.addLink( shading, outline, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
				_graph.addLink( depth, outline, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_1 );
			}
		}
		else if ( _proxyRenderSettings and not _proxyRenderSettings->data.activeOutline )
		{
			_graph.removePass( outline );
			outline = nullptr;
		}

		// Selection.
		if ( not selection )
		{
			if ( not _proxyRenderSettings or _proxyRenderSettings->data.activeSelection )
			{
				selection = _graph.addPass( descPassSelection );

				_graph.addLink( geo, selection, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
				_graph.addLink( depth, selection, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_2 );
			}
		}
		else if ( _proxyRenderSettings and not _proxyRenderSettings->data.activeSelection )
		{
			_graph.removePass( selection );
			selection = nullptr;
		}
		if ( selection )
		{
			if ( outline )
			{
				_graph.addLink( outline, selection, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_1 );
			}
			else
			{
				_graph.addLink( shading, selection, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_1 );
			}
		}

		// FXAA.
		if ( not fxaa )
		{
			fxaa = _graph.addPass( desPassFXAA );
			_graph.setOutput( &fxaa->outputs[ E_CHAN_OUT::COLOR_0 ] );
		}
		if ( selection )
		{
			_graph.addLink( selection, fxaa, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
		}
		else if ( outline )
		{
			_graph.addLink( outline, fxaa, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
		}
		else
		{
			_graph.addLink( shading, fxaa, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
		}
	}
} // namespace VTX::Renderer
