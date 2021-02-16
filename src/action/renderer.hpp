#ifndef __VTX_ACTION_RENDERER__
#define __VTX_ACTION_RENDERER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "color/rgb.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "renderer/base_renderer.hpp"
#include "vtx_app.hpp"

namespace VTX::Action::Renderer
{
	class ApplyRenderEffectPreset : public BaseAction
	{
	  public:
		ApplyRenderEffectPreset( const Model::Renderer::RenderEffectPreset & p_preset ) : _preset( p_preset ) {};
		virtual void execute() override
		{
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		};

	  private:
		const Model::Renderer::RenderEffectPreset & _preset;
	};

	class ChangeShading : public BaseAction
	{
	  public:
		ChangeShading( Model::Renderer::RenderEffectPreset & p_preset, const VTX::Renderer::SHADING & p_shading ) :
			_preset( p_preset ), _shading( p_shading ) {};

		virtual void execute() override
		{
			_preset.setShading( _shading );
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const VTX::Renderer::SHADING		  _shading;
	};

	class EnableSSAO : public BaseAction
	{
	  public:
		EnableSSAO( Model::Renderer::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override
		{
			_preset.enableSSAO( _enable );
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _enable;
	};
	class ChangeSSAOIntensity : public BaseAction
	{
	  public:
		ChangeSSAOIntensity( Model::Renderer::RenderEffectPreset & p_preset, const int p_intensity ) :
			_preset( p_preset ), _intensity( p_intensity ) {};

		virtual void execute() override
		{
			_preset.setSSAOIntensity( _intensity );
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const int							  _intensity;
	};
	class ChangeSSAOBlurSize : public BaseAction
	{
	  public:
		ChangeSSAOBlurSize( Model::Renderer::RenderEffectPreset & p_preset, const int p_blurSize ) :
			_preset( p_preset ), _blurSize( p_blurSize ) {};

		virtual void execute() override
		{
			_preset.setSSAOBlurSize( _blurSize );
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const int							  _blurSize;
	};

	class EnableOutline : public BaseAction
	{
	  public:
		EnableOutline( Model::Renderer::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override
		{
			_preset.enableOutline( _enable );
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _enable;
	};
	class ChangeOutlineThickness : public BaseAction
	{
	  public:
		ChangeOutlineThickness( Model::Renderer::RenderEffectPreset & p_preset, const float p_thickness ) :
			_preset( p_preset ), _thickness( p_thickness ) {};

		virtual void execute() override
		{
			_preset.setOutlineThickness( _thickness );
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _thickness;
	};
	class ChangeOutlineColor : public BaseAction
	{
	  public:
		ChangeOutlineColor( Model::Renderer::RenderEffectPreset & p_preset, const Color::Rgb & p_color ) :
			_preset( p_preset ), _color( p_color ) {};

		virtual void execute() override
		{
			_preset.setOutlineColor( _color );
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const Color::Rgb					  _color;
	};

	class EnableFog : public BaseAction
	{
	  public:
		EnableFog( Model::Renderer::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override
		{
			_preset.enableFog( _enable );
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _enable;
	};
	class ChangeFogNear : public BaseAction
	{
	  public:
		ChangeFogNear( Model::Renderer::RenderEffectPreset & p_preset, const float p_near ) :
			_preset( p_preset ), _near( p_near ) {};

		virtual void execute() override
		{
			_preset.setFogNear( _near );
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _near;
	};
	class ChangeFogFar : public BaseAction
	{
	  public:
		ChangeFogFar( Model::Renderer::RenderEffectPreset & p_preset, const float p_far ) :
			_preset( p_preset ), _far( p_far ) {};

		virtual void execute() override
		{
			_preset.setFogFar( _far );
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _far;
	};
	class ChangeFogDensity : public BaseAction
	{
	  public:
		ChangeFogDensity( Model::Renderer::RenderEffectPreset & p_preset, const float p_density ) :
			_preset( p_preset ), _density( p_density ) {};

		virtual void execute() override
		{
			_preset.setFogDensity( _density );
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _density;
	};
	class ChangeFogColor : public BaseAction
	{
	  public:
		ChangeFogColor( Model::Renderer::RenderEffectPreset & p_preset, const Color::Rgb & p_color ) :
			_preset( p_preset ), _color( p_color ) {};

		virtual void execute() override
		{
			_preset.setFogColor( _color );
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const Color::Rgb					  _color;
	};

	class ChangeBackgroundColor : public BaseAction
	{
	  public:
		ChangeBackgroundColor( Model::Renderer::RenderEffectPreset & p_preset, const Color::Rgb & p_color ) :
			_preset( p_preset ), _color( p_color ) {};

		virtual void execute() override
		{
			_preset.setBackgroundColor( _color );
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const Color::Rgb					  _color;
	};
	class ChangeCameraLightColor : public BaseAction
	{
	  public:
		ChangeCameraLightColor( Model::Renderer::RenderEffectPreset & p_preset, const Color::Rgb & p_color ) :
			_preset( p_preset ), _color( p_color ) {};

		virtual void execute() override
		{
			_preset.setCameraLightColor( _color );
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const Color::Rgb					  _color;
	};
	class ChangeCameraFOV : public BaseAction
	{
	  public:
		ChangeCameraFOV( Model::Renderer::RenderEffectPreset & p_preset, const float p_fov ) :
			_preset( p_preset ), _fov( p_fov ) {};

		virtual void execute() override
		{
			_preset.setCameraFOV( _fov );
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_CAMERA_UPDATED;
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _fov;
	};
	class ChangeCameraNear : public BaseAction
	{
	  public:
		ChangeCameraNear( Model::Renderer::RenderEffectPreset & p_preset, const float p_near ) :
			_preset( p_preset ), _near( p_near ) {};

		virtual void execute() override
		{
			_preset.setCameraNearClip( _near );
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_CAMERA_UPDATED;
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _near;
	};
	class ChangeCameraFar : public BaseAction
	{
	  public:
		ChangeCameraFar( Model::Renderer::RenderEffectPreset & p_preset, const float p_far ) :
			_preset( p_preset ), _far( p_far ) {};

		virtual void execute() override
		{
			_preset.setCameraFarClip( _far );
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_CAMERA_UPDATED;
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _far;
	};

	class EnableCameraAntialiasing : public BaseAction
	{
	  public:
		EnableCameraAntialiasing( Model::Renderer::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override
		{
			_preset.setAA( _enable );
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _enable;
	};
	class ChangeCameraPerspective : public BaseAction
	{
	  public:
		ChangeCameraPerspective( Model::Renderer::RenderEffectPreset & p_preset, const bool p_perspective ) :
			_preset( p_preset ), _perspective( p_perspective ) {};

		virtual void execute() override
		{
			_preset.setPerspectiveProjection( _perspective );
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _perspective;
	};
} // namespace VTX::Action::Renderer
#endif
