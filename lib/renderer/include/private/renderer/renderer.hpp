#ifndef __VTX_RENDERER_RENDERER__
#define __VTX_RENDERER_RENDERER__

#include "caches.hpp"
#include "context/context_wrapper.hpp"
#include "passes.hpp"
#include "render_graph.hpp"
#include "renderer/context/default.hpp"
#include "renderer/context/opengl_45.hpp"
#include "renderer/proxy/camera.hpp"
#include "renderer/proxy/color_layout.hpp"
#include "renderer/proxy/mesh.hpp"
#include "renderer/proxy/render_settings.hpp"
#include "renderer/proxy/representation.hpp"
#include "renderer/proxy/system.hpp"
#include "renderer/proxy/voxels.hpp"
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
		Renderer( const size_t p_width, const size_t p_height );

		/**
		 * @brief Send data to GPU.
		 * @param p_key the buffer name to send on.
		 * @param p_index is the index of the data to set if we need to update only one value in an array.
		 */
		template<typename T>
		inline void setValue( const T & p_value, const std::string & p_key, const size_t p_index = 0 )
		{
			_context.setValue<T>( p_value, p_key, p_index );
			setNeedUpdate( true );
		}

		inline Pass * const addPass( const Pass & p_pass ) { return _renderGraph->addPass( p_pass ); }

		inline void removePass( const Pass * const p_pass ) { _renderGraph->removePass( p_pass ); }

		inline bool addLink(
			Pass * const	   p_passSrc,
			Pass * const	   p_passDest,
			const E_CHAN_OUT & p_channelSrc	 = E_CHAN_OUT::COLOR_0,
			const E_CHAN_IN &  p_channelDest = E_CHAN_IN::_0
		)
		{
			return _renderGraph->addLink( p_passSrc, p_passDest, p_channelSrc, p_channelDest );
		}

		inline void removeLink( const Link * const p_link ) { _renderGraph->removeLink( p_link ); }

		inline void addGlobalData( const BufferData & p_globalData ) { _globalData.emplace_back( p_globalData ); }

		inline const Passes &		getPasses() const { return _renderGraph->getPasses(); }
		inline const Links &		getLinks() const { return _renderGraph->getLinks(); }
		inline const RenderQueue &	getRenderQueue() const { return _renderGraph->getRenderQueue(); }
		inline const Output * const getOutput() const { return _renderGraph->getOutput(); }
		inline void					setOutput( const Output * const p_output ) { _renderGraph->setOutput( p_output ); }

		inline void setOutput( const Handle p_output )
		{
			_context.setOutput( p_output );
			setNeedUpdate( true );
		}

		template<E_GRAPHIC_API API, typename... Args>
		void set( Args &&... p_args )
		{
			bool isFirstBuild;

			// Static map enum to context type.
			if constexpr ( API == E_GRAPHIC_API::DEFAULT )
			{
				isFirstBuild = not _context.hasContext<Context::Default>();
				_context.set<Context::Default>( width, height, std::forward<Args>( p_args )... );
			}
			else if constexpr ( API == E_GRAPHIC_API::OPENGL45 )
			{
				isFirstBuild = not _context.hasContext<Context::OpenGL45>();
				_context.set<Context::OpenGL45>( width, height, std::forward<Args>( p_args )... );
			}
			else
			{
				static_assert( std::is_same_v<API, void>, "Unknown graphic API." );
			}

			build();

			if ( isFirstBuild )
			{
				onReady();
			}
		}

		void build();

		/**
		 * @brief Resize the renderer.
		 * @param p_output the output id to render on (eg. the output framebuffer for OpenGL impl.).
		 */
		void resize( const size_t p_width, const size_t p_height );

		void clean();

		/**
		 * @brief The render loop.
		 * @param p_time is the current running time.
		 */
		void render( const float p_deltaTime, const float p_elapsedTime );

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
			return _context.getTextureData<Vec2i>( "Geometric", p_x, height - p_y, E_CHAN_OUT::COLOR_2 );
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
		inline void								  compileShaders() const { _context.compileShaders(); }
		inline const std::vector<Pass *> &		  getAvailablePasses() const { return availablePasses; }
		inline const InstructionsDurationRanges & getInstructionsDurationRanges() const
		{
			return _instructionsDurationRanges;
		}

		inline StructInfos getInfos() const
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

		size_t width;
		size_t height;

		bool showAtoms	 = true;
		bool showBonds	 = true;
		bool showRibbons = true;
		bool showVoxels	 = true;

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
		Context::ContextWrapper		 _context;
		std::vector<BufferData>		 _globalData;
		bool						 _needUpdate	  = false;
		size_t						 _framesRemaining = BUFFER_COUNT;
		std::unique_ptr<RenderGraph> _renderGraph;
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

		void _applyRepresentationLogic( Proxy::Representation * const );

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
		void _renderLog( const float p_deltaTime, const float p_elapsedTime );
	};
} // namespace VTX::Renderer
#endif
