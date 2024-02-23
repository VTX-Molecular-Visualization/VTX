#ifndef __VTX_RENDERER_RENDERER__
#define __VTX_RENDERER_RENDERER__

#include "caches.hpp"
#include "context/opengl_45.hpp"
#include "passes.hpp"
#include "proxy/mesh.hpp"
#include "proxy/molecule.hpp"
#include "proxy/representation.hpp"
#include "proxy/voxel.hpp"
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
		using RenderGraphOpenGL45  = RenderGraph<Context::OpenGL45, Scheduler::DepthFirstSearch>;
		using CallbackClean		   = std::function<void()>;
		using CallbackReady		   = std::function<void()>;
		using CallbackSnapshotPre  = std::function<void( const size_t, const size_t )>;
		using CallbackSnapshotPost = std::function<void( const size_t, const size_t )>;

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
			geo->programs[ 0 ].draw.value().countFunction	 = [ this ]() { return showAtoms ? sizeAtoms : 0; };
			geo->programs[ 1 ].draw.value().countFunction	 = [ this ]() { return showBonds ? sizeBonds : 0; };
			geo->programs[ 2 ].draw.value().countFunction	 = [ this ]() { return showRibbons ? sizeRibbons : 0; };
			geo->programs[ 3 ].draw.value().countFunction	 = [ this ]() { return showVoxels ? sizeVoxels : 0; };
			blurX->name										 = "BlurX";
			blurY->name										 = "BlurY";
			blurY->programs[ 0 ].uniforms.entries[ 0 ].value = StructUniformValue<Vec2i> { Vec2i( 0, 1 ) };

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
				{ { { "Matrix view", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
					{ "Matrix projection", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
					{ "Camera position", E_TYPE::VEC3F, StructUniformValue<Vec3f> { VEC3F_ZERO } },
					{ "Camera clip infos", // { _near * _far, _far, _far - _near, _near }
					  E_TYPE::VEC4F,
					  StructUniformValue<Vec4f> { VEC4F_ZERO } },
					{ "Mouse position", E_TYPE::VEC2I, StructUniformValue<Vec2i> { Vec2i { 0, 0 } } },
					{ "Is perspective", E_TYPE::UINT, StructUniformValue<uint> { 1u } } } }
			);

			// TODO: move to pass ubo?
			_renderGraph->addUniforms(
				{ { { "Color layout", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> {} } }, 256 }
			);

			// TODO: dynamic ubo size instead of fixed 256.
			_renderGraph->addUniforms(
				{ { { "Matrix model view", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
					{ "Matrix normal", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } } },
				  256 }
			);
		}

		// Only first entry of the array saved on cpu.
		template<typename T>
		inline void setUniform( const std::vector<T> & p_value, const std::string & p_key )
		{
			assert( _context != nullptr );
			_context->setUniform<T>( p_value, p_key );
			setNeedUpdate( true );
		}

		template<typename T>
		inline void setUniform( const T & p_value, const std::string & p_key )
		{
			assert( _context != nullptr );
			_context->setUniform<T>( p_value, p_key );
			setNeedUpdate( true );
		}

		template<typename T>
		inline void getUniform( T & p_value, const std::string & p_key )
		{
			_context->getUniform<T>( p_value, p_key );
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

		void build( const uint p_output = 0, void * p_loader = nullptr );
		void clean();

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

		inline void addCallbackReady( const Util::Callback<>::Func & p_cb ) { _callbackReady += p_cb; }
		inline void addCallbackClean( const Util::Callback<>::Func & p_cb ) { _callbackClean += p_cb; }

		inline void setMatrixView( const Mat4f & p_view )
		{
			setUniform( p_view, "Matrix view" );

			// Update model view matrices.
			_refreshDataModels();
		}

		inline void setMatrixProjection( const Mat4f & p_proj ) { setUniform( p_proj, "Matrix projection" ); }

		inline void setCameraPosition( const Vec3f & p_position ) { setUniform( p_position, "Camera position" ); }

		inline void setCameraClipInfos( const float p_near, const float p_far )
		{
			setUniform( Vec4f( p_near * p_far, p_far, p_far - p_near, p_near ), "Camera clip infos" );
		}

		inline void setMousePosition( const Vec2i & p_position )
		{
			// setUniform( Vec2i { p_position.x, _height - p_position.y }, "Mouse position" );
		}

		inline void setPerspective( const bool p_perspective )
		{
			setUniform( uint( p_perspective ), "Is perspective" );
		}

		void addProxy( Proxy::Molecule & p_proxy );

		inline void addProxy( const Proxy::Voxel & p_proxy )
		{
			/*
			_proxiesVoxels.push_back( p_proxy );

			if ( _renderGraph->isBuilt() )
			{
				_setData( p_proxy );
			}

			setNeedUpdate( true );
			*/
		}

		inline void setColorLayout( const std::array<Util::Color::Rgba, 256> & p_layout )
		{
			setUniform( std::vector<Util::Color::Rgba>( p_layout.begin(), p_layout.end() ), "Color layout" );
		}

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
				_framesRemaining = _BUFFER_COUNT;
			}
		}

		inline void compileShaders() const { _context->compileShaders(); }

		inline RenderGraphOpenGL45 &			  getRenderGraph() { return *_renderGraph; }
		inline const std::vector<Pass *> &		  getAvailablePasses() const { return availablePasses; }
		inline const InstructionsDurationRanges & getInstructionsDurationRanges() const
		{
			return _instructionsDurationRanges;
		}
		inline std::vector<Proxy::Molecule *> &		  getProxiesMolecules() { return _proxiesMolecules; }
		inline std::vector<Proxy::Mesh *> &			  getProxiesMeshes() { return _proxiesMeshes; }
		inline std::vector<Proxy::Representation *> & getProxiesRepresentations() { return _proxiesRepresentations; }
		inline std::vector<Proxy::Voxel *> &		  getProxiesVoxels() { return _proxiesVoxels; }

		size_t		width;
		size_t		height;
		StructInfos infos;

		size_t sizeAtoms   = 0;
		size_t sizeBonds   = 0;
		size_t sizeRibbons = 0;
		size_t sizeVoxels  = 0;

		bool showAtoms	 = true;
		bool showBonds	 = true;
		bool showRibbons = true;
		bool showVoxels	 = true;

		bool forceUpdate  = false;
		bool logDurations = false;

	  private:
		const size_t _BUFFER_COUNT = 2;

		Context::OpenGL45 *					 _context		  = nullptr;
		void *								 _loader		  = nullptr;
		bool								 _needUpdate	  = false;
		size_t								 _framesRemaining = _BUFFER_COUNT;
		FilePath							 _shaderPath;
		std::unique_ptr<RenderGraphOpenGL45> _renderGraph;
		Instructions						 _instructions;
		InstructionsDurationRanges			 _instructionsDurationRanges;

		// Callbacks.
		Util::Callback<> _callbackReady;
		Util::Callback<> _callbackClean;

		// Proxies.
		std::vector<Proxy::Molecule *>		 _proxiesMolecules;
		std::vector<Proxy::Mesh *>			 _proxiesMeshes;
		std::vector<Proxy::Representation *> _proxiesRepresentations;
		std::vector<Proxy::Voxel *>			 _proxiesVoxels;

		// TODO: check complexity.
		inline size_t _getProxyId( const Proxy::Molecule * const p_proxy ) const
		{
			return std::distance(
				_proxiesMolecules.begin(), std::find( _proxiesMolecules.begin(), _proxiesMolecules.end(), p_proxy )
			);
		}

		// Cache.
		std::map<const Proxy::Molecule * const, Cache::SphereCylinder> _cacheSpheresCylinders;
		std::map<const Proxy::Molecule * const, Cache::Ribbon>		   _cacheRibbons;

		// TODO: make "filler" functions for each type of data instead of _setDataX?
		inline void _refreshDataMolecules()
		{
			_refreshDataSpheresCylinders();
			_refreshDataRibbons();
			_refreshDataModels();

			setNeedUpdate( true );
		}

		void _refreshDataSpheresCylinders();
		void _refreshDataRibbons();
		void _refreshDataModels();

		enum E_ELEMENT_FLAGS
		{
			VISIBILITY = 0,
			SELECTION  = 1
		};

		struct _StructUBOModel
		{
			Mat4f mv;
			Mat4f n;
		};

		void _setData( const Proxy::Voxel & p_proxy )
		{
			assert( p_proxy.mins );
			assert( p_proxy.maxs );
			assert( p_proxy.mins->size() == p_proxy.maxs->size() );

			_context->setData( *p_proxy.mins, "VoxelsMins" );
			_context->setData( *p_proxy.maxs, "VoxelsMaxs" );

			sizeVoxels += uint( p_proxy.mins->size() );
		}

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
