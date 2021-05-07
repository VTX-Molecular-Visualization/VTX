#include "render_effect_preset.hpp"
#include "object3d/camera.hpp"
#include "object3d/scene.hpp"
#include "renderer/gl/gl.hpp"
#include "setting.hpp"
#include "ui/main_window.hpp"
#include "vtx_app.hpp"

namespace VTX::Model::Renderer
{
	RenderEffectPreset::RenderEffectPreset() : BaseModel( ID::Model::MODEL_RENDERER_RENDER_EFFECT_PRESET )
	{
		_shading = Setting::SHADING_DEFAULT;

		_ssao		   = Setting::ACTIVE_AO_DEFAULT;
		_ssaoIntensity = Setting::AO_INTENSITY_DEFAULT;
		_ssaoBlurSize  = Setting::AO_BLUR_SIZE_DEFAULT;

		_outline		  = Setting::ACTIVE_OUTLINE_DEFAULT;
		_outlineThickness = Setting::OUTLINE_THICKNESS_DEFAULT;
		_outlineColor	  = Setting::OUTLINE_COLOR_DEFAULT;

		_fog		= Setting::ACTIVE_FOG_DEFAULT;
		_fogNear	= Setting::FOG_NEAR_DEFAULT;
		_fogFar		= Setting::FOG_FAR_DEFAULT;
		_fogDensity = Setting::FOG_DENSITY_DEFAULT;
		_fogColor	= Setting::FOG_COLOR_DEFAULT;

		_backgroundColor	   = Setting::BACKGROUND_COLOR_DEFAULT;
		_cameraLightColor	   = Setting::LIGHT_COLOR_DEFAULT;
		_cameraFOV			   = Setting::CAMERA_FOV_DEFAULT;
		_cameraNearClip		   = Setting::CAMERA_NEAR_DEFAULT;
		_cameraFarClip		   = Setting::CAMERA_FAR_DEFAULT;
		_antiAliasing		   = Setting::ACTIVE_AA_DEFAULT;
		_perspectiveProjection = Setting::CAMERA_PERSPECTIVE_DEFAULT;
	}

	void RenderEffectPreset::setName( const std::string & p_name )
	{
		_name = std::string( p_name );
		_notifyViews( new Event::VTXEvent( Event::Model::DISPLAY_NAME_CHANGE ) );
	}
	void RenderEffectPreset::setIconPath( const std::string & p_iconPath )
	{
		_iconPath = std::string( p_iconPath );
		_notifyDataChanged();
	}
	void RenderEffectPreset::setQuickAccess( const bool p_quickAccess )
	{
		_quickAccess = p_quickAccess;
		_notifyViews( new Event::VTXEvent( Event::Model::QUICK_ACCESS_CHANGE ) );
	}

	void RenderEffectPreset::enableSSAO( const bool p_enable )
	{
		_ssao = p_enable;
		_notifyDataChanged();
	}

	void RenderEffectPreset::setSSAOIntensity( const int p_ssaoIntensity )
	{
		_ssaoIntensity
			= Util::Math::clamp( p_ssaoIntensity, VTX::Setting::AO_INTENSITY_MIN, VTX::Setting::AO_INTENSITY_MAX );
		_notifyDataChanged();
	}
	void RenderEffectPreset::setSSAOBlurSize( const int p_ssaoBlurSize )
	{
		_ssaoBlurSize
			= Util::Math::clamp( p_ssaoBlurSize, VTX::Setting::AO_BLUR_SIZE_MIN, VTX::Setting::AO_BLUR_SIZE_MAX );
		_notifyDataChanged();
	}

	void RenderEffectPreset::enableOutline( const bool p_enable )
	{
		_outline = p_enable;
		_notifyDataChanged();
	}
	void RenderEffectPreset::setOutlineThickness( const float p_outlineThickness )
	{
		_outlineThickness = p_outlineThickness;
		_notifyDataChanged();
	}
	void RenderEffectPreset::setOutlineColor( const Color::Rgb & p_outlineColor )
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
	void RenderEffectPreset::setFogColor( const Color::Rgb & p_fogColor )
	{
		_fogColor = p_fogColor;
		_notifyDataChanged();
	}

