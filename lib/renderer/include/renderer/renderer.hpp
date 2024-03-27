#ifndef __VTX_RENDERER_RENDERER__
#define __VTX_RENDERER_RENDERER__

#include "caches.hpp"
#include "context/opengl_45.hpp"
#include "passes.hpp"
#include "proxy/camera.hpp"
#include "proxy/color_layout.hpp"
#include "proxy/mesh.hpp"
#include "proxy/molecule.hpp"
#include "proxy/render_settings.hpp"
#include "proxy/representations.hpp"
#include "proxy/voxels.hpp"
#include "render_graph.hpp"
#include "scheduler/depth_first_search.hpp"
#include <util/callback.hpp>
#include <util/chrono.hpp>
#include <util/logger.hpp>

namespace VTX::Renderer
{
	class Renderer
	{
	  public:
		using RenderGraphOpenGL45 = RenderGraph<Context::OpenGL45, Scheduler::DepthFirstSearch>;

		Renderer(
			const size_t	 p_width,
			const size_t	 p_height,
			const FilePath & p_shaderPath,
			void *			 p_loader = nullptr
		) :
			width( p_width ),
			height( p_height ), _shaderPath( p_shaderPath ), _loader( p_loader )
		{
			// Graph.
			_renderGraph = std::make_unique<RenderGraphOpenGL45>();

			// Passes.
			Pass * const geo	   = _renderGraph->addPass( descPassGeometric );
			Pass * const depth	   = _renderGraph->addPass( descPassDepth );
			Pass * const ssao	   = _renderGraph->addPass( descPassSSAO );
			Pass * const blurX	   = _renderGraph->addPass( descPassBlur );
			Pass * const blurY	   = _renderGraph->addPass( descPassBlur );
			Pass * const shading   = _renderGraph->addPass( descPassShading );
			Pass * const outline   = _renderGraph->addPass( descPassOutline );
			Pass * const selection = _renderGraph->addPass( descPassSelection );
			Pass * const fxaa	   = _renderGraph->addPass( desPassFXAA );

			// Setup values.
			geo->programs[ 0 ].draw.value().countFunction = [ this ]() { return showAtoms ? sizeAtoms : 0; };
			geo->programs[ 1 ].draw.value().countFunction = [ this ]() { return showBonds ? sizeBonds : 0; };
			geo->programs[ 2 ].draw.value().countFunction = [ this ]() { return showRibbons ? sizeRibbons : 0; };
			geo->programs[ 3 ].draw.value().countFunction = [ this ]() { return showVoxels ? sizeVoxels : 0; };
			blurX->name									  = "BlurX";
			blurY->name									  = "BlurY";
			blurY->programs[ 0 ].uniforms[ 0 ].value	  = StructUniformValue<Vec2i> { Vec2i( 0, 1 ) };

			// Links.
			_renderGraph->addLink( geo, depth, E_CHANNEL_OUTPUT::DEPTH, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( geo, ssao, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( depth, ssao, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_2 );
			_renderGraph->addLink( ssao, blurX, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( depth, blurX, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_1 );
			_renderGraph->addLink( blurX, blurY, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( depth, blurY, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_1 );
			_renderGraph->addLink( geo, shading, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( geo, shading, E_CHANNEL_OUTPUT::COLOR_1, E_CHANNEL_INPUT::_1 );
			_renderGraph->addLink( blurY, shading, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_2 );
			_renderGraph->addLink( shading, outline, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( depth, outline, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_1 );
			_renderGraph->addLink( geo, selection, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( outline, selection, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_1 );
			_renderGraph->addLink( depth, selection, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_2 );
			_renderGraph->addLink( selection, fxaa, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->setOutput( &fxaa->outputs[ E_CHANNEL_OUTPUT::COLOR_0 ] );

			// Shared uniforms.
			_renderGraph->addUniforms(
				{ "Camera",
				  { { "Matrix view", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
					{ "Matrix projection", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
					{ "Camera position", E_TYPE::VEC3F, StructUniformValue<Vec3f> { VEC3F_ZERO } },
					{ "Camera clip infos", // { _near * _far, _far, _far - _near, _near }
					  E_TYPE::VEC4F,
					  StructUniformValue<Vec4f> { VEC4F_ZERO } },
					{ "Mouse position", E_TYPE::VEC2I, StructUniformValue<Vec2i> { Vec2i { 0, 0 } } },
					{ "Is perspective", E_TYPE::UINT, StructUniformValue<uint> { 1 } } },
				  15 }
			);

			_renderGraph->addUniforms(
				{ "Color layout", { { "Colors", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> {} } }, 14, true }
			);

			_renderGraph->addUniforms(
				{ "Models",
				  { { "Matrix model view", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
					{ "Matrix normal", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } } },
				  13,
				  true }
			);

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

		template<typename T>
		inline void setUniform( const T & p_value, const std::string & p_key, const size_t p_size = 0 )
		{
			assert( _context != nullptr );
			_context->setData<T>( p_value, p_key, p_size );
			setNeedUpdate( true );
		}

		inline void resize( const size_t p_width, const size_t p_height, const uint p_output = 0 )
		{
			// assert( _context != nullptr );
			//  TODO: delete context check.
			if ( _context == nullptr )
				return;

			width  = p_width;
			height = p_height;

			_context->resize( _renderGraph->getRenderQueue(), p_width, p_height );

			setNeedUpdate( true );
		}

		inline void setOutput( const uint p_output )
		{
			assert( _context != nullptr );

			_context->setOutput( p_output );
			setNeedUpdate( true );
		}

		void		build( const uint p_output = 0, void * p_loader = nullptr );
		void		clean();
		inline bool hasContext() const { return _context != nullptr; }

		inline void render( const float p_time )
		{
			if ( _needUpdate || forceUpdate || _framesRemaining > 0 )
			{
				if ( logDurations )
				{
					_renderLog( p_time );
				}
				else
				{
					_render( p_time );
				}

				if ( forceUpdate == false )
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

		void addProxyMolecule( Proxy::Molecule & p_proxy );
		void removeProxyMolecule( Proxy::Molecule & p_proxy );
		void addProxyMolecules( std::vector<Proxy::Molecule *> & p_proxies );
		void removeProxyMolecules( std::vector<Proxy::Molecule *> & p_proxies );

		void addProxyRepresentation( Proxy::Representation & p_proxy );
		void removeProxyRepresentation( Proxy::Representation & p_proxy );
		void addProxyRepresentations( std::vector<Proxy::Representation *> & p_proxies );
		void removeProxyRepresentations( std::vector<Proxy::Representation *> & p_proxies );

		void setProxyCamera( Proxy::Camera & p_proxy );
		void setProxyColorLayout( Proxy::ColorLayout & p_proxy );
		void setProxyRenderSettings( Proxy::RenderSettings & p_proxy );
		void setProxyVoxels( Proxy::Voxels & p_proxy );

		void snapshot(
			std::vector<uchar> & p_image,
			const size_t		 p_width,
			const size_t		 p_height,
			const float			 p_fov,
			const float			 p_near,
			const float			 p_far
		);

		inline Vec2i getPickedIds( const size_t p_x, const size_t p_y ) const
		{
			std::any idsAny = std::make_any<Vec2i>();
			_context->getTextureData( idsAny, p_x, height - p_y, "Geometric", E_CHANNEL_OUTPUT::COLOR_2 );
			return std::any_cast<Vec2i>( idsAny );
		}

		inline void setNeedUpdate( const bool p_value )
		{
			_needUpdate = p_value;
			if ( p_value == false )
			{
				_framesRemaining = BUFFER_COUNT;
			}
		}

		inline void compileShaders() const { _context->compileShaders(); }

		inline RenderGraphOpenGL45 &			  getRenderGraph() { return *_renderGraph; }
		inline const std::vector<Pass *> &		  getAvailablePasses() const { return availablePasses; }
		inline const InstructionsDurationRanges & getInstructionsDurationRanges() const
		{
			return _instructionsDurationRanges;
		}

		inline StructInfos getInfos()
		{
			StructInfos infos;
			_context->fillInfos( infos );

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

		size_t width;
		size_t height;

		size_t sizeAtoms   = 0;
		size_t sizeBonds   = 0;
		size_t sizeRibbons = 0;
		size_t sizeVoxels  = 0;

		bool showAtoms	 = true;
		bool showBonds	 = true;
		bool showRibbons = true;
		bool showVoxels	 = false;

		bool forceUpdate  = true;
		bool logDurations = false;

		static const size_t BUFFER_COUNT = 2;

	  private:
		Context::OpenGL45 *					 _context		  = nullptr;
		void *								 _loader		  = nullptr;
		bool								 _needUpdate	  = false;
		size_t								 _framesRemaining = BUFFER_COUNT;
		FilePath							 _shaderPath;
		std::unique_ptr<RenderGraphOpenGL45> _renderGraph;
		Instructions						 _instructions;
		InstructionsDurationRanges			 _instructionsDurationRanges;

		// Proxies.

		std::vector<Proxy::Molecule *>		 _proxiesMolecules;
		std::vector<Proxy::Representation *> _proxyRepresentations;
		Proxy::Camera *						 _proxyCamera;
		Proxy::ColorLayout *				 _proxyColorLayout;
		Proxy::RenderSettings *				 _proxyRenderSettings;
		Proxy::Voxels *						 _proxyVoxels;

		void _addProxyMolecule( Proxy::Molecule & p_proxy );
		void _removeProxyMolecule( Proxy::Molecule & p_proxy );

		// TODO: check complexity.
		inline size_t _getProxyId( const Proxy::Molecule * const p_proxy ) const
		{
			size_t id = std::distance(
				_proxiesMolecules.begin(), std::find( _proxiesMolecules.begin(), _proxiesMolecules.end(), p_proxy )
			);

			assert( id < _proxiesMolecules.size() );

			return id;
		}

		// Cache.
		std::map<const Proxy::Molecule * const, Cache::SphereCylinder> _cacheSpheresCylinders;
		std::map<const Proxy::Molecule * const, Cache::Ribbon>		   _cacheRibbons;

		// TODO: make "filler" functions for each type of data instead of _setDataX?
		inline void _refreshDataMolecules()
		{
			_refreshDataSpheresCylinders();
			_refreshDataRibbons();
			_refreshDataSES();
			_refreshDataModels();

			setNeedUpdate( true );
		}

		void _refreshDataSpheresCylinders();
		void _refreshDataRibbons();
		void _refreshDataSES();
		void _refreshDataModels();
		void _refreshDataVoxels();

		enum E_ELEMENT_FLAGS
		{
			VISIBILITY = 0,
			SELECTION  = 1
		};

		// TODO: find a way to delete that?
		struct _StructUBOCamera
		{
			// layout 140.
			Mat4f matrixView;
			Mat4f matrixProjection;
			Vec3f cameraPosition;
			uint  padding;
			Vec4f cameraClipInfos;
			Vec2i mousePosition;
			uint  isPerspective;
		};

		struct _StructUBOModel
		{
			// layout 140.
			Mat4f mv;
			Mat4f n;
		};

		struct _StructUBORepresentation
		{
			// layout 430 (forced or renderdoc bug?).
			float radiusSphereFixed;
			float radiusSphereAdd;
			uint  radiusFixed;
			float radiusCylinder;

			uint cylinderColorBlendingMode;
			uint ribbonColorBlendingMode;
			// uint padding[ 2 ];
		};

		inline void _render( const float p_time ) const
		{
			for ( const Instruction & instruction : _instructions )
			{
				instruction();
			}
		}

		inline void _renderLog( const float p_time )
		{
			for ( InstructionsDurationRange & instructionDurationRange : _instructionsDurationRanges )
			{
				instructionDurationRange.duration = _context->measureTaskDuration(

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
	};
} // namespace VTX::Renderer
#endif
