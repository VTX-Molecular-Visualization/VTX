#ifndef __VTX_RENDERER_RENDERER__
#define __VTX_RENDERER_RENDERER__

#include "caches.hpp"
#include "context/opengl_45.hpp"
#include "passes.hpp"
#include "proxy/camera.hpp"
#include "proxy/color_layout.hpp"
#include "proxy/mesh.hpp"
#include "proxy/render_settings.hpp"
#include "proxy/representation.hpp"
#include "proxy/system.hpp"
#include "proxy/voxels.hpp"
#include "render_graph.hpp"
#include "scheduler/depth_first_search.hpp"
#include <util/callback.hpp>
#include <util/chrono.hpp>
#include <util/logger.hpp>
#include <util/math/range_list.hpp>

namespace VTX::Renderer
{
	/**
	 * @brief The renderer, only accessed from the facade.
	 */
	class Renderer
	{
	  public:
		/**
		 * @brief The render graph with OpenGL45 API and the DFS scheduler.
		 */
		using RenderGraphOpenGL45 = RenderGraph<Context::OpenGL45, Scheduler::DepthFirstSearch>;

		Renderer(
			const size_t	 p_width,
			const size_t	 p_height,
			const FilePath & p_shaderPath,
			void *			 p_loader = nullptr
		);

		/**
		 * @brief Send data to GPU.
		 * @param p_key the buffer name to send on.
		 * @param p_index is the index of the data to set if we need to update only one value in an array.
		 */
		template<typename T>
		inline void setValue( const T & p_value, const std::string & p_key, const size_t p_index = 0 )
		{
			assert( _context != nullptr );
			_context->setValue<T>( p_value, p_key, p_index );
			setNeedUpdate( true );
		}

		/**
		 * @brief Resize the renderer.
		 * @param p_output the output id to render on (eg. the output framebuffer for OpenGL impl.).
		 */
		inline void resize( const size_t p_width, const size_t p_height, const Handle p_output = 0 )
		{
			VTX_TRACE( "Resizing renderer to {}x{}", width, height );

			if ( _context == nullptr )
				return;

			width  = p_width;
			height = p_height;

			Vec2i size = { p_width, p_height };
			setValue( size, "Resolution" );

			_context->resize( _renderGraph->getRenderQueue(), p_width, p_height );

			setNeedUpdate( true );
		}

		inline void setOutput( const Handle p_output )
		{
			assert( _context != nullptr );

			_context->setOutput( p_output );
			setNeedUpdate( true );
		}

		void		build( const uint p_output = 0, void * p_loader = nullptr );
		void		clean();
		inline bool hasContext() const { return _context != nullptr; }

		/**
		 * @brief The render loop.
		 * @param p_time is the current running time.
		 */
		inline void render( const float p_deltaTime, const float p_elapsedTime )
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

		void addProxySystem( Proxy::System & p_proxy );
		void removeProxySystem( Proxy::System & p_proxy );
		void addProxySystems( std::vector<Proxy::System *> & p_proxies );
		void removeProxySystems( std::vector<Proxy::System *> & p_proxies );

		void addProxyRepresentation( Proxy::Representation & p_proxy );
		void removeProxyRepresentation( Proxy::Representation & p_proxy );
		void addProxyRepresentations( std::vector<Proxy::Representation *> & p_proxies );
		void removeProxyRepresentations( std::vector<Proxy::Representation *> & p_proxies );

		void setProxyCamera( Proxy::Camera & p_proxy );
		void setProxyColorLayout( Proxy::ColorLayout & p_proxy );
		void setProxyRenderSettings( Proxy::RenderSettings & p_proxy );
		void setProxyVoxels( Proxy::Voxels & p_proxy );

		/**
		 * @brief Exports the renderer to an array of pixels.
		 * @param p_outImage the output image data.
		 */
		void snapshot(
			std::vector<uchar> & p_outImage,
			const size_t		 p_width,
			const size_t		 p_height,
			const float			 p_fov,
			const float			 p_near,
			const float			 p_far
		);

