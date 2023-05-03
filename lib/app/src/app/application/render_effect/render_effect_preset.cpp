#include "app/application/render_effect/render_effect_preset.hpp"
#include "app/application/scene.hpp"
#include "app/application/setting.hpp"
#include "app/component/render/camera.hpp"
#include "app/old_app/vtx_app.hpp"
#include "app/render/renderer/gl/gl.hpp"

namespace VTX::App::Application::RenderEffect
{
	RenderEffectPreset::RenderEffectPreset() : BaseModel( VTX::ID::Model::MODEL_RENDERER_RENDER_EFFECT_PRESET )
	{
		_shading = VTX::App::Application::Setting::SHADING_DEFAULT;

		_ssao		   = VTX::App::Application::Setting::ACTIVE_AO_DEFAULT;
		_ssaoIntensity = VTX::App::Application::Setting::AO_INTENSITY_DEFAULT;
		_ssaoBlurSize  = VTX::App::Application::Setting::AO_BLUR_SIZE_DEFAULT;

		_outline		  = VTX::App::Application::Setting::ACTIVE_OUTLINE_DEFAULT;
		_outlineThickness = VTX::App::Application::Setting::OUTLINE_THICKNESS_DEFAULT;
		_outlineSensivity = VTX::App::Application::Setting::OUTLINE_SENSIVITY_DEFAULT;
		_outlineColor	  = VTX::App::Application::Setting::OUTLINE_COLOR_DEFAULT;

		_fog		= VTX::App::Application::Setting::ACTIVE_FOG_DEFAULT;
		_fogNear	= VTX::App::Application::Setting::FOG_NEAR_DEFAULT;
		_fogFar		= VTX::App::Application::Setting::FOG_FAR_DEFAULT;
		_fogDensity = VTX::App::Application::Setting::FOG_DENSITY_DEFAULT;
		_fogColor	= VTX::App::Application::Setting::FOG_COLOR_DEFAULT;

		_backgroundColor  = VTX::App::Application::Setting::BACKGROUND_COLOR_DEFAULT;
		_cameraLightColor = VTX::App::Application::Setting::LIGHT_COLOR_DEFAULT;
	}

	void RenderEffectPreset::setName( const std::string & p_name )
	{
		_name = std::string( p_name );
		_notifyViews( App::Event::Model::DISPLAY_NAME_CHANGE );
	}
	void RenderEffectPreset::setIconPath( const std::string & p_iconPath )
	{
		_iconPath = std::string( p_iconPath );
		_notifyDataChanged();
	}
	void RenderEffectPreset::setQuickAccess( const bool p_quickAccess )
	{
		_quickAccess = p_quickAccess;
		_notifyViews( App::Event::Model::QUICK_ACCESS_CHANGE );
	}

	void RenderEffectPreset::enableSSAO( const bool p_enable )
	{
		_ssao = p_enable;
		_notifyDataChanged();
	}

	void RenderEffectPreset::setSSAOIntensity( const int p_ssaoIntensity )
	{
		_ssaoIntensity = Util::Math::clamp( p_ssaoIntensity,
											VTX::App::Application::Setting::AO_INTENSITY_MIN,
											VTX::App::Application::Setting::AO_INTENSITY_MAX );
		_notifyDataChanged();
	}
	void RenderEffectPreset::setSSAOBlurSize( const int p_ssaoBlurSize )
	{
		_ssaoBlurSize = Util::Math::clamp( p_ssaoBlurSize,
										   VTX::App::Application::Setting::AO_BLUR_SIZE_MIN,
										   VTX::App::Application::Setting::AO_BLUR_SIZE_MAX );
		_notifyDataChanged();
	}

	void RenderEffectPreset::enableOutline( const bool p_enable )
	{
		_outline = p_enable;
		_notifyDataChanged();
	}
	void RenderEffectPreset::setOutlineThickness( const uint p_outlineThickness )
	{
		_outlineThickness = Util::Math::clamp( p_outlineThickness,
											   VTX::App::Application::Setting::OUTLINE_THICKNESS_MIN,
											   VTX::App::Application::Setting::OUTLINE_THICKNESS_MAX );
		_notifyDataChanged();
	}
	void RenderEffectPreset::setOutlineSensivity( const float p_outlineSensivity )
	{
		_outlineSensivity = Util::Math::clamp( p_outlineSensivity,
											   VTX::App::Application::Setting::OUTLINE_SENSIVITY_MIN,
											   VTX::App::Application::Setting::OUTLINE_SENSIVITY_MAX );
		_notifyDataChanged();
	}
	void RenderEffectPreset::setOutlineColor( const Util::Color::Rgba & p_outlineColor )
	{
		_outlineColor = p_outlineColor;
		_notifyDataChanged();
	}

	void RenderEffectPreset::enableFog( const bool p_enable )
	{
		_fog = p_enable;
		_notifyDataChanged();
	}
	void RenderEffectPreset::setFogNear( const float p_fogNear )
	{
		_fogNear = p_fogNear <= _fogFar ? p_fogNear : _fogFar;
		_notifyDataChanged();
	}
	void RenderEffectPreset::setFogFar( const float p_fogFar )
	{
		_fogFar = p_fogFar > _fogNear ? p_fogFar : _fogNear;
		_notifyDataChanged();
	}
	void RenderEffectPreset::setFogDensity( const float p_fogDensity )
	{
		_fogDensity = Util::Math::clamp( p_fogDensity, 0.0f, 1.0f );
		_notifyDataChanged();
	}
	void RenderEffectPreset::setFogColor( const Util::Color::Rgba & p_fogColor )
	{
		_fogColor = p_fogColor;
		_notifyDataChanged();
	}
	void RenderEffectPreset::setBackgroundColor( const Util::Color::Rgba & p_backgroundColor )
	{
		_backgroundColor = p_backgroundColor;
		_notifyDataChanged();
	}
	void RenderEffectPreset::setCameraLightColor( const Util::Color::Rgba & p_cameraLightColor )
	{
		_cameraLightColor = p_cameraLightColor;
		_notifyDataChanged();
	}

	void RenderEffectPreset::copyFrom( const RenderEffectPreset & p_source )
	{
		_name		 = p_source._name;
		_iconPath	 = p_source._iconPath;
		_quickAccess = p_source._quickAccess;

		_shading = p_source._shading;

		_ssao		   = p_source._ssao;
		_ssaoIntensity = p_source._ssaoIntensity;
		_ssaoBlurSize  = p_source._ssaoBlurSize;

		_outline		  = p_source._outline;
		_outlineThickness = p_source._outlineThickness;
		_outlineColor	  = p_source._outlineColor;

		_fog		= p_source._fog;
		_fogNear	= p_source._fogNear;
		_fogFar		= p_source._fogFar;
		_fogDensity = p_source._fogDensity;
		_fogColor	= p_source._fogColor;

		_backgroundColor  = p_source._backgroundColor;
		_cameraLightColor = p_source._cameraLightColor;
	}

} // namespace VTX::App::Application::RenderEffect
