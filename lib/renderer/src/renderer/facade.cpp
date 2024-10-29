#include "renderer/facade.hpp"
#include "renderer/renderer.hpp"

namespace VTX::Renderer
{
	Facade::~Facade() = default;

	Facade::Facade( const size_t p_width, const size_t p_height, const FilePath & p_shaderPath, void * p_loader )
	{
		_renderer = std::make_unique<Renderer>( p_width, p_height, p_shaderPath, p_loader );
	}

	void Facade::resize( const size_t p_width, const size_t p_height, const uint p_output )
	{
		_renderer->resize( p_width, p_height, p_output );
	}

	void Facade::build( const uint p_output, void * p_loader ) { _renderer->build( p_output, p_loader ); }

	void Facade::render( const float p_deltaTime, const float p_elapsedTime )
	{
		_renderer->render( p_deltaTime, p_elapsedTime );
	}

	void Facade::setOutput( const uint p_output ) { _renderer->setOutput( p_output ); }

	void Facade::addProxyMolecule( Proxy::System & p_proxy ) { _renderer->addProxyMolecule( p_proxy ); }
	void Facade::removeProxyMolecule( Proxy::System & p_proxy ) { _renderer->removeProxyMolecule( p_proxy ); }
	void Facade::addProxyMolecules( std::vector<Proxy::System *> & p_proxies )
	{
		_renderer->addProxyMolecules( p_proxies );
	}
	void Facade::removeProxyMolecules( std::vector<Proxy::System *> & p_proxies )
	{
		_renderer->removeProxyMolecules( p_proxies );
	}

	void Facade::addProxyRepresentation( Proxy::Representation & p_proxy )
	{
		_renderer->addProxyRepresentation( p_proxy );
	}
	void Facade::removeProxyRepresentation( Proxy::Representation & p_proxy )
	{
		_renderer->removeProxyRepresentation( p_proxy );
	}
	void Facade::addProxyRepresentations( std::vector<Proxy::Representation *> & p_proxies )
	{
		_renderer->addProxyRepresentations( p_proxies );
	}
	void Facade::removeProxyRepresentations( std::vector<Proxy::Representation *> & p_proxies )
	{
		_renderer->removeProxyRepresentations( p_proxies );
	}

	void Facade::setProxyCamera( Proxy::Camera & p_proxy ) { _renderer->setProxyCamera( p_proxy ); }

	void Facade::setProxyColorLayout( Proxy::ColorLayout & p_proxy ) { _renderer->setProxyColorLayout( p_proxy ); }

	void Facade::setProxyRenderSettings( Proxy::RenderSettings & p_proxy )
	{
		_renderer->setProxyRenderSettings( p_proxy );
	}

	void Facade::setProxyVoxels( Proxy::Voxels & p_proxy ) { _renderer->setProxyVoxels( p_proxy ); }

	void Facade::snapshot(
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

	Vec2i Facade::getPickedIds( const size_t p_x, const size_t p_y ) const
	{
		return _renderer->getPickedIds( p_x, p_y );
	}

	void Facade::setNeedUpdate( const bool p_value ) { _renderer->setNeedUpdate( p_value ); }

	StructInfos Facade::getInfos() const { return _renderer->getInfos(); }

	Util::Callback<> & Facade::onReady() { return _renderer->onReady; }

} // namespace VTX::Renderer
