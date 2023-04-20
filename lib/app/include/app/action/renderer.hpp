#ifndef __VTX_ACTION_RENDERER__
#define __VTX_ACTION_RENDERER__

#include "app/core/action/base_action.hpp"
#include "app/old_app/color/rgba.hpp"
#include "app/old_app/model/renderer/render_effect_preset.hpp"
#include "app/old_app/model/renderer/render_effect_preset_library.hpp"
#include <unordered_set>

namespace VTX::Action::Renderer
{
	class ReloadPresets : public Core::Action::BaseAction
	{
	  public:
		ReloadPresets() {};
		virtual void execute() override;
	};

	class ResetPresets : public Core::Action::BaseAction
	{
	  public:
		ResetPresets() {};
		virtual void execute() override;
	};

	class SavePreset : public Core::Action::BaseAction
	{
	  public:
		SavePreset( const Model::Renderer::RenderEffectPreset & p_preset )
		{
			_renderEffectPresets.emplace( &p_preset );
		};
		SavePreset( const std::unordered_set<const Model::Renderer::RenderEffectPreset *> & p_presets )
		{
			for ( const Model::Renderer::RenderEffectPreset * const preset : p_presets )
				_renderEffectPresets.emplace( preset );
		};
		SavePreset( Model::Renderer::RenderEffectPresetLibrary & p_library )
		{
			for ( int i = 0; i < p_library.getPresetCount(); i++ )
				_renderEffectPresets.emplace( p_library.getPreset( i ) );
			_clearDirectory = true;
		};

		void setAsync( const bool p_async ) { _async = p_async; }

		virtual void execute() override;

	  private:
		std::unordered_set<const Model::Renderer::RenderEffectPreset *> _renderEffectPresets
			= std::unordered_set<const Model::Renderer::RenderEffectPreset *>();

		bool _clearDirectory = false;
		bool _async			 = true;
	};

	class ApplyRenderEffectPreset : public Core::Action::BaseAction
	{
	  public:
		ApplyRenderEffectPreset( Model::Renderer::RenderEffectPreset & p_preset, const bool p_setAsDefault = false ) :
			_preset( p_preset ), _setAsDefault( p_setAsDefault ) {};
		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _setAsDefault;
	};

	class ChangeName : public Core::Action::BaseAction
	{
	  public:
		ChangeName( Model::Renderer::RenderEffectPreset & p_preset, const std::string & p_name ) :
			_preset( p_preset ), _name( Model::Renderer::RenderEffectPresetLibrary::get().getValidName( p_name ) ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const std::string					  _name;
	};

	class ChangeQuickAccess : public Core::Action::BaseAction
	{
	  public:
		ChangeQuickAccess( Model::Renderer::RenderEffectPreset & p_preset, const bool p_quickAccess ) :
			_preset( p_preset ), _quickAccess( p_quickAccess ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _quickAccess;
	};

	class ChangeShading : public Core::Action::BaseAction
	{
	  public:
		ChangeShading( Model::Renderer::RenderEffectPreset & p_preset, const VTX::Renderer::SHADING & p_shading ) :
			_preset( p_preset ), _shading( p_shading ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const VTX::Renderer::SHADING		  _shading;
	};

	class EnableSSAO : public Core::Action::BaseAction
	{
	  public:
		EnableSSAO( Model::Renderer::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _enable;
	};
	class ChangeSSAOIntensity : public Core::Action::BaseAction
	{
	  public:
		ChangeSSAOIntensity( Model::Renderer::RenderEffectPreset & p_preset, const int p_intensity ) :
			_preset( p_preset ), _intensity( p_intensity ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const int							  _intensity;
	};
	class ChangeSSAOBlurSize : public Core::Action::BaseAction
	{
	  public:
		ChangeSSAOBlurSize( Model::Renderer::RenderEffectPreset & p_preset, const int p_blurSize ) :
			_preset( p_preset ), _blurSize( p_blurSize ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const int							  _blurSize;
	};

	class EnableOutline : public Core::Action::BaseAction
	{
	  public:
		EnableOutline( Model::Renderer::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _enable;
	};
	class ChangeOutlineThickness : public Core::Action::BaseAction
	{
	  public:
		ChangeOutlineThickness( Model::Renderer::RenderEffectPreset & p_preset, const uint p_thickness ) :
			_preset( p_preset ), _thickness( p_thickness ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const uint							  _thickness;
	};
	class ChangeOutlineSensivity : public Core::Action::BaseAction
	{
	  public:
		ChangeOutlineSensivity( Model::Renderer::RenderEffectPreset & p_preset, const float p_sensivity ) :
			_preset( p_preset ), _sensivity( p_sensivity ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _sensivity;
	};
	class ChangeOutlineColor : public Core::Action::BaseAction
	{
	  public:
		ChangeOutlineColor( Model::Renderer::RenderEffectPreset & p_preset, const Color::Rgba & p_color ) :
			_preset( p_preset ), _color( p_color ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const Color::Rgba					  _color;
	};

	class EnableFog : public Core::Action::BaseAction
	{
	  public:
		EnableFog( Model::Renderer::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _enable;
	};
	class ChangeFogNear : public Core::Action::BaseAction
	{
	  public:
		ChangeFogNear( Model::Renderer::RenderEffectPreset & p_preset, const float p_near ) :
			_preset( p_preset ), _near( p_near ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _near;
	};
	class ChangeFogFar : public Core::Action::BaseAction
	{
	  public:
		ChangeFogFar( Model::Renderer::RenderEffectPreset & p_preset, const float p_far ) :
			_preset( p_preset ), _far( p_far ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _far;
	};
	class ChangeFogDensity : public Core::Action::BaseAction
	{
	  public:
		ChangeFogDensity( Model::Renderer::RenderEffectPreset & p_preset, const float p_density ) :
			_preset( p_preset ), _density( p_density ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const float							  _density;
	};
	class ChangeFogColor : public Core::Action::BaseAction
	{
	  public:
		ChangeFogColor( Model::Renderer::RenderEffectPreset & p_preset, const Color::Rgba & p_color ) :
			_preset( p_preset ), _color( p_color ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const Color::Rgba					  _color;
	};

	class ChangeBackgroundColor : public Core::Action::BaseAction
	{
	  public:
		ChangeBackgroundColor( Model::Renderer::RenderEffectPreset & p_preset, const Color::Rgba & p_color ) :
			_preset( p_preset ), _color( p_color ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const Color::Rgba					  _color;
	};

	class ChangeCameraLightColor : public Core::Action::BaseAction
	{
	  public:
		ChangeCameraLightColor( Model::Renderer::RenderEffectPreset & p_preset, const Color::Rgba & p_color ) :
			_preset( p_preset ), _color( p_color ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const Color::Rgba					  _color;
	};

	class AddNewPresetInLibrary : public Core::Action::BaseAction
	{
	  public:
		explicit AddNewPresetInLibrary( const std::string & p_presetName ) : _presetName( p_presetName ) {}

		virtual void execute() override;

		const std::string _presetName;
	};

	class CopyPresetInLibrary : public Core::Action::BaseAction
	{
	  public:
		explicit CopyPresetInLibrary( const int p_presetIndex ) : _presetIndex( p_presetIndex ) {}

		virtual void execute() override;

	  private:
		const int _presetIndex;
	};

	class DeletePresetInLibrary : public Core::Action::BaseAction
	{
	  public:
		explicit DeletePresetInLibrary( const int p_presetIndex ) : _presetIndex( p_presetIndex ) {}

		virtual void execute() override;

	  private:
		const int _presetIndex;
	};
} // namespace VTX::Action::Renderer
#endif