		/**
		 * @brief Get the id of the the data stored in the given pixel.
		 * @return ( AtomId, 0 ) for an atom, ( AtomId1, AtomId2 ) for a bond.
		 */
		inline Vec2i getPickedIds( const size_t p_x, const size_t p_y ) const
		{
			Vec2i ids;
			_context->getTextureData( ids, p_x, height - p_y, "Geometric", E_CHAN_OUT::COLOR_2 );
			return ids;
		}

		inline void setNeedUpdate( const bool p_value )
		{
			_needUpdate = p_value;
			if ( not p_value )
			{
				_framesRemaining = BUFFER_COUNT;
			}
		}

		// Benchmarker only.
		inline void								  compileShaders() const { _context->compileShaders(); }
		inline RenderGraphOpenGL45 &			  getRenderGraph() { return *_renderGraph; }
		inline const std::vector<Pass *> &		  getAvailablePasses() const { return availablePasses; }
		inline const InstructionsDurationRanges & getInstructionsDurationRanges() const
		{
			return _instructionsDurationRanges;
		}

		inline StructInfos getInfos() const
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

		bool showAtoms	 = true;
		bool showBonds	 = true;
		bool showRibbons = true;
		bool showVoxels	 = false;

		bool forceUpdate  = true;
		bool logDurations = false;

		static const size_t BUFFER_COUNT = 2;

		// Draw ranges.
		// TODO: test render time with/without ranges/multidraw.
		using RangeList = Util::Math::RangeList<size_t>;
		RangeList	drawRangeSpheresRL;
		RangeList	drawRangeCylindersRL;
		RangeList	drawRangeRibbonsRL;
		Draw::Range drawRangeSpheres;
		Draw::Range drawRangeCylinders;
		Draw::Range drawRangeRibbons;
		Draw::Range drawRangeVoxels;

		// TODO: currentSize proxies and ranges?

		// Callbacks.
		Util::Callback<> onReady;

	  private:
		Context::OpenGL45 *					 _context		  = nullptr;
		void *								 _loader		  = nullptr;
		bool								 _needUpdate	  = false;
		size_t								 _framesRemaining = BUFFER_COUNT;
		FilePath							 _shaderPath;
		std::unique_ptr<RenderGraphOpenGL45> _renderGraph;
		// All instructions computed by the graph and his context.
		Instructions _instructions;
		// Used to log render times.
		InstructionsDurationRanges _instructionsDurationRanges;

		// Proxies.
		std::vector<Proxy::System *>		 _proxiesSystems;
		std::vector<Proxy::Representation *> _proxyRepresentations;
		Proxy::Camera *						 _proxyCamera		  = nullptr;
		Proxy::ColorLayout *				 _proxyColorLayout	  = nullptr;
		Proxy::RenderSettings *				 _proxyRenderSettings = nullptr;
		Proxy::Voxels *						 _proxyVoxels		  = nullptr;

		void _addProxySystem( Proxy::System & p_proxy );
		void _removeProxySystem( Proxy::System & p_proxy );

		// TODO: check complexity.
		inline size_t _getProxyId( const Proxy::System * const p_proxy ) const
		{
			size_t id = std::distance(
				_proxiesSystems.begin(), std::find( _proxiesSystems.begin(), _proxiesSystems.end(), p_proxy )
			);

			assert( id < _proxiesSystems.size() );

			return id;
		}

		// Cache.
		std::map<const Proxy::System * const, Cache::SphereCylinder> _cacheSpheresCylinders;
		std::map<const Proxy::System * const, Cache::Ribbon>		 _cacheRibbons;
		std::map<const Proxy::System * const, Cache::SES>			 _cacheSES;

		void _refreshGraph();

		// TODO: make "filler" functions for each type of data instead of _setDataX?
		inline void _refreshDataSystems()
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
			Vec2i resolution;
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

		/**
		 * @brief The main render loop that call each generated instruction.
		 * @param p_time the current time.
		 */
		inline void _render( const float p_deltaTime, const float p_elapsedTime ) const
		{
			for ( const Instruction & instruction : _instructions )
			{
				instruction();
			}
		}

		/**
		 * @brief The main render loop that call instructions with time logging.
		 * @param p_time the current time.
		 */
		inline void _renderLog( const float p_deltaTime, const float p_elapsedTime )
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
