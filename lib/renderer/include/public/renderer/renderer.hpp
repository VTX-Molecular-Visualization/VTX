#ifndef __VTX_RENDERER_RENDERER__
#define __VTX_RENDERER_RENDERER__

#undef VTX_CUDA_ENABLED

#ifdef VTX_CUDA_ENABLED
#include "bcs/sesdf/sesdf.hpp"
#endif
#include "renderer/camera.hpp"
#include "renderer/color.hpp"
#include "renderer/context/context_wrapper.hpp"
#include "renderer/geometry/geometries.hpp"
#include "renderer/graphics_config.hpp"
#include "renderer/render_graph.hpp"
#include "renderer/representation.hpp"
#include "renderer/struct_infos.hpp"
#include <util/callback.hpp>
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
		void setOpenGL45( const FilePath & );

		/**
		 * @brief Build the renderer with the current _graph.
		 */
		void build();

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
		 */
		void render( const float, const float ) noexcept;

		/**
		 * @brief Push data to the renderer.
		 */
		void setCamera( const Camera &, const Vec3f &, const Mat4f &, const Mat4f & );
		void setGraphicsConfig( const GraphicsConfig & );
		void setColorLayout( const Color::Layout & );
		void setRepresentation( const Representation & );
		void setVoxels( const std::vector<Vec3f> &, const std::vector<Vec3f> & );

		/**
		 * @brief Add / remove / update system.
		 */
		void addSystem();
		void removeSystem();

		/**
		 * @brief Exports the renderer to an array of pixels.
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
			// return _context.getTextureData<Vec2i>( "Geometric", p_x, _height - p_y, E_CHAN_OUT::COLOR_2 );
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
		const StructInfos & getInfos( const bool = false );

		/**
		 * @brief Buffer swapping count.
		 */
		static constexpr size_t BUFFER_COUNT = 2;

		/**
		 * @brief Force update each frame.
		 */
		bool forceUpdate = true;

		Util::Callback<> onReady;

#ifdef VTX_CUDA_ENABLED
		std::unique_ptr<bcs::Sesdf> _sesData;
		bcs::sesdf::SesdfGraphics	_sesSurface {};
		GLuint						_sesVao		   = GL_INVALID_VALUE;
		GLuint						_sesSegmentVao = GL_INVALID_VALUE;
		GLuint						_sesCircleVao  = GL_INVALID_VALUE;
		GLuint						_sesConvexVao  = GL_INVALID_VALUE;

		Context::GL::ProgramManager _pm
			= Context::GL::ProgramManager( VTX::Util::Filesystem::getExecutableDir() / "shaders" );
		Context::GL::Program * _sesProgramConcave;
		Context::GL::Program * _sesProgramSegment;
		Context::GL::Program * _sesProgramCircle;
		Context::GL::Program * _sesProgramConvex;
#endif
		// void _createSes( Proxy::System & p_proxy );

	  private:
		/**
		 * @brief Render _graph to handle the rendering pipeline.
		 */
		RenderGraph _graph;

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
		 * @brief Update next frame.
		 */
		bool   _needUpdate		= false;
		size_t _framesRemaining = BUFFER_COUNT;

		/**
		 * @brief Renderer infos.
		 */
		StructInfos _infos;

		/**
		 * @brief Refresh the render graph according to the graphics config.
		 */
		void _refreshGraph( const GraphicsConfig & );

		// TODO: make "filler" functions for each type of data instead of _setDataX?
		inline void _refreshDataSystems()
		{
			//_refreshDataSpheresCylinders();
			//_refreshDataRibbons();
			_refreshDataModels();

			setNeedUpdate( true );
		}

		void _refreshDataModels();

		void _applyRepresentationLogic( const Representation & );

		enum E_ELEMENT_FLAGS
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
