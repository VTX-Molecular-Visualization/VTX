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
			_renderer->resize( p_width, p_height, p_output );
		}

		inline void build( const uint p_output = 0, void * p_loader = nullptr )
		{
			_renderer->build( p_output, p_loader );
		}

		// inline void clean() { _renderer->clean(); }

		inline void render( const float p_time ) { _renderer->render( p_time ); }

		inline void setOutput( const uint p_output ) { _renderer->setOutput( p_output ); }

		inline void addProxyMolecule( Proxy::Molecule & p_proxy ) { _renderer->addProxyMolecule( p_proxy ); }
		inline void removeProxyMolecule( Proxy::Molecule & p_proxy ) { _renderer->removeProxyMolecule( p_proxy ); }
		inline void addProxyMolecules( std::vector<Proxy::Molecule *> & p_proxies )
		{
			_renderer->addProxyMolecules( p_proxies );
		}
		inline void removeProxyMolecules( std::vector<Proxy::Molecule *> & p_proxies )
		{
			_renderer->removeProxyMolecules( p_proxies );
		}

		inline void addProxyRepresentation( Proxy::Representation & p_proxy )
		{
			_renderer->addProxyRepresentation( p_proxy );
		}
		inline void removeProxyRepresentation( Proxy::Representation & p_proxy )
		{
			_renderer->removeProxyRepresentation( p_proxy );
		}
		inline void addProxyRepresentations( std::vector<Proxy::Representation *> & p_proxies )
		{
			_renderer->addProxyRepresentations( p_proxies );
		}
		inline void removeProxyRepresentations( std::vector<Proxy::Representation *> & p_proxies )
		{
			_renderer->removeProxyRepresentations( p_proxies );
		}

		inline void setProxyCamera( Proxy::Camera & p_proxy ) { _renderer->setProxyCamera( p_proxy ); }

		inline void setProxyColorLayout( Proxy::ColorLayout & p_proxy ) { _renderer->setProxyColorLayout( p_proxy ); }

		inline void setProxyRenderSettings( Proxy::RenderSettings & p_proxy )
		{
			_renderer->setProxyRenderSettings( p_proxy );
		}

		inline void setProxyVoxels( Proxy::Voxels & p_proxy ) { _renderer->setProxyVoxels( p_proxy ); }

		inline void snapshot(
			std::vector<uchar> & p_image,
			const size_t		 p_width,
			const size_t		 p_height,
			const float			 p_fov,
			const float			 p_near,
			const float			 p_far
		)
		{
			_renderer->snapshot( p_image, p_width, p_height, p_fov, p_near, p_far );
		}

		inline Vec2i getPickedIds( const size_t p_x, const size_t p_y ) const
		{
			return _renderer->getPickedIds( p_x, p_y );
		}

		inline void setNeedUpdate( const bool p_value ) { _renderer->setNeedUpdate( p_value ); }

	  private:
		std::unique_ptr<Renderer> _renderer;
	};

} // namespace VTX::Renderer

#endif
