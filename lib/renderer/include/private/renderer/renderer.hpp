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

		inline size_t		 width() const { return _width; }
		inline size_t		 height() const { return _height; }
		inline RenderGraph & graph() { return _graph; }

		/**
		 * @brief Send data to GPU.
		 * @param p_value the data to send.
		 * @param p_key the buffer name to send on.
		 * @param p_index is the index of the data to set if we need to update only one value in an array.
		 */
		template<typename T>
		inline void setValue( const T & p_value, const std::string & p_key, const size_t p_index = 0 )
		{
			_context.setValue<T>( p_value, p_key, p_index );
			setNeedUpdate( true );
		}

		/**
		 * @brief Set the output to render on.
		 */
		inline void setOutput( const Handle p_output )
		{
			_context.setOutput( p_output );
			setNeedUpdate( true );
		}

		/**
		 * @brief Add data shared between all render passes.
		 */
		inline void addGlobalData( const BufferData & p_globalData ) { _globalData.emplace_back( p_globalData ); }

		/**
		 * @brief Set the current graphic API context.
		 */
		template<Context::ConceptContextImpl C, typename... Args>
		void set( Args &&... p_args )
		{
			const bool isFirstBuild = not _context.hasContext<C>();
			_context.set<C>( _width, _height, std::forward<Args>( p_args )... );

			build();

			if ( isFirstBuild )
			{
				onReady();
			}
		}

		/**
		 * @brief Build the renderer with the current _graph.
		 */
		void build();

		/**
		 * @brief Resize the renderer.
		 * @param p_output the output id to render on (eg. the output framebuffer for OpenGL impl.).
		 */
		void resize( const size_t p_width, const size_t p_height );

		/**
		 * @brief Clean all.
		 */
		void clean();

		/**
		 * @brief The main render loop.
		 */
		void render( const float p_deltaTime, const float p_elapsedTime );

		/**
		 * @brief Add data to the renderer.
		 */
		void addProxySystem( Proxy::System & p_proxy );
		void removeProxySystem( Proxy::System & p_proxy );
		void addProxySystems( std::vector<Proxy::System *> & p_proxies );
		void removeProxySystems( std::vector<Proxy::System *> & p_proxies );

		void setProxyCamera( Proxy::Camera & p_proxy );
		void setProxyColorLayout( Proxy::ColorLayout & p_proxy );
		void setProxyRepresentation( Proxy::Representation & p_proxies );
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
			return _context.getTextureData<Vec2i>( "Geometric", p_x, _height - p_y, E_CHAN_OUT::COLOR_2 );
		}

		/**
		 * @brief Ask for a render update.
		 */
		inline void setNeedUpdate( const bool p_value )
		{
			_needUpdate = p_value;
			if ( not p_value )
			{
				_framesRemaining = BUFFER_COUNT;
			}
		}

		/**
		 * @brief Get the current renderer infos.
		 */
		StructInfos getInfos() const;

		/**
		 * @brief Buffer swapping count.
		 */
		static const size_t BUFFER_COUNT = 2;

		/**
		 * @brief Primitives to show.
		 */
		bool showAtoms	 = true;
		bool showBonds	 = true;
		bool showRibbons = true;
		bool showVoxels	 = true;

		/**
		 * @brief Force update each frame.
		 */
		bool forceUpdate = true;

		/**
		 * @brief Current ranges to draw.
		 */
		// TODO: test render time with/without ranges/multidraw.
		using RangeList = Util::Math::RangeList<size_t>;
		RangeList	drawRangeSpheresRL;
		RangeList	drawRangeCylindersRL;
		RangeList	drawRangeRibbonsRL;
		Draw::Range drawRangeSpheres;
		Draw::Range drawRangeCylinders;
		Draw::Range drawRangeRibbons;
		Draw::Range drawRangeVoxels;

		/**
		 * @brief Callback triggered when the renderering context is ready and _graph built (first time only).
		 */
		Util::Callback<> onReady;

		/**
		 * @brief Useful for benchmarker only.
		 */
		inline void								  compileShaders() const { _context.compileShaders(); }
		inline const std::vector<Pass *> &		  getAvailablePasses() const { return availablePasses; }
		inline const InstructionsDurationRanges & getInstructionsDurationRanges() const
		{
			return _instructionsDurationRanges;
		}
		bool logDurations = false;

	  private:
		/**
		 * @brief Render _graph to handle the rendering pipeline.
		 */
		RenderGraph _graph;

		/**
		 * @brief Wrapper to handle the graphic APIs.
		 */
		Context::ContextWrapper _context;

		/**
		 * @brief Size.
		 */
		size_t _width;
		size_t _height;

		/**
		 * @brief Data shared between all render passes.
		 */
		std::vector<BufferData> _globalData;

		/**
		 * @brief Update next frame.
		 */
		bool   _needUpdate		= false;
		size_t _framesRemaining = BUFFER_COUNT;

		/**
		 * @brief Instructions to render, generated by context from the _graph.
		 */
		Instructions _instructions;

		/**
		 * @brief Instruction to render but with execution time logging.
		 */
		InstructionsDurationRanges _instructionsDurationRanges;

		/**
		 * @brief All data proxies.
		 */
		std::vector<Proxy::System *> _proxiesSystems;
		Proxy::Representation *		 _proxyRepresentation = nullptr;
		Proxy::Camera *				 _proxyCamera		  = nullptr;
		Proxy::ColorLayout *		 _proxyColorLayout	  = nullptr;
		Proxy::RenderSettings *		 _proxyRenderSettings = nullptr;
		Proxy::Voxels *				 _proxyVoxels		  = nullptr;

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

		/**
		 * @brief Cache some data to avoid recomputing.
		 */
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