	void RenderEffectPreset::setBackgroundColor( const Color::Rgb & p_backgroundColor )
	{
		_backgroundColor = p_backgroundColor;
		_notifyDataChanged();
	}
	void RenderEffectPreset::setCameraLightColor( const Color::Rgb & p_cameraLightColor )
	{
		_cameraLightColor = p_cameraLightColor;
		_notifyDataChanged();
	}
	void RenderEffectPreset::setCameraFOV( const float p_cameraFOV )
	{
		_cameraFOV = p_cameraFOV;
		_notifyDataChanged();
	}
	void RenderEffectPreset::setCameraNearClip( const float p_cameraNearClip )
	{
		_cameraNearClip = p_cameraNearClip;
		_notifyDataChanged();
	}
	void RenderEffectPreset::setCameraFarClip( const float p_cameraFarClip )
	{
		_cameraFarClip = p_cameraFarClip;
		_notifyDataChanged();
	}
	void RenderEffectPreset::setAA( const bool p_antiAliasing )
	{
		_antiAliasing = p_antiAliasing;
		_notifyDataChanged();
	}
	void RenderEffectPreset::setPerspectiveProjection( const bool p_perspectiveProjection )
	{
		_perspectiveProjection = p_perspectiveProjection;
		_notifyDataChanged();
	}

	void RenderEffectPreset::apply() const
	{
		VTX::Renderer::GL::GL & rendererGL = VTXApp::get().getMainWindow().getOpenGLWidget().getRendererGL();
		VTX::Object3D::Camera & camera	   = VTXApp::get().getScene().getCamera();

		// Shading
		VTX_SETTING().shading = _shading;
		rendererGL.setShading();

		// SSAO
		VTX_SETTING().aoIntensity = _ssaoIntensity;
		VTX_SETTING().aoBlurSize  = _ssaoBlurSize;
		VTX_SETTING().activeAO	  = _ssao;
		rendererGL.activeSSAO( _ssao );

		// Outline
		VTX_SETTING().activeOutline = _outline;
		VTX_SETTING().outlineColor	= _outlineColor;
		rendererGL.activeOutline( _outline );

		// Fog
		VTX_SETTING().activeFog	 = _fog;
		VTX_SETTING().fogNear	 = _fogNear;
		VTX_SETTING().fogFar	 = _fogFar;
		VTX_SETTING().fogDensity = _fogDensity;
		VTX_SETTING().fogColor	 = _fogColor;
		rendererGL.activeFog( _fog );

		// Camera
		VTX_SETTING().backgroundColor	= _backgroundColor;
		VTX_SETTING().lightColor		= _cameraLightColor;
		VTX_SETTING().cameraFov			= _cameraFOV;
		VTX_SETTING().cameraNear		= _cameraNearClip;
		VTX_SETTING().cameraFar			= _cameraFarClip;
		VTX_SETTING().activeAA			= _antiAliasing;
		VTX_SETTING().cameraPerspective = _perspectiveProjection;

		camera.setFov( _cameraFOV );
		camera.setNear( _cameraNearClip );
		camera.setFar( _cameraFarClip );
		camera.setPerspective( _perspectiveProjection );

		rendererGL.activeAA( _antiAliasing );
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

		_backgroundColor	   = p_source._backgroundColor;
		_cameraLightColor	   = p_source._cameraLightColor;
		_cameraFOV			   = p_source._cameraFOV;
		_cameraNearClip		   = p_source._cameraNearClip;
		_cameraFarClip		   = p_source._cameraFarClip;
		_antiAliasing		   = p_source._antiAliasing;
		_perspectiveProjection = p_source._perspectiveProjection;
	}

} // namespace VTX::Model::Renderer
