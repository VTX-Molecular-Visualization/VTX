#include "app/component/representation/render_settings.hpp"
#include "app/services.hpp"
#include <renderer/facade.hpp>

namespace VTX::App::Component::Representation
{
	RenderSettings::RenderSettings( Library::Preset::RenderSettings & p_preset ) :
		Core::ECS::BaseComponentProxyPreset<Renderer::Proxy::RenderSettings, App::Library::Preset::RenderSettings>(
			p_preset
		)
	{
	}

	void RenderSettings::setupProxy()
	{
		using namespace Renderer;
		using namespace VTX::Renderer::Settings;
		using namespace VTX::Renderer::Proxy;

		_proxy = std::make_unique<VTX::Renderer::Proxy::RenderSettings>( _preset.getData() );

		_preset.getCallback<E_RENDER_SETTINGS::SHADING_MODE, E_SHADING>() +=
			[ this ]( const E_SHADING p_value ) { _proxy->getCallback<E_RENDER_SETTINGS::SHADING_MODE>()(); };
		_preset.getCallback<E_RENDER_SETTINGS::COLOR_LIGHT, Util::Color::Rgba>() +=
			[ this ]( const Util::Color::Rgba p_value ) { _proxy->getCallback<E_RENDER_SETTINGS::COLOR_LIGHT>()(); };
		_preset.getCallback<E_RENDER_SETTINGS::COLOR_BACKGROUND, Util::Color::Rgba>() +=
			[ this ]( const Util::Color::Rgba p_value )
		{ _proxy->getCallback<E_RENDER_SETTINGS::COLOR_BACKGROUND>()(); };
		_preset.getCallback<E_RENDER_SETTINGS::SPECULAR_FACTOR, float>() +=
			[ this ]( const float p_value ) { _proxy->getCallback<E_RENDER_SETTINGS::SPECULAR_FACTOR>()(); };
		_preset.getCallback<E_RENDER_SETTINGS::SHININESS, float>() +=
			[ this ]( const float p_value ) { _proxy->getCallback<E_RENDER_SETTINGS::SHININESS>()(); };
		_preset.getCallback<E_RENDER_SETTINGS::TOON_STEPS, uint>() +=
			[ this ]( const uint p_value ) { _proxy->getCallback<E_RENDER_SETTINGS::TOON_STEPS>()(); };
		_preset.getCallback<E_RENDER_SETTINGS::ACTIVE_SSAO, bool>() +=
			[ this ]( const bool p_value ) { _proxy->getCallback<E_RENDER_SETTINGS::ACTIVE_SSAO>()(); };
		_preset.getCallback<E_RENDER_SETTINGS::SSAO_INTENSITY, float>() +=
			[ this ]( const float p_value ) { _proxy->getCallback<E_RENDER_SETTINGS::SSAO_INTENSITY>()(); };
		_preset.getCallback<E_RENDER_SETTINGS::BLUR_SIZE, float>() +=
			[ this ]( const float p_value ) { _proxy->getCallback<E_RENDER_SETTINGS::BLUR_SIZE>()(); };
		_preset.getCallback<E_RENDER_SETTINGS::ACTIVE_OUTLINE, bool>() +=
			[ this ]( const bool p_value ) { _proxy->getCallback<E_RENDER_SETTINGS::ACTIVE_OUTLINE>()(); };
		_preset.getCallback<E_RENDER_SETTINGS::COLOR_OUTLINE, Util::Color::Rgba>() +=
			[ this ]( const Util::Color::Rgba p_value ) { _proxy->getCallback<E_RENDER_SETTINGS::COLOR_OUTLINE>()(); };
		_preset.getCallback<E_RENDER_SETTINGS::OUTLINE_SENSITIVITY, float>() +=
			[ this ]( const float p_value ) { _proxy->getCallback<E_RENDER_SETTINGS::OUTLINE_SENSITIVITY>()(); };
		_preset.getCallback<E_RENDER_SETTINGS::OUTLINE_THICKNESS, uint>() +=
			[ this ]( const uint p_value ) { _proxy->getCallback<E_RENDER_SETTINGS::OUTLINE_THICKNESS>()(); };
		_preset.getCallback<E_RENDER_SETTINGS::ACTIVE_FOG, bool>() +=
			[ this ]( const bool p_value ) { _proxy->getCallback<E_RENDER_SETTINGS::ACTIVE_FOG>()(); };
		_preset.getCallback<E_RENDER_SETTINGS::COLOR_FOG, Util::Color::Rgba>() +=
			[ this ]( const Util::Color::Rgba p_value ) { _proxy->getCallback<E_RENDER_SETTINGS::COLOR_FOG>()(); };
		_preset.getCallback<E_RENDER_SETTINGS::FOG_NEAR, float>() +=
			[ this ]( const float p_value ) { _proxy->getCallback<E_RENDER_SETTINGS::FOG_NEAR>()(); };
		_preset.getCallback<E_RENDER_SETTINGS::FOG_FAR, float>() +=
			[ this ]( const float p_value ) { _proxy->getCallback<E_RENDER_SETTINGS::FOG_FAR>()(); };
		_preset.getCallback<E_RENDER_SETTINGS::FOG_DENSITY, float>() +=
			[ this ]( const float p_value ) { _proxy->getCallback<E_RENDER_SETTINGS::FOG_DENSITY>()(); };
		_preset.getCallback<E_RENDER_SETTINGS::ACTIVE_SELECTION, bool>() +=
			[ this ]( const bool p_value ) { _proxy->getCallback<E_RENDER_SETTINGS::ACTIVE_SELECTION>()(); };
		_preset.getCallback<E_RENDER_SETTINGS::COLOR_SELECTION, Util::Color::Rgba>() +=
			[ this ]( const Util::Color::Rgba p_value )
		{ _proxy->getCallback<E_RENDER_SETTINGS::COLOR_SELECTION>()(); };

		RENDERER().onReady( [ this ]() { RENDERER().setProxyRenderSettings( *_proxy ); } );
	}

} // namespace VTX::App::Component::Representation
