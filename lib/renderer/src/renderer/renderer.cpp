#include "renderer/renderer.hpp"
#include <util/math.hpp>
#include <util/math/aabb.hpp>
#include <util/math/grid.hpp>
#include <util/math/range.hpp>
#include <util/string.hpp>

namespace VTX::Renderer
{

	Renderer::Renderer( const size_t p_width, const size_t p_height, const FilePath & p_shaderPath, void * p_loader ) :
		width( p_width ), height( p_height ), _shaderPath( p_shaderPath ), _loader( p_loader )
	{
		// Graph.
		_renderGraph = std::make_unique<RenderGraphOpenGL45>();

		// Passes.
		_refreshGraph();

		// Shared uniforms.
		_renderGraph->addUniforms(
			{ "Camera",
			  { { "Matrix view", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
				{ "Matrix projection", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
				{ "Camera position", E_TYPE::VEC3F, StructUniformValue<Vec3f> { VEC3F_ZERO } },
				{ "Camera clip infos", // { _near * _far, _far, _far - _near, _near }
				  E_TYPE::VEC4F,
				  StructUniformValue<Vec4f> { VEC4F_ZERO } },
				{ "Resolution", E_TYPE::VEC2I, StructUniformValue<Vec2i> { Vec2i { p_width, p_height } } },
				{ "Mouse position", E_TYPE::VEC2I, StructUniformValue<Vec2i> { Vec2i { 0, 0 } } },
				{ "Is perspective", E_TYPE::UINT, StructUniformValue<uint> { 1 } } },
			  15 }
		);

		_renderGraph->addUniforms(
			{ "Color layout", { { "Colors", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> {} } }, 14, true }
		);

		_renderGraph->addUniforms( { "Models",
									 { { "Matrix model view", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
									   { "Matrix normal", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } } },
									 13,
									 true } );

		_renderGraph->addUniforms( { "Representations",
									 { { "Sphere radius fixed", E_TYPE::FLOAT, StructUniformValue<float> {} },
									   { "Sphere radius add", E_TYPE::FLOAT, StructUniformValue<float> {} },
									   { "Is sphere radius fixed", E_TYPE::UINT, StructUniformValue<uint> {} },
									   { "Cylinder radius", E_TYPE::FLOAT, StructUniformValue<float> {} },

									   { "Cylinder color blending", E_TYPE::UINT, StructUniformValue<uint> {} },
									   { "Ribbon color blending", E_TYPE::UINT, StructUniformValue<uint> {} } },
									 12,
									 true } );
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
			geo									   = _renderGraph->addPass( descPassGeometric );
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
		}

		// Depth.
		if ( not depth )
		{
			depth = _renderGraph->addPass( descPassDepth );

			_renderGraph->addLink( geo, depth, E_CHAN_OUT::DEPTH, E_CHAN_IN::_0 );
		}

		// SSAO.
		if ( not ssao )
		{
			if ( not _proxyRenderSettings or _proxyRenderSettings->get<bool>( E_RENDER_SETTINGS::ACTIVE_SSAO ) )
			{
				ssao  = _renderGraph->addPass( descPassSSAO );
				blurX = _renderGraph->addPass( descPassBlur );
				blurY = _renderGraph->addPass( descPassBlur );

				blurX->name								 = "BlurX";
				blurY->name								 = "BlurY";
				blurY->programs[ 0 ].uniforms[ 0 ].value = StructUniformValue<Vec2i> { Vec2i( 0, 1 ) };

				_renderGraph->addLink( geo, ssao, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
				_renderGraph->addLink( depth, ssao, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_2 );
				_renderGraph->addLink( ssao, blurX, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
				_renderGraph->addLink( depth, blurX, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_1 );
				_renderGraph->addLink( blurX, blurY, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
				_renderGraph->addLink( depth, blurY, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_1 );
			}
		}
		else if ( _proxyRenderSettings and not _proxyRenderSettings->get<bool>( E_RENDER_SETTINGS::ACTIVE_SSAO ) )
		{
			_renderGraph->removePass( ssao );
			_renderGraph->removePass( blurX );
			_renderGraph->removePass( blurY );
			ssao  = nullptr;
			blurX = nullptr;
			blurY = nullptr;
		}

		// Shading.
		if ( not shading )
		{
			shading = _renderGraph->addPass( descPassShading );

			_renderGraph->addLink( geo, shading, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
			_renderGraph->addLink( geo, shading, E_CHAN_OUT::COLOR_1, E_CHAN_IN::_1 );
		}
		if ( ssao )
		{
			_renderGraph->addLink( blurY, shading, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_2 );
		}

		// Outline.
		if ( not outline )
		{
			if ( not _proxyRenderSettings or _proxyRenderSettings->get<bool>( E_RENDER_SETTINGS::ACTIVE_OUTLINE ) )
			{
				outline = _renderGraph->addPass( descPassOutline );

				_renderGraph->addLink( shading, outline, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
				_renderGraph->addLink( depth, outline, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_1 );
			}
		}
		else if ( _proxyRenderSettings and not _proxyRenderSettings->get<bool>( E_RENDER_SETTINGS::ACTIVE_OUTLINE ) )
		{
			_renderGraph->removePass( outline );
			outline = nullptr;
		}

		// Selection.
		if ( not selection )
		{
			if ( not _proxyRenderSettings or _proxyRenderSettings->get<bool>( E_RENDER_SETTINGS::ACTIVE_SELECTION ) )
			{
				selection = _renderGraph->addPass( descPassSelection );

				_renderGraph->addLink( geo, selection, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
				_renderGraph->addLink( depth, selection, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_2 );
			}
		}
		else if ( _proxyRenderSettings and not _proxyRenderSettings->get<bool>( E_RENDER_SETTINGS::ACTIVE_SELECTION ) )
		{
			_renderGraph->removePass( selection );
			selection = nullptr;
		}
		if ( selection )
		{
			if ( outline )
			{
				_renderGraph->addLink( outline, selection, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_1 );
			}
			else
			{
				_renderGraph->addLink( shading, selection, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_1 );
			}
		}

		// FXAA.
		if ( not fxaa )
		{
			fxaa = _renderGraph->addPass( desPassFXAA );
			_renderGraph->setOutput( &fxaa->outputs[ E_CHAN_OUT::COLOR_0 ] );
		}
		if ( selection )
		{
			_renderGraph->addLink( selection, fxaa, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
		}
		else if ( outline )
		{
			_renderGraph->addLink( outline, fxaa, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
		}
		else
		{
			_renderGraph->addLink( shading, fxaa, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
		}
	}

	void Renderer::build( const uint p_output, void * p_loader )
	{
		bool isFirstBuild = not hasContext();

		// Build renderer graph.
		VTX_DEBUG(
			"Renderer graph setup total time: {}",
			Util::String::durationToStr( Util::CHRONO_CPU(
				[ & ]()
				{
					_context = _renderGraph->setup(
						p_loader ? p_loader : _loader,
						width,
						height,
						_shaderPath,
						_instructions,
						_instructionsDurationRanges,
						p_output
					);
				}
			) )
		);

		std::string str = "Passes: ";
		for ( const Pass * const pass : _renderGraph->getRenderQueue() )
		{
			str += pass->name + " -> ";
		}
		str += "Output";
		VTX_DEBUG( "{}", str );

		if ( isFirstBuild )
		{
			onReady();
		}

		///////////////////// COMPUTE
		uint size = 10000;

		ComputePass::Data bufferReadOnly { size * sizeof( Vec4f ), nullptr, 1 };
		ComputePass::Data bufferWriteOnly { size * sizeof( Vec4f ), nullptr, 2 };
		ComputePass::Data bufferReadWrite { size * sizeof( Vec4f ), nullptr, 3 };

		auto computePass = ComputePass {
			Program { "ComputeDebug",
					  std::vector<FilePath> { "compute/debug.comp" },
					  Uniforms { { { "Intensity", E_TYPE::UINT, StructUniformValue<uint> { size } } } } },
			{ &bufferReadOnly, &bufferWriteOnly, &bufferReadWrite },
			size
		};

		_context->compute( computePass );

		_context->clearComputeBuffers();
	}

	void Renderer::clean()
	{
		_context = nullptr;
		_instructions.clear();
		_instructionsDurationRanges.clear();
		_renderGraph->clean();
		_needUpdate		 = false;
		_framesRemaining = 0;

		_proxiesSystems.clear();
		_proxyCamera	  = nullptr;
		_proxyColorLayout = nullptr;
		_proxyRepresentations.clear();
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
	}

#pragma region Proxy systems

	void Renderer::addProxySystem( Proxy::System & p_proxy )
	{
		_addProxySystem( p_proxy );
		_refreshDataSystems();
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
		assert( hasContext() );
		assert( p_proxy.idDefaultRepresentation < _proxyRepresentations.size() );

		// If size max reached, do not add.
		if ( _proxiesSystems.size() >= UNSIGNED_SHORT_MAX )
		{
			throw GLException( "Max system count reached" );
		}

		_proxiesSystems.push_back( &p_proxy );
		_cacheSpheresCylinders.emplace( &p_proxy, Cache::SphereCylinder() );
		_cacheRibbons.emplace( &p_proxy, Cache::Ribbon() );

		// Set up callbacks.
		// Transform.
		p_proxy.onTransform += [ this, &p_proxy ]()
		{
			const Mat4f matrixModelView = *_proxyCamera->matrixView * *p_proxy.transform;
			const Mat4f matrixNormal	= Util::Math::transpose( Util::Math::inverse( matrixModelView ) );

			setValue( _StructUBOModel { matrixModelView, matrixNormal }, "Matrix model view", _getProxyId( &p_proxy ) );
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

			drawRangeSpheresRL.toVectors<void *, uint>( drawRangeSpheres.offsets, drawRangeSpheres.counts );
			drawRangeCylindersRL.toVectors<void *, uint>( drawRangeCylinders.offsets, drawRangeCylinders.counts );
			drawRangeRibbonsRL.toVectors<void *, uint>( drawRangeRibbons.offsets, drawRangeRibbons.counts );
		};

		// Representation.
		p_proxy.onRepresentation += [ this, &p_proxy ]( const uchar p_representation )
		{
			Cache::SphereCylinder & cacheSC = _cacheSpheresCylinders[ &p_proxy ];
			Cache::Ribbon &			cacheR	= _cacheRibbons[ &p_proxy ];

			cacheSC.representations = std::vector<uchar>( cacheSC.rangeSpheres.getCount(), p_representation );
			cacheR.representations	= std::vector<uchar>( cacheR.range.getCount(), p_representation );

			_context->setSubData(
				cacheSC.representations, "SpheresCylindersRepresentations", cacheSC.rangeSpheres.getFirst()
			);
			_context->setSubData( cacheR.representations, "RibbonsRepresentations", cacheR.range.getFirst() );
		};

		// Remove.
		p_proxy.onRemove += [ this, &p_proxy ]() { removeProxySystem( p_proxy ); };

		// Positions.
		p_proxy.onAtomPositions += [ this, &p_proxy ]()
		{
			Cache::SphereCylinder & cacheSC = _cacheSpheresCylinders[ &p_proxy ];
			_context->setSubData(
				*p_proxy.atomPositions, "SpheresCylindersPositions", cacheSC.rangeSpheres.getFirst()
			);
		};

		// Colors.
		p_proxy.onAtomColors += [ this, &p_proxy ]( const std::vector<uchar> & p_colors )
		{
			Cache::SphereCylinder & cacheSC = _cacheSpheresCylinders[ &p_proxy ];
			_context->setSubData( p_colors, "SpheresCylindersColors", cacheSC.rangeSpheres.getFirst() );
		};

		// Residue colors.
		p_proxy.onResidueColors += [ this, &p_proxy ]( const std::vector<uchar> & p_colors )
		{
			Cache::Ribbon & cacheR = _cacheRibbons[ &p_proxy ];
			_context->setSubData( p_colors, "RibbonsColors", cacheR.range.getFirst() );
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
			_context->setSubData( cacheSC.flags, "SpheresCylindersFlags", cacheSC.rangeSpheres.getFirst() );

			for ( size_t i = 0; i < cacheR.range.getCount(); ++i )
			{
				cacheR.flags[ i ] &= ~mask;
				cacheR.flags[ i ] |= p_select << E_ELEMENT_FLAGS::SELECTION;
			}
			_context->setSubData( cacheR.flags, "RibbonsFlags", cacheR.range.getFirst() );
		};

		p_proxy.onAtomSelections +=
			[ this, &p_proxy ]( const Util::Math::RangeList<uint> & p_atomIds, const bool p_select )
		{
			Cache::SphereCylinder & cacheSC = _cacheSpheresCylinders[ &p_proxy ];
			Cache::Ribbon &			cacheR	= _cacheRibbons[ &p_proxy ];
			uchar					mask	= 1 << E_ELEMENT_FLAGS::SELECTION;
			VTX_DEBUG( "=================================== ON ATOM SELECTIONS" );
			for ( auto it = p_atomIds.rangeBegin(); it != p_atomIds.rangeEnd(); ++it )
			{
				VTX_DEBUG( "=================================== RANGE" );
				for ( uint i = it->getFirst(); i <= it->getLast(); ++i )
				{
					cacheSC.flags[ i ] &= ~mask;
					cacheSC.flags[ i ] |= p_select << E_ELEMENT_FLAGS::SELECTION;

					VTX_DEBUG( "Select atom: {} -> {}", i, p_select );
				}
			}

			/*
			for ( const uint i : p_atomIds )
			{
				cacheSC.flags[ i ] &= ~mask;
				cacheSC.flags[ i ] |= p_select << E_ELEMENT_FLAGS::SELECTION;
			}
			*/

			_context->setSubData( cacheSC.flags, "SpheresCylindersFlags", cacheSC.rangeSpheres.getFirst() );

			// TODO: ribbons and SES.
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
			_context->setSubData( cacheSC.flags, "SpheresCylindersFlags", cacheSC.offset );

			for ( size_t i = 0; i < cacheR.size; ++i )
			{
				cacheR.bufferFlags[ i ] &= ~mask;
				cacheR.bufferFlags[ i ] |= p_visible << E_ELEMENT_FLAGS::VISIBILITY;
			}
			_context->setSubData( cacheR.bufferFlags, "RibbonsFlags", cacheR.offset );
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

	void Renderer::addProxyRepresentation( Proxy::Representation & p_proxy ) {}

	void Renderer::removeProxyRepresentation( Proxy::Representation & p_proxy ) {}

	void Renderer::_applyRepresentationLogic( Proxy::Representation * const p_representation )
	{
		using namespace Proxy;

		bool hasSphere	 = p_representation->get<bool>( E_REPRESENTATION_SETTINGS::HAS_SPHERE );
		bool hasCylinder = p_representation->get<bool>( E_REPRESENTATION_SETTINGS::HAS_CYLINDER );
		bool hasRibbon	 = p_representation->get<bool>( E_REPRESENTATION_SETTINGS::HAS_RIBBON );

		showAtoms	= hasSphere;
		showBonds	= hasCylinder;
		showRibbons = hasRibbon;

		float cylinderRadius = p_representation->get<float>( E_REPRESENTATION_SETTINGS::RADIUS_CYLINDER );

		// Spheres asked.
		if ( hasSphere )
		{
			const bool isSphereRadiusFixed
				= p_representation->get<bool>( E_REPRESENTATION_SETTINGS::IS_SPHERE_RADIUS_FIXED );
			float sphereRadiusFixed = p_representation->get<float>( E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_FIXED );
			float sphereRadiusAdd	= p_representation->get<float>( E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_ADD );

			// Scale sphere radius to cylinder radius.
			if ( isSphereRadiusFixed && sphereRadiusFixed < cylinderRadius )
			{
				sphereRadiusFixed = cylinderRadius;
			}

			setValue( sphereRadiusFixed, "Sphere radius fixed", 0 );
			setValue( sphereRadiusAdd, "Sphere radius add", 0 );
			setValue( uint( isSphereRadiusFixed ), "Is sphere radius fixed", 0 );

			if ( not isSphereRadiusFixed )
			{
				showBonds	= false;
				showRibbons = false;
			}
		}
		// No spheres asked.
		else
		{
			// Cylinders asked, force spheres at the same radius.
			if ( hasCylinder )
			{
				showAtoms = true;
				setValue( uint( true ), "Is sphere radius fixed", 0 );
				setValue( cylinderRadius, "Sphere radius fixed", 0 );
			}
			// Hide.
			else
			{
				showAtoms = false;
			}
		}

		if ( hasCylinder )
		{
			setValue( cylinderRadius, "Cylinder radius", 0 );
		}
	}

	void Renderer::addProxyRepresentations( std::vector<Proxy::Representation *> & p_proxies )
	{
		using namespace Proxy;
		assert( hasContext() );

		_proxyRepresentations.insert(
			std::end( _proxyRepresentations ), std::begin( p_proxies ), std::end( p_proxies )
		);

		std::vector<_StructUBORepresentation> representations;
		for ( Proxy::Representation * const representation : _proxyRepresentations )
		{
			representations.emplace_back( _StructUBORepresentation {
				representation->get<float>( E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_FIXED ),
				representation->get<float>( E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_ADD ),
				representation->get<bool>( E_REPRESENTATION_SETTINGS::IS_SPHERE_RADIUS_FIXED ),
				representation->get<float>( E_REPRESENTATION_SETTINGS::RADIUS_CYLINDER ),
				representation->get<bool>( E_REPRESENTATION_SETTINGS::CYLINDER_COLOR_BLENDING ),
				representation->get<bool>( E_REPRESENTATION_SETTINGS::RIBBON_COLOR_BLENDING ) } );

			// showAtoms	= representation->get<bool>( E_REPRESENTATION_SETTINGS::HAS_SPHERE );
			showBonds	= representation->get<bool>( E_REPRESENTATION_SETTINGS::HAS_CYLINDER );
			showRibbons = representation->get<bool>( E_REPRESENTATION_SETTINGS::HAS_RIBBON );

			_applyRepresentationLogic( representation );

			// Callbacks.
			representation->onChange<E_REPRESENTATION_SETTINGS::HAS_SPHERE, bool>() +=
				[ this, representation ]( const bool p_value ) { _applyRepresentationLogic( representation ); };
			representation->onChange<E_REPRESENTATION_SETTINGS::IS_SPHERE_RADIUS_FIXED, bool>() +=
				[ this, representation ]( const bool p_value ) { _applyRepresentationLogic( representation ); };
			representation->onChange<E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_FIXED, float>() +=
				[ this, representation ]( const float p_value ) { _applyRepresentationLogic( representation ); };
			representation->onChange<E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_ADD, float>() +=
				[ this, representation ]( const float p_value ) { _applyRepresentationLogic( representation ); };

			representation->onChange<E_REPRESENTATION_SETTINGS::HAS_CYLINDER, bool>() +=
				[ this, representation ]( const bool p_value ) { _applyRepresentationLogic( representation ); };
			representation->onChange<E_REPRESENTATION_SETTINGS::RADIUS_CYLINDER, float>() +=
				[ this, representation ]( const float p_value ) { _applyRepresentationLogic( representation ); };
			representation->onChange<E_REPRESENTATION_SETTINGS::CYLINDER_COLOR_BLENDING, bool>() +=
				[ this ]( const bool p_value ) { setValue( uint( p_value ), "Cylinder color blending", 0 ); };

			representation->onChange<E_REPRESENTATION_SETTINGS::HAS_RIBBON, bool>() +=
				[ this, representation ]( const bool p_value ) { _applyRepresentationLogic( representation ); };
			representation->onChange<E_REPRESENTATION_SETTINGS::RIBBON_COLOR_BLENDING, bool>() +=
				[ this ]( const bool p_value ) { setValue( uint( p_value ), "Ribbon color blending", 0 ); };
		}

		_context->setData( representations, "Representations" );

		// TODO: remove useless primitives with multi calls.
		// TODO: compute ss if needed
		// TODO: delete others ss from cache?

		setNeedUpdate( true );
	}

	void Renderer::removeProxyRepresentations( std::vector<Proxy::Representation *> & p_proxies ) {}

#pragma endregion Proxy representations

	void Renderer::setProxyCamera( Proxy::Camera & p_proxy )
	{
		assert( hasContext() );
		assert( p_proxy.matrixView );
		assert( p_proxy.matrixProjection );

		_proxyCamera = &p_proxy;

		_context->setData<_StructUBOCamera>(
			{ { *p_proxy.matrixView,
				*p_proxy.matrixProjection,
				p_proxy.cameraPosition,
				0,
				Vec4f(
					p_proxy.cameraNear * p_proxy.cameraFar,
					p_proxy.cameraFar,
					p_proxy.cameraFar - p_proxy.cameraNear,
					p_proxy.cameraNear
				),
				Vec2i( width, height ),
				p_proxy.mousePosition,
				p_proxy.isPerspective } },
			"Camera"
		);

		p_proxy.onMatrixView += [ this, &p_proxy ]()
		{
			setValue( *p_proxy.matrixView, "Matrix view" );
			_refreshDataModels();
		};

		p_proxy.onMatrixProjection +=
			[ this, &p_proxy ]() { setValue( *p_proxy.matrixProjection, "Matrix projection" ); };

		p_proxy.onCameraPosition +=
			[ this, &p_proxy ]( const Vec3f & p_position ) { setValue( p_position, "Camera position" ); };

		p_proxy.onCameraNearFar += [ this, &p_proxy ]( const float p_near, const float p_far )
		{ setValue( Vec4f( p_near * p_far, p_far, p_far - p_near, p_near ), "Camera clip infos" ); };

		p_proxy.onMousePosition += [ this, &p_proxy ]( const Vec2i & p_position )
		{
			// setUniform( Vec2i { p_position.x, height - p_position.y }, "Mouse position" );
		};

		p_proxy.onPerspective +=
			[ this, &p_proxy ]( const bool p_perspective ) { setValue( uint( p_perspective ), "Is perspective" ); };
	}

	void Renderer::setProxyColorLayout( Proxy::ColorLayout & p_proxy )
	{
		assert( hasContext() );

		_proxyColorLayout = &p_proxy;
		_context->setData<Util::Color::Rgba>( *p_proxy.colors, "Color layout" );
		setNeedUpdate( true );

		p_proxy.onChange += [ this, &p_proxy ]()
		{
			_context->setData<Util::Color::Rgba>( *p_proxy.colors, "Color layout" );
			setNeedUpdate( true );
		};
	}

	void Renderer::setProxyRenderSettings( Proxy::RenderSettings & p_proxy )
	{
		using namespace Proxy;

		assert( hasContext() );

		_proxyRenderSettings = &p_proxy;

		_refreshGraph();
		build();

		// Default values.
		// Shading.
		setValue( p_proxy.get<uint>( E_RENDER_SETTINGS::SHADING_MODE ), "ShadingShadingMode" );
		setValue( p_proxy.get<Util::Color::Rgba>( E_RENDER_SETTINGS::COLOR_LIGHT ), "ShadingShadingLight color" );
		setValue(
			p_proxy.get<Util::Color::Rgba>( E_RENDER_SETTINGS::COLOR_BACKGROUND ), "ShadingShadingBackground color"
		);
		setValue( p_proxy.get<float>( E_RENDER_SETTINGS::SPECULAR_FACTOR ), "ShadingShadingSpecular factor" );
		setValue( p_proxy.get<float>( E_RENDER_SETTINGS::SHININESS ), "ShadingShadingShininess" );
		setValue( p_proxy.get<uint>( E_RENDER_SETTINGS::TOON_STEPS ), "ShadingShadingToon steps" );
		// SSAO.
		if ( p_proxy.get<bool>( E_RENDER_SETTINGS::ACTIVE_SSAO ) )
		{
			setValue( p_proxy.get<float>( E_RENDER_SETTINGS::SSAO_INTENSITY ), "SSAOSSAOIntensity" );
			setValue( p_proxy.get<float>( E_RENDER_SETTINGS::BLUR_SIZE ), "BlurXBlurSize" );
			setValue( p_proxy.get<float>( E_RENDER_SETTINGS::BLUR_SIZE ), "BlurYBlurSize" );
		}
		// Outline.
		if ( p_proxy.get<bool>( E_RENDER_SETTINGS::ACTIVE_OUTLINE ) )
		{
			setValue( p_proxy.get<Util::Color::Rgba>( E_RENDER_SETTINGS::COLOR_OUTLINE ), "OutlineOutlineColor" );
			setValue( p_proxy.get<float>( E_RENDER_SETTINGS::OUTLINE_SENSITIVITY ), "OutlineOutlineSensitivity" );
			setValue( p_proxy.get<uint>( E_RENDER_SETTINGS::OUTLINE_THICKNESS ), "OutlineOutlineThickness" );
		}
		// Fog.
		setValue( p_proxy.get<Util::Color::Rgba>( E_RENDER_SETTINGS::COLOR_FOG ), "ShadingShadingFog color" );
		setValue( p_proxy.get<float>( E_RENDER_SETTINGS::FOG_NEAR ), "ShadingShadingFog near" );
		setValue( p_proxy.get<float>( E_RENDER_SETTINGS::FOG_FAR ), "ShadingShadingFog far" );
		setValue(
			p_proxy.get<bool>( E_RENDER_SETTINGS::ACTIVE_FOG ) ? p_proxy.get<float>( E_RENDER_SETTINGS::FOG_DENSITY )
															   : 0.f,
			"ShadingShadingFog density"
		);
		// Selection.
		if ( p_proxy.get<bool>( E_RENDER_SETTINGS::ACTIVE_SELECTION ) )
		{
			setValue( p_proxy.get<Util::Color::Rgba>( E_RENDER_SETTINGS::COLOR_SELECTION ), "SelectionSelectionColor" );
		}

		// Callbacks.
		// Shading.
		p_proxy.onChange<E_RENDER_SETTINGS::SHADING_MODE, uint>() +=
			[ this ]( const uint p_mode ) { setValue( p_mode, "ShadingShadingMode" ); };

		p_proxy.onChange<E_RENDER_SETTINGS::COLOR_LIGHT, Util::Color::Rgba>() +=
			[ this ]( const Util::Color::Rgba & p_color ) { setValue( p_color, "ShadingShadingLight color" ); };
		p_proxy.onChange<E_RENDER_SETTINGS::COLOR_BACKGROUND, Util::Color::Rgba>() +=
			[ this ]( const Util::Color::Rgba & p_color ) { setValue( p_color, "ShadingShadingBackground color" ); };
		p_proxy.onChange<E_RENDER_SETTINGS::SPECULAR_FACTOR, float>() +=
			[ this ]( const float p_factor ) { setValue( p_factor, "ShadingShadingSpecular factor" ); };
		p_proxy.onChange<E_RENDER_SETTINGS::SHININESS, float>() +=
			[ this ]( const float p_shininess ) { setValue( p_shininess, "ShadingShadingShininess" ); };
		p_proxy.onChange<E_RENDER_SETTINGS::TOON_STEPS, uint>() +=
			[ this ]( const uint p_steps ) { setValue( p_steps, "ShadingShadingToon steps" ); };
		// SSAO.
		p_proxy.onChange<E_RENDER_SETTINGS::SSAO_INTENSITY, float>() +=
			[ this ]( const float p_intensity ) { setValue( p_intensity, "SSAOSSAOIntensity" ); };
		p_proxy.onChange<E_RENDER_SETTINGS::BLUR_SIZE, float>() += [ this ]( const float p_size )
		{
			setValue( p_size, "BlurXBlurSize" );
			setValue( p_size, "BlurYBlurSize" );
		};
		// Outline.
		p_proxy.onChange<E_RENDER_SETTINGS::COLOR_OUTLINE, Util::Color::Rgba>() +=
			[ this ]( const Util::Color::Rgba & p_color ) { setValue( p_color, "OutlineOutlineColor" ); };
		p_proxy.onChange<E_RENDER_SETTINGS::OUTLINE_SENSITIVITY, float>() +=
			[ this ]( const float p_sensivity ) { setValue( p_sensivity, "OutlineOutlineSensitivity" ); };
		p_proxy.onChange<E_RENDER_SETTINGS::OUTLINE_THICKNESS, uint>() +=
			[ this ]( const uint p_thickness ) { setValue( p_thickness, "OutlineOutlineThickness" ); };
		// Fog.
		p_proxy.onChange<E_RENDER_SETTINGS::COLOR_FOG, Util::Color::Rgba>() +=
			[ this ]( const Util::Color::Rgba & p_color ) { setValue( p_color, "ShadingShadingFog color" ); };
		p_proxy.onChange<E_RENDER_SETTINGS::FOG_NEAR, float>() +=
			[ this ]( const float p_near ) { setValue( p_near, "ShadingShadingFog near" ); };
		p_proxy.onChange<E_RENDER_SETTINGS::FOG_FAR, float>() +=
			[ this ]( const float p_far ) { setValue( p_far, "ShadingShadingFog far" ); };
		p_proxy.onChange<E_RENDER_SETTINGS::FOG_DENSITY, float>() +=
			[ this ]( const float p_density ) { setValue( p_density, "ShadingShadingFog density" ); };
		// Selection.
		p_proxy.onChange<E_RENDER_SETTINGS::COLOR_SELECTION, Util::Color::Rgba>() +=
			[ this ]( const Util::Color::Rgba & p_color ) { setValue( p_color, "SelectionSelectionColor" ); };

		// Active.
		p_proxy.onChange<E_RENDER_SETTINGS::ACTIVE_FOG, bool>() += [ this ]( const bool p_active )
		{
			setValue(
				p_active ? _proxyRenderSettings->get<float>( E_RENDER_SETTINGS::FOG_DENSITY ) : 0.f,
				"ShadingShadingFog density"
			);
		};

		p_proxy.onChange<E_RENDER_SETTINGS::ACTIVE_SSAO, bool>() += [ this ]( const bool p_active )
		{
			_refreshGraph();
			build();
			if ( p_active )
			{
				setValue( _proxyRenderSettings->get<float>( E_RENDER_SETTINGS::SSAO_INTENSITY ), "SSAOSSAOIntensity" );
				setValue( _proxyRenderSettings->get<float>( E_RENDER_SETTINGS::BLUR_SIZE ), "BlurXBlurSize" );
				setValue( _proxyRenderSettings->get<float>( E_RENDER_SETTINGS::BLUR_SIZE ), "BlurYBlurSize" );
			}
		};
		p_proxy.onChange<E_RENDER_SETTINGS::ACTIVE_OUTLINE, bool>() += [ this ]( const bool p_active )
		{
			_refreshGraph();
			build();
			if ( p_active )
			{
				setValue(
					_proxyRenderSettings->get<Util::Color::Rgba>( E_RENDER_SETTINGS::COLOR_OUTLINE ),
					"OutlineOutlineColor"
				);
				setValue(
					_proxyRenderSettings->get<float>( E_RENDER_SETTINGS::OUTLINE_SENSITIVITY ),
					"OutlineOutlineSensitivity"
				);
				setValue(
					_proxyRenderSettings->get<uint>( E_RENDER_SETTINGS::OUTLINE_THICKNESS ), "OutlineOutlineThickness"
				);
			}
		};
		p_proxy.onChange<E_RENDER_SETTINGS::ACTIVE_SELECTION, bool>() += [ this ]( const bool p_active )
		{
			_refreshGraph();
			build();
			if ( p_active )
			{
				setValue(
					_proxyRenderSettings->get<Util::Color::Rgba>( E_RENDER_SETTINGS::COLOR_SELECTION ),
					"SelectionSelectionColor"
				);
			}
		};

		setNeedUpdate( true );
	}

	void Renderer::setProxyVoxels( Proxy::Voxels & p_proxy )
	{
		assert( hasContext() );

		_proxyVoxels = &p_proxy;

		assert( p_proxy.mins );
		assert( p_proxy.maxs );
		assert( p_proxy.mins->size() == p_proxy.maxs->size() );

		_context->setData( *p_proxy.mins, "VoxelsMins" );
		_context->setData( *p_proxy.maxs, "VoxelsMaxs" );

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
		_context->reserveData<Vec3f>( totalAtoms, "SpheresCylindersPositions" );
		_context->reserveData<uchar>( totalAtoms, "SpheresCylindersColors" );
		_context->reserveData<float>( totalAtoms, "SpheresCylindersRadii" );
		_context->reserveData<uint>( totalAtoms, "SpheresCylindersIds" );
		_context->reserveData<uchar>( totalAtoms, "SpheresCylindersFlags" );
		_context->reserveData<ushort>( totalAtoms, "SpheresCylindersModels" );
		_context->reserveData<uchar>( totalAtoms, "SpheresCylindersRepresentations" );
		_context->reserveData<uint>( totalBonds, "SpheresCylindersIdx" );

		size_t offsetAtoms = 0;
		size_t offsetBonds = 0;
		ushort modelId	   = 0;
		for ( const Proxy::System * const proxy : _proxiesSystems )
		{
			Cache::SphereCylinder & cache = _cacheSpheresCylinders[ proxy ];

			const size_t atomCount = proxy->atomPositions->size();
			const size_t bondCount = proxy->bonds->size();

			// Fill buffers.
			_context->setSubData( *proxy->atomPositions, "SpheresCylindersPositions", offsetAtoms );
			_context->setSubData( proxy->atomColors, "SpheresCylindersColors", offsetAtoms );
			_context->setSubData( proxy->atomRadii, "SpheresCylindersRadii", offsetAtoms );
			_context->setSubData( proxy->atomIds, "SpheresCylindersIds", offsetAtoms );

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

			_context->setSubData( cache.flags, "SpheresCylindersFlags", offsetAtoms );
			_context->setSubData( std::vector<ushort>( atomCount, modelId ), "SpheresCylindersModels", offsetAtoms );
			_context->setSubData( cache.representations, "SpheresCylindersRepresentations", offsetAtoms );

			// Move bonds.
			std::vector<uint> bonds( bondCount );
			for ( size_t i = 0; i < bondCount; ++i )
			{
				bonds[ i ] = uint( ( *proxy->bonds )[ i ] + offsetAtoms );
			}
			_context->setSubData( bonds, "SpheresCylindersIdx", offsetBonds );

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

		drawRangeSpheresRL.toVectors<void *, uint>( drawRangeSpheres.offsets, drawRangeSpheres.counts );
		drawRangeCylindersRL.toVectors<void *, uint>( drawRangeCylinders.offsets, drawRangeCylinders.counts );
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

		_context->reserveData<Vec4f>( totalCaPositions, "RibbonsPositions" );
		_context->reserveData<Vec3f>( totalCaPositions, "RibbonsDirections" );
		_context->reserveData<uchar>( totalCaPositions, "RibbonsTypes" );
		_context->reserveData<uchar>( totalCaPositions, "RibbonsColors" );
		_context->reserveData<uint>( totalCaPositions, "RibbonsIds" );
		_context->reserveData<uchar>( totalCaPositions, "RibbonsFlags" );
		_context->reserveData<ushort>( totalCaPositions, "RibbonsModels" );
		_context->reserveData<uchar>( totalCaPositions, "RibbonsRepresentations" );
		_context->reserveData<uint>( totalIndices, "RibbonsIdx" );

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

			_context->setSubData( cache.positions, "RibbonsPositions", offsetCaPositions );
			_context->setSubData( cache.directions, "RibbonsDirections", offsetCaPositions );
			_context->setSubData( cache.ssTypes, "RibbonsTypes", offsetCaPositions );
			_context->setSubData( cache.colors, "RibbonsColors", offsetCaPositions );
			_context->setSubData( cache.ids, "RibbonsIds", offsetCaPositions );
			_context->setSubData( cache.flags, "RibbonsFlags", offsetCaPositions );
			_context->setSubData(
				std::vector<ushort>( cache.positions.size(), modelId ), "RibbonsModels", offsetCaPositions
			);
			_context->setSubData( cache.representations, "RibbonsRepresentations", offsetCaPositions );
			_context->setSubData( indices, "RibbonsIdx", offsetIndices );

			// Offsets.
			cache.range = Util::Math::Range<size_t> { offsetCaPositions, cache.positions.size() };
			offsetCaPositions += cache.positions.size();
			offsetIndices += cache.indices.size();
		}

		// Ranges.
		drawRangeRibbonsRL.clear();
		drawRangeRibbonsRL.addRange( Util::Math::Range<size_t> { 0, uint( offsetIndices ) } );
		drawRangeRibbonsRL.toVectors<void *, uint>( drawRangeRibbons.offsets, drawRangeRibbons.counts );
	}

	void Renderer::_refreshDataSES()
	{
		return;
		using namespace Util;
		using namespace Math;

		struct _InputData
		{
			std::vector<uint> atomList;
			Util::Math::AABB  aabb;
			float			  maxRadius = 0.f;
		};

		auto _generateInputData = []( const std::vector<size_t> & p_categoryChainIds,
									  const std::vector<uint> *	  p_residueFirstAtomIndexes,
									  const std::vector<uint> *	  p_residueAtomCounts,
									  const std::vector<size_t> * p_chainFirstResidues,
									  const std::vector<size_t> * p_chainResidueCounts,
									  const std::vector<Vec3f> *  p_atomPositions,
									  const std::vector<float> &  p_atomRadii ) -> _InputData
		{
			_InputData data;

			for ( size_t chainIdx : p_categoryChainIds )
			{
				size_t idxFirstResidue = ( *p_chainFirstResidues )[ chainIdx ];
				size_t residueCount	   = ( *p_chainResidueCounts )[ chainIdx ];

				for ( size_t residueIdx = idxFirstResidue; residueIdx < idxFirstResidue + residueCount; ++residueIdx )
				{
					size_t idxFirstAtom = ( *p_residueFirstAtomIndexes )[ residueIdx ];
					size_t atomCount	= ( *p_residueAtomCounts )[ residueIdx ];

					for ( size_t i = idxFirstAtom; i < idxFirstAtom + atomCount; ++i )
					{
						data.atomList.emplace_back( uint( i ) );
						data.aabb.extend( ( *p_atomPositions )[ i ] );
						data.maxRadius = std::max( data.maxRadius, p_atomRadii[ i ] );
					}
				}
			}

			return data;
		};

		Chrono chrono;
		chrono.start();

		size_t totalVertices = 0;
		size_t totalIndices	 = 0;

		const float PROBE_RADIUS = 1.4f;
		const float VOXEL_SIZE	 = 0.4f;

		for ( const Proxy::System * const proxy : _proxiesSystems )
		{
			// TODO: asserts.

			// TODO: Compute data only if not cached.
			Cache::SES & cache = _cacheSES[ proxy ];

			// Compute input data from category chain ids.
			_InputData data = _generateInputData(
				proxy->categoryPolymerChainIds,
				proxy->residueFirstAtomIndexes,
				proxy->residueAtomCounts,
				proxy->chainFirstResidues,
				proxy->chainResidueCounts,
				proxy->atomPositions,
				proxy->atomRadii
			);

			// Sort atoms in acceleration grid.
			const float atomGridCellSize = PROBE_RADIUS + data.maxRadius;
			const Vec3f gridMin			 = data.aabb.getMin() - atomGridCellSize;
			const Vec3f gridMax			 = data.aabb.getMax() + atomGridCellSize;

			const Vec3f gridSize	 = gridMax - gridMin;
			Vec3i		atomGridSize = Vec3i( Util::Math::ceil( gridSize / atomGridCellSize ) );

			Math::Grid gridAtoms( gridMin, Vec3f( atomGridCellSize ), atomGridSize );

			std::vector<std::vector<uint>> atomGridDataTmp( gridAtoms.getCellCount(), std::vector<uint>() );

			const std::vector<Vec3f> & atomPositions = *proxy->atomPositions;

			// Store atom indices in acceleration grid.
			std::vector<Vec4f> atomPositionsVdW( atomPositions.size() );
			for ( const uint idx : data.atomList )
			{
				/*
				if ( _category->getSystemPtr()->getAtom( idx ) == nullptr )
				{
					continue;
				}
				*/

				const uint hash = gridAtoms.gridHash( atomPositions[ idx ] );

				if ( hash >= atomGridDataTmp.size() )
				{
					continue;
				}

				atomGridDataTmp[ hash ].emplace_back( idx );
				atomPositionsVdW[ idx ] = Vec4f( atomPositions[ idx ], proxy->atomRadii[ idx ] );
			}

			// Linerize data in 1D arrays.
			std::vector<Range<uint>> atomGridDataSorted( gridAtoms.getCellCount(), Range<uint> { 0, 0 } );
			std::vector<uint>		 atomIndexSorted;

			for ( uint i = 0; i < atomGridDataTmp.size(); ++i )
			{
				const std::vector<uint> & data = atomGridDataTmp[ i ];
				if ( not data.empty() )
				{
					atomGridDataSorted[ i ] = Range<uint> { uint( atomIndexSorted.size() ), uint( data.size() ) };
					atomIndexSorted.insert( atomIndexSorted.end(), data.begin(), data.end() );
				}
			}

			// Free tmp memory.
			atomGridDataTmp.clear();
			atomGridDataTmp.shrink_to_fit();

			// Compute SES grid and compute SDF.
			Vec3i	   sesGridSize = Vec3i( Util::Math::ceil( gridSize / VOXEL_SIZE ) );
			Math::Grid gridSES( gridMin, Vec3f( VOXEL_SIZE ), sesGridSize );

			// Debug display.
			auto voxels = gridAtoms.toVoxels();
			_context->setData( voxels.first, "VoxelsMins" );
			_context->setData( voxels.second, "VoxelsMaxs" );

			drawRangeVoxels.offsets = { 0 };
			drawRangeVoxels.counts	= { uint( voxels.first.size() ) };

			/////////////////////
			// Worker: create SDF.
			// Worker::GpuComputer workerCreateSDF( IO::FilePath( "ses/create_sdf.comp" ) );
			struct SESGridData
			{
				float sdf;
				int	  nearestAtom;
			};

			ComputePass::Data bufferSesGridData { gridSES.getCellCount() * sizeof( SESGridData ), nullptr, 0 };
			ComputePass::Data bufferAtomGridDataSorted { atomGridDataSorted.size() * sizeof( Range<uint> ),
														 atomGridDataSorted.data(),
														 1 };
			ComputePass::Data bufferAtomIndexSorted { atomIndexSorted.size() * sizeof( uint ),
													  atomIndexSorted.data(),
													  2 };
			ComputePass::Data bufferAtomPosition { atomPositionsVdW.size() * sizeof( Vec4f ),
												   atomPositionsVdW.data(),
												   3 };

			const size_t sizeCreateSDF = bufferSesGridData.size + bufferAtomGridDataSorted.size
										 + bufferAtomIndexSorted.size + bufferAtomPosition.size;

			auto computePass = ComputePass {
				Program { "CreateSDF", std::vector<FilePath> { "ses/create_sdf.comp" }, Uniforms { /* TODO */ } },
				{ &bufferSesGridData, &bufferAtomGridDataSorted, &bufferAtomIndexSorted, &bufferAtomPosition },
				sizeCreateSDF
			};

			_context->compute( computePass );

			_context->clearComputeBuffers();
		}

		chrono.stop();
		VTX_DEBUG( "SES: {}s", chrono.elapsedTime() );
	}

	void Renderer::_refreshDataModels()
	{
		std::vector<_StructUBOModel> models;

		for ( const Proxy::System * const proxy : _proxiesSystems )
		{
			assert( proxy->transform );
			assert( _proxyCamera );

			const Mat4f matrixModelView = *_proxyCamera->matrixView * *proxy->transform;
			const Mat4f matrixNormal	= Util::Math::transpose( Util::Math::inverse( matrixModelView ) );
			models.emplace_back( _StructUBOModel { matrixModelView, matrixNormal } );
		}

		_context->setData( models, "Models" );
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
		setValue( matrixProjection, "Matrix projection" );
		_context->snapshot( p_outImage, _renderGraph->getRenderQueue(), _instructions, p_width, p_height );
		setValue( matrixProjectionOld, "Matrix projection" );
	}

} // namespace VTX::Renderer
