#include "renderer/facade.hpp"
#include "renderer/renderer.hpp"

namespace VTX::Renderer
{
	Facade::~Facade() = default;

	Facade::Facade( const size_t p_width, const size_t p_height, const FilePath & p_shaderPath, void * p_loader )
	{
		_renderer		   = std::make_unique<Renderer>( p_width, p_height, p_shaderPath, p_loader );
		showAtoms		   = &_renderer->showAtoms;
		showBonds		   = &_renderer->showBonds;
		showRibbons		   = &_renderer->showRibbons;
		showVoxels		   = &_renderer->showVoxels;
		forceUpdate		   = &_renderer->forceUpdate;
		logDurations	   = &_renderer->logDurations;
		drawRangeSpheres   = &_renderer->drawRangeSpheres;
		drawRangeCylinders = &_renderer->drawRangeCylinders;
		drawRangeRibbons   = &_renderer->drawRangeRibbons;
		drawRangeVoxels	   = &_renderer->drawRangeVoxels;
	}

	void Facade::resize( const size_t p_width, const size_t p_height ) { _renderer->resize( p_width, p_height ); }

	void Facade::build( const uint p_output, void * p_loader ) { _renderer->build( p_output, p_loader ); }

	void Facade::clean() { _renderer->clean(); }

	void Facade::render( const float p_deltaTime, const float p_elapsedTime )
	{
		_renderer->render( p_deltaTime, p_elapsedTime );
	}

	void Facade::setOutput( const uint p_output ) { _renderer->setOutput( p_output ); }

	void Facade::addProxySystem( Proxy::System & p_proxy ) { _renderer->addProxySystem( p_proxy ); }

	void Facade::removeProxySystem( Proxy::System & p_proxy ) { _renderer->removeProxySystem( p_proxy ); }

	void Facade::addProxySystems( std::vector<Proxy::System *> & p_proxies )
	{
		_renderer->addProxySystems( p_proxies );
	}

	void Facade::removeProxySystems( std::vector<Proxy::System *> & p_proxies )
	{
		_renderer->removeProxySystems( p_proxies );
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

	size_t Facade::getWidth() const { return _renderer->width; }

	size_t Facade::getHeight() const { return _renderer->height; }

	Vec2i Facade::getPickedIds( const size_t p_x, const size_t p_y ) const
	{
		return _renderer->getPickedIds( p_x, p_y );
	}

	void Facade::setNeedUpdate( const bool p_value ) { _renderer->setNeedUpdate( p_value ); }

	StructInfos Facade::getInfos() const { return _renderer->getInfos(); }

	uint Facade::getBufferCount() { return Renderer::BUFFER_COUNT; }

	void Facade::addPass( const Pass & p_pass ) { _renderer->addPass( p_pass ); }

	void Facade::removePass( const Pass * const p_pass ) { _renderer->removePass( p_pass ); }

	bool Facade::addLink(
		Pass * const	   p_passSrc,
		Pass * const	   p_passDest,
		const E_CHAN_OUT & p_channelSrc,
		const E_CHAN_IN &  p_channelDest
	)
	{
		return _renderer->addLink( p_passSrc, p_passDest, p_channelSrc, p_channelDest );
	}

	void Facade::removeLink( const Link * const p_link ) { _renderer->removeLink( p_link ); }

	const Passes & Facade::getPasses() const { return _renderer->getPasses(); }

	const Links & Facade::getLinks() const { return _renderer->getLinks(); }

	const RenderQueue & Facade::getRenderQueue() const { return _renderer->getRenderQueue(); }

	const Output * const Facade::getOutput() const { return _renderer->getOutput(); }

	void Facade::setOutput( const Output * const p_output ) { _renderer->setOutput( p_output ); }

	void Facade::compileShaders() const { _renderer->compileShaders(); }

	const InstructionsDurationRanges & Facade::getInstructionsDurationRanges() const
	{
		return _renderer->getInstructionsDurationRanges();
	}

	const std::vector<Pass *> & Facade::getAvailablePasses() const { return _renderer->getAvailablePasses(); }

	Util::Callback<> & Facade::onReady() { return _renderer->onReady; }

	void Facade::setValue( const float p_value, const std::string & p_key, const size_t p_index )
	{
		_renderer->setValue( p_value, p_key, p_index );
	}

	void Facade::setValue( const uint p_value, const std::string & p_key, const size_t p_index )
	{
		_renderer->setValue( p_value, p_key, p_index );
	}

	void Facade::setValue( const int p_value, const std::string & p_key, const size_t p_index )
	{
		_renderer->setValue( p_value, p_key, p_index );
	}

	void Facade::setValue( const bool p_value, const std::string & p_key, const size_t p_index )
	{
		_renderer->setValue( p_value, p_key, p_index );
	}

	void Facade::setValue( const Vec2f & p_value, const std::string & p_key, const size_t p_index )
	{
		_renderer->setValue( p_value, p_key, p_index );
	}

	void Facade::setValue( const Vec2i & p_value, const std::string & p_key, const size_t p_index )
	{
		_renderer->setValue( p_value, p_key, p_index );
	}

	void Facade::setValue( const Vec3f & p_value, const std::string & p_key, const size_t p_index )
	{
		_renderer->setValue( p_value, p_key, p_index );
	}

	void Facade::setValue( const Vec4f & p_value, const std::string & p_key, const size_t p_index )
	{
		_renderer->setValue( p_value, p_key, p_index );
	}

} // namespace VTX::Renderer
