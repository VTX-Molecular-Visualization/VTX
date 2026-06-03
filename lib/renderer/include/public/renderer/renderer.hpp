#ifndef __VTX_RENDERER_RENDERER__
#define __VTX_RENDERER_RENDERER__

#include "renderer/builder/render_graph_build.hpp"
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
#include <unordered_set>
#include <util/callback.hpp>
#include <util/math/bitset.hpp>
#include <vector>

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
		inline size_t width() const { return _width; }

		inline size_t height() const { return _height; }

		inline RenderGraph & graph() { return _graph; }

		/**
		 * @brief Set graphic context.
		 */
		void setDefault();
		void setOpenGL( const Desc::NativeContextInfo &, const FilePath & p_shaderDir );

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
		void setVoxels( std::span<const Vec3f>, std::span<const Vec3f> );

		/**
		 * @brief Add/remove systems.
		 */
		Desc::Handle addSystem( Cache::System && );
		void		 patchSystem( const Desc::Handle, Cache::System::Data && );
		void		 removeSystem( const Desc::Handle );

		/**
		 * @brief Add/remove representation presets.
		 */
		Desc::Handle addRepresentation( const Representation & );
		void		 patchRepresentation( const Desc::Handle, Cache::Representation::Data && );
		void		 removeRepresentation( const Desc::Handle );

		void setRepresentationDirty( const Desc::Handle, const Cache::E_REPRESENTATION_DIRTY );

		/**
		 * @brief Ensure a physical chunk exists for a chunked render graph buffer.
		 * @return true
		 * if a command buffer rebuild was required.
		 */
		bool ensureBufferChunk( const Desc::BufferRef & );
		bool releaseBufferChunk( const Desc::BufferRef & );

		/**
		 * @brief Push system data.
		 */
		void setSystemTransform( const Desc::Handle, const Mat4f & );

		/**
		 * @brief Push frame.
		 */
		void setSystemPositions( const Desc::Handle, std::span<const Vec3f> );

		/**
		 * @brief Update system data.
		 */
		void setSystemDirty( const Desc::Handle, const Cache::E_SYSTEM_DIRTY );

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
		void setNeedUpdate( const bool p_value ) { _dirtyRenderer |= Cache::E_RENDERER_DIRTY::NEED_UPDATE; }

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
		Util::Callback<const StructInfos &> onReady;

	  private:
		/**
		 * @brief Render _graph to handle the rendering pipeline.
		 */
		RenderGraph _graph;

		/**
		 * @brief Current pipeline configuration.
		 */
		std::optional<Builder::PipelineConfig> _config;
		Cache::GraphicsConfig				   _graphicsConfig;
		Cache::ColorLayout					   _colorLayout;

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
		 * @brief Dirty entries.
		 */
		template<typename T>
			requires std::is_enum_v<T>
		struct DirtyEntry
		{
			Desc::Handle handle;
			T			 flags;
		};

		using DirtySystem		  = DirtyEntry<Cache::E_SYSTEM_DIRTY>;
		using DirtyRepresentation = DirtyEntry<Cache::E_REPRESENTATION_DIRTY>;

		/**
		 * @brief Pending renderer refreshes flushed at frame start.
		 */
		Cache::E_RENDERER_DIRTY			 _dirtyRenderer = Cache::E_RENDERER_DIRTY::NONE;
		std::vector<DirtySystem>		 _dirtySystems;
		std::vector<DirtyRepresentation> _dirtyRepresentations;

		/**
		 * @brief Renderer infos.
		 */
		StructInfos _infos;

		/**
		 * @brief Cached data to update.
		 */
		Cache::Camera						_camera;
		Cache::Voxels						_voxels;
		ResourcePool<Cache::System>			_systems;
		ResourcePool<Cache::Representation> _representations;

		/**
		 * @brief Flush pending renderer refreshes.
		 */
		void _flushDirty();

		static void _executeSESExternalPass( uintptr_t );

		/**
		 * @brief The main render loop that call each generated instruction.
		 */
		inline void _render( const float p_deltaTime, const float p_elapsedTime ) const noexcept { _context.execute(); }
	};
} // namespace VTX::Renderer
#endif
