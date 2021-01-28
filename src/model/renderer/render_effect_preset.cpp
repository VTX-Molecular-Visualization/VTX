#include "render_effect_preset.hpp"
#include "renderer/gl/gl.hpp"
#include "vtx_app.hpp"

namespace VTX::Model::Renderer
{
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
	};
	void RenderEffectPreset::setSSAOBlurSize( const int p_ssaoBlurSize )
	{
		_ssaoBlurSize
			= Util::Math::clamp( p_ssaoBlurSize, VTX::Setting::AO_BLUR_SIZE_MIN, VTX::Setting::AO_BLUR_SIZE_MAX );
		_notifyDataChanged();
	};

	void RenderEffectPreset::enableOutline( const bool p_enable )
	{
		_outline = p_enable;
		_notifyDataChanged();
	};
	void RenderEffectPreset::setOutlineThickness( const float p_oulineThickness )
	{
		_oulineThickness = p_oulineThickness;
		_notifyDataChanged();
	};
	void RenderEffectPreset::setOutlineColor( const Color::Rgb & p_outlineColor )
	{
		_outlineColor = p_outlineColor;
		_notifyDataChanged();
	};

	void RenderEffectPreset::enableFog( const bool p_enable )
	{
		_fog = p_enable;
		_notifyDataChanged();
	}
	void RenderEffectPreset::setFogNear( const float p_fogNear )
	{
		_fogNear = p_fogNear <= _fogFar ? p_fogNear : _fogFar;
		_notifyDataChanged();
	};
	void RenderEffectPreset::setFogFar( const float p_fogFar )
	{
		_fogFar = p_fogFar > _fogNear ? p_fogFar : _fogNear;
		_notifyDataChanged();
	};
	void RenderEffectPreset::setFogDensity( const float p_fogDensity )
	{
		_fogDensity = Util::Math::clamp( p_fogDensity, 0.0f, 1.0f );
		_notifyDataChanged();
	};
	void RenderEffectPreset::setFogColor( const Color::Rgb & p_fogColor )
	{
		_fogColor = p_fogColor;
		_notifyDataChanged();
	}

	void RenderEffectPreset::_notifyDataChanged() { BaseModel::_notifyDataChanged(); }

	void RenderEffectPreset::apply() const
	{
		VTX::Renderer::GL::GL & rendererGL = VTXApp::get().getMainWindow().getOpenGLWidget().getRendererGL();

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
	}

} // namespace VTX::Model::Renderer
