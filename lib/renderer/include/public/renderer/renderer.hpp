#ifndef __VTX_RENDERER_RENDERER__
#define __VTX_RENDERER_RENDERER__

#include "renderer/caches.hpp"
#include "renderer/camera.hpp"
#include "renderer/color.hpp"
#include "renderer/context/context_wrapper.hpp"
#include "renderer/geometry/geometries.hpp"
#include "renderer/graphics_config.hpp"
#include "renderer/layout/layouts.hpp"
#include "renderer/render_graph.hpp"
#include "renderer/representation.hpp"
#include "renderer/struct_infos.hpp"
#include "renderer/system_data.hpp"
#include "renderer/types.hpp"
#include <unordered_set>
#include <util/callback.hpp>
#include <util/math/bitset.hpp>

namespace VTX::Renderer
{

	/**
	 * @brief The renderer, only accessed from the facade.
	 */
	class Renderer
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		Renderer( const size_t p_width = 0, const size_t p_height = 0 );
		Renderer( const Renderer & )			 = delete;
		Renderer & operator=( const Renderer & ) = delete;

		/**
		 * @brief Accessors.
		 */
		inline size_t		 width() const { return _width; }
		inline size_t		 height() const { return _height; }
		inline RenderGraph & graph() { return _graph; }

		// TODO: redo or remove.
		template<typename T>
		inline void setValue( const T & p_value, const Desc::Key & p_key, const size_t p_index = 0 )
		{
			//_context.setValue<T>( p_value, p_key, p_index );
			setNeedUpdate( true );
		}

		/**
		 * @brief Set graphic context.
		 */
		void setDefault();
		void setOpenGL( const uintptr_t p_nativeSurface, const uintptr_t p_nativeDisplay, const FilePath & p_shaderDir );

		/**
		 * @brief Resize the renderer.
		 */
		void resize( const size_t, const size_t );

		/**
		 * @brief Clean all.
		 */
		void clear();

		/**
		 * @brief The main render loop.
		 * @return true if rendered, false if nothing updated.
		 */
		bool render( const float = 0, const float = 0 ) noexcept;

		/**
		 * @brief Push data to the renderer.
		 */
		void setCamera( const Camera &, const Vec3f &, const Mat4f &, const Mat4f & );
		void setGraphicsConfig( const GraphicsConfig & );
		void setColorLayout( const Color::Layout & );
		void setRepresentations( const std::vector<const Representation *> & );
		void setVoxels( const std::vector<Vec3f> &, const std::vector<Vec3f> & );

		/**
		 * @brief Push systems.
		 */
		void setSystems( const std::vector<SystemData> & );

		/**
		 * @brief Push system data.
		 */
		void setSystemTransform( const SystemUID, const Mat4f & );
		void setSystemPosition( const SystemUID, std::span<const Vec3f> );
		void setSystemColors( const SystemUID, std::span<const ColorIndex> );
		void setSystemRepresentation( const SystemUID, const MapRepresentationRanges & );
		void setSystemVisibility( const SystemUID, const Util::Math::BitSet & );
		void setSystemSelection( const SystemUID, const Util::Math::BitSet & );

		/**
		 * @brief Exports the renderer to an array of pixels.
		 */
		std::vector<std::byte> snapshot();

		/**
		 * @brief Get the id of the the data stored in the given pixel.
		 * @return ( AtomId, 0 ) for an atom, ( AtomId1, AtomId2 ) for a bond.
		 */
		Vec2i getPickedIds( const size_t, const size_t ) const;

		/**
		 * @brief Ask for a render update.
		 */
		inline void setNeedUpdate( const bool p_value ) { _needUpdate = p_value; }

		/**
		 * @brief Force update each frame.
		 */
		inline void setForceUpdate( const bool p_value ) { _forceUpdate = p_value; }

		/**
		 * @brief Force update each frame.
		 */
		inline void setVSync( const bool p_value ) { _context.setOption( Desc::E_OPTION::VSYNC, p_value ); }

		/**
		 * @brief Get the current renderer infos.
		 */
		const StructInfos & getInfos( const bool = false );

		/**
		 * @brief Callback when ready.
		 * TODO: use event hub?
		 */
		Util::Callback<> onReady;

	  private:
		/**
		 * @brief Render _graph to handle the rendering pipeline.
		 */
		RenderGraph _graph;

		/**
		 * @brief Render queue built from the _graph.
		 */
		Desc::RenderQueue _queue;

		/**
		 * @brief Vertex layouts.
		 */
		Layouts _layouts;

		/**
		 * @brief Geometries.
		 */
		Geometries _geometries;

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
		 * @brief Force update each frame.
		 */
		bool _forceUpdate = true;

		/**
		 * @brief Update next frame.
		 */
		bool _needUpdate = false;

		/**
		 * @brief Rebuild draw ranges next frame.
		 */
		std::unordered_set<Desc::Handle> _systemToRefresh;

		/**
		 * @brief Renderer infos.
		 */
		StructInfos _infos;

		/**
		 * @brief Cached data to update.
		 */
		Cache::Camera												   _cacheCamera;
		ResourceHandler<Cache::System, DescDummy, SystemUID>		   _systems;
		std::unordered_map<RepresentationIndex, Cache::Representation> _cacheRepresentations;

		/**
		 * @brief Refresh the render graph according to the graphics config.
		 * @return true if the graph has changed.
		 */
		bool _refreshGraph( const GraphicsConfig & );

		/**
		 * @brief Refresh transform when camera or system changed.
		 */
		void _refreshDataModels();

		/**
		 * @brief Refresh visibility from visibility and representation ranges.
		 */
		void _refreshSystemVisibility( const Desc::Handle );

		/**
		 * @brief Flags to push boolean values into one byte.
		 */
		enum struct E_ELEMENT_FLAGS : uint8_t
		{
			VISIBILITY = 0,
			SELECTION  = 1
		};

		/**
		 * @brief The main render loop that call each generated instruction.
		 */
		inline void _render( const float p_deltaTime, const float p_elapsedTime ) const noexcept { _context.execute(); }
	};
} // namespace VTX::Renderer
#endif
