#include "app/component/representation/representation.hpp"
#include "app/core/renderer/renderer_system.hpp"

namespace VTX::App::Component::Representation
{
	Representation::Representation( Library::Preset::Representation & p_preset ) : BaseComponentProxyPreset( p_preset )
	{
	}

	void Representation::setupProxy()
	{
		using namespace VTX::Core::Struct;
		using namespace Renderer::Proxy;

		_proxy = std::make_unique<Renderer::Proxy::Representation>( _preset.getData() );

		_preset.getCallback<E_REPRESENTATION_VALUES::HAS_SPHERE, bool>() +=
			[ this ]( const bool p_value ) { _proxy->getCallback<E_REPRESENTATION_VALUES::HAS_SPHERE>()(); };
		_preset.getCallback<E_REPRESENTATION_VALUES::RADIUS_SPHERE_FIXED, float>() +=
			[ this ]( const float p_value ) { _proxy->getCallback<E_REPRESENTATION_VALUES::RADIUS_SPHERE_FIXED>()(); };
		_preset.getCallback<E_REPRESENTATION_VALUES::RADIUS_SPHERE_ADD, float>() +=
			[ this ]( const float p_value ) { _proxy->getCallback<E_REPRESENTATION_VALUES::RADIUS_SPHERE_ADD>()(); };
		_preset.getCallback<E_REPRESENTATION_VALUES::IS_SPHERE_RADIUS_FIXED, bool>() += [ this ]( const bool p_value )
		{ _proxy->getCallback<E_REPRESENTATION_VALUES::IS_SPHERE_RADIUS_FIXED>()(); };

		_preset.getCallback<E_REPRESENTATION_VALUES::HAS_CYLINDER, bool>() +=
			[ this ]( const bool p_value ) { _proxy->getCallback<E_REPRESENTATION_VALUES::HAS_CYLINDER>()(); };
		_preset.getCallback<E_REPRESENTATION_VALUES::RADIUS_CYLINDER, float>() +=
			[ this ]( const float p_value ) { _proxy->getCallback<E_REPRESENTATION_VALUES::RADIUS_CYLINDER>()(); };
		_preset.getCallback<E_REPRESENTATION_VALUES::CYLINDER_COLOR_BLENDING, bool>() += [ this ]( const bool p_value )
		{ _proxy->getCallback<E_REPRESENTATION_VALUES::CYLINDER_COLOR_BLENDING>()(); };

		_preset.getCallback<E_REPRESENTATION_VALUES::HAS_RIBBON, bool>() +=
			[ this ]( const bool p_value ) { _proxy->getCallback<E_REPRESENTATION_VALUES::HAS_RIBBON>()(); };
		_preset.getCallback<E_REPRESENTATION_VALUES::RIBBON_COLOR_BLENDING, bool>() +=
			[ this ]( const bool p_value ) { _proxy->getCallback<E_REPRESENTATION_VALUES::RIBBON_COLOR_BLENDING>()(); };

		_preset.getCallback<E_REPRESENTATION_VALUES::HAS_SES, bool>() +=
			[ this ]( const bool p_value ) { _proxy->getCallback<E_REPRESENTATION_VALUES::HAS_SES>()(); };
		_preset.getCallback<E_REPRESENTATION_VALUES::SES_PROBE_RADIUS, float>() +=
			[ this ]( const float p_value ) { _proxy->getCallback<E_REPRESENTATION_VALUES::SES_PROBE_RADIUS>()(); };

		RENDERER_SYSTEM().onReady( [ this ]() { RENDERER_SYSTEM().setProxyRepresentation( *_proxy ); } );
	}

} // namespace VTX::App::Component::Representation
