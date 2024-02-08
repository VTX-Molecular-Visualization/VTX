#ifndef __VTX_RENDERER_FACADE__
#define __VTX_RENDERER_FACADE__

#include "renderer.hpp"

namespace VTX::Renderer
{

	class Facade
	{
	  public:
		Facade() = delete;
		Facade( const size_t p_width, const size_t p_height, const FilePath & p_shaderPath, void * p_loader = nullptr )
		{
			_renderer = std::make_unique<Renderer>( p_width, p_height, p_shaderPath, p_loader );
		}

		inline void resize( const size_t p_width, const size_t p_height, const uint p_output = 0 )
		{
			_renderer->resize( p_width, p_height );
		}

		inline void build( const uint p_output = 0, void * p_loader = nullptr )
		{
			_renderer->build( p_output, p_loader );
		}

		// inline void clean() { _renderer->clean(); }

		inline void render( const float p_time ) { _renderer->render( p_time ); }

		inline void setOutput( const uint p_output ) { _renderer->setOutput( p_output ); }

		inline void setCallbackClean( const Renderer::CallbackClean & p_cb ) { _renderer->setCallbackClean( p_cb ); }

		inline void setCallbackReady( const Renderer::CallbackReady & p_cb ) { _renderer->setCallbackReady( p_cb ); }

		inline void setCallbackSnapshotPre( const Renderer::CallbackSnapshotPre & p_cb )
		{
			_renderer->setCallbackSnapshotPre( p_cb );
		}

		inline void setCallbackSnapshotPost( const Renderer::CallbackSnapshotPost & p_cb )
		{
			_renderer->setCallbackSnapshotPost( p_cb );
		}

		inline void setMatrixView( const Mat4f & p_view ) { _renderer->setMatrixView( p_view ); }

		inline void setMatrixProjection( const Mat4f & p_proj ) { _renderer->setMatrixProjection( p_proj ); }

		inline void setCameraPosition( const Vec3f & p_position ) { _renderer->setCameraPosition( p_position ); }

		inline void setCameraClipInfos( const float p_near, const float p_far )
		{
			_renderer->setCameraClipInfos( p_near, p_far );
		}

		inline void setMousePosition( const Vec2i & p_position ) { _renderer->setMousePosition( p_position ); }

		inline void setPerspective( const bool p_perspective ) { _renderer->setPerspective( p_perspective ); }

		inline void addProxy( const Proxy::Molecule & p_proxy ) { _renderer->addProxy( p_proxy ); }

		inline void addProxy( const Proxy::Voxel & p_proxy ) { _renderer->addProxy( p_proxy ); }

		inline void setColorLayout( const std::array<Util::Color::Rgba, 256> & p_layout )
		{
			_renderer->setColorLayout( p_layout );
		}

		inline void snapshot( std::vector<uchar> & p_image, const size_t p_width = 0, const size_t p_height = 0 )
		{
			_renderer->snapshot( p_image, p_width, p_height );
		}

		inline Vec2i getPickedIds( const size_t p_x, const size_t p_y ) const
		{
			return _renderer->getPickedIds( p_x, p_y );
		}

		// inline void setNeedUpdate( const bool p_value ) { _renderer->setNeedUpdate( p_value ); }

	  private:
		std::unique_ptr<Renderer> _renderer;
	};

} // namespace VTX::Renderer

#endif
