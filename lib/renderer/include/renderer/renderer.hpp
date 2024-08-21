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
#include "proxy/representation.hpp"
#include "proxy/voxels.hpp"
#include "render_graph.hpp"
#include "scheduler/depth_first_search.hpp"
#include <util/callback.hpp>
#include <util/chrono.hpp>
#include <util/logger.hpp>
#include <util/math/range_list.hpp>

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
		);

		template<typename T>
		inline void setValue( const T & p_value, const std::string & p_key, const size_t p_index = 0 )
		{
			assert( _context != nullptr );
			_context->setValue<T>( p_value, p_key, p_index );
			setNeedUpdate( true );
		}

		inline void resize( const size_t p_width, const size_t p_height, const uint p_output = 0 )
		{
			VTX_TRACE( "Resizing renderer to {}x{}", width, height );

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
			_context->getTextureData( idsAny, p_x, height - p_y, "Geometric", E_CHAN_OUT::COLOR_2 );
			return std::any_cast<Vec2i>( idsAny );
		}

		inline void setNeedUpdate( const bool p_value )
		{
			_needUpdate = p_value;
			if ( not p_value )
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
		std::map<const Proxy::Molecule * const, Cache::SES>			   _cacheSES;

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
