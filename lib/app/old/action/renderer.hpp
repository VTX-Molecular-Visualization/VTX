#ifndef __VTX_APP_OLD_ACTION_RENDERER__
#define __VTX_APP_OLD_ACTION_RENDERER__

#include "app/old/action.hpp"
#include "app/old/application/render_effect/render_effect_library.hpp"
#include "app/old/application/render_effect/render_effect_preset.hpp"
#include "app/old/core/action/base_action.hpp"
#include <unordered_set>
#include <util/color/rgba.hpp>

namespace VTX::App::Old::Action::Renderer
{
	class ReloadPresets : public App::Old::Core::Action::BaseAction
	{
	  public:
		ReloadPresets() {};
		virtual void execute() override;
	};

	class ResetPresets : public App::Old::Core::Action::BaseAction
	{
	  public:
		ResetPresets() {};
		virtual void execute() override;
	};

	class SavePreset : public App::Old::Core::Action::BaseAction
	{
	  public:
		SavePreset( const App::Old::Application::RenderEffect::RenderEffectPreset & p_preset )
		{
			_renderEffectPresets.emplace( &p_preset );
		};
		SavePreset( const std::unordered_set<const App::Old::Application::RenderEffect::RenderEffectPreset *> & p_presets )
		{
			for ( const App::Old::Application::RenderEffect::RenderEffectPreset * const preset : p_presets )
				_renderEffectPresets.emplace( preset );
		};
		SavePreset( App::Old::Application::RenderEffect::RenderEffectLibrary & p_library )
		{
			for ( int i = 0; i < p_library.getPresetCount(); i++ )
				_renderEffectPresets.emplace( p_library.getPreset( i ) );
			_clearDirectory = true;
		};

		void setAsync( const bool p_async ) { _async = p_async; }

		virtual void execute() override;

	  private:
		std::unordered_set<const App::Old::Application::RenderEffect::RenderEffectPreset *> _renderEffectPresets
			= std::unordered_set<const App::Old::Application::RenderEffect::RenderEffectPreset *>();

		bool _clearDirectory = false;
		bool _async			 = true;
	};

	class ApplyRenderEffectPreset : public App::Old::Core::Action::BaseAction
	{
	  public:
		ApplyRenderEffectPreset( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset,
								 const bool											  p_setAsDefault = false ) :
			_preset( p_preset ),
			_setAsDefault( p_setAsDefault ) {};
		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const bool											 _setAsDefault;
	};

	class ChangeName : public App::Old::Core::Action::BaseAction
	{
	  public:
		ChangeName( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset, const std::string & p_name ) :
			_preset( p_preset ),
			_name( App::Old::Application::RenderEffect::RenderEffectLibrary::get().getValidName( p_name ) ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const std::string									 _name;
	};

	class ChangeQuickAccess : public App::Old::Core::Action::BaseAction
	{
	  public:
		ChangeQuickAccess( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset, const bool p_quickAccess ) :
			_preset( p_preset ), _quickAccess( p_quickAccess ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const bool											 _quickAccess;
	};

	class ChangeShading : public App::Old::Core::Action::BaseAction
	{
	  public:
		ChangeShading( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset,
					   const Render::Renderer::SHADING &					p_shading ) :
			_preset( p_preset ),
			_shading( p_shading ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const Render::Renderer::SHADING						 _shading;
	};

	class EnableSSAO : public App::Old::Core::Action::BaseAction
	{
	  public:
		EnableSSAO( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const bool											 _enable;
	};
	class ChangeSSAOIntensity : public App::Old::Core::Action::BaseAction
	{
	  public:
		ChangeSSAOIntensity( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset, const int p_intensity ) :
			_preset( p_preset ), _intensity( p_intensity ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const int											 _intensity;
	};
	class ChangeSSAOBlurSize : public App::Old::Core::Action::BaseAction
	{
	  public:
		ChangeSSAOBlurSize( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset, const int p_blurSize ) :
			_preset( p_preset ), _blurSize( p_blurSize ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const int											 _blurSize;
	};

	class EnableOutline : public App::Old::Core::Action::BaseAction
	{
	  public:
		EnableOutline( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const bool											 _enable;
	};
	class ChangeOutlineThickness : public App::Old::Core::Action::BaseAction
	{
	  public:
		ChangeOutlineThickness( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset,
								const uint											 p_thickness ) :
			_preset( p_preset ),
			_thickness( p_thickness ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const uint											 _thickness;
	};
	class ChangeOutlineSensivity : public App::Old::Core::Action::BaseAction
	{
	  public:
		ChangeOutlineSensivity( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset,
								const float											 p_sensivity ) :
			_preset( p_preset ),
			_sensivity( p_sensivity ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const float											 _sensivity;
	};
	class ChangeOutlineColor : public App::Old::Core::Action::BaseAction
	{
	  public:
		ChangeOutlineColor( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset,
							const Util::Color::Rgba &							 p_color ) :
			_preset( p_preset ),
			_color( p_color ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const Util::Color::Rgba								 _color;
	};

	class EnableFog : public App::Old::Core::Action::BaseAction
	{
	  public:
		EnableFog( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const bool											 _enable;
	};
	class ChangeFogNear : public App::Old::Core::Action::BaseAction
	{
	  public:
		ChangeFogNear( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset, const float p_near ) :
			_preset( p_preset ), _near( p_near ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const float											 _near;
	};
	class ChangeFogFar : public App::Old::Core::Action::BaseAction
	{
	  public:
		ChangeFogFar( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset, const float p_far ) :
			_preset( p_preset ), _far( p_far ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const float											 _far;
	};
	class ChangeFogDensity : public App::Old::Core::Action::BaseAction
	{
	  public:
		ChangeFogDensity( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset, const float p_density ) :
			_preset( p_preset ), _density( p_density ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const float											 _density;
	};
	class ChangeFogColor : public App::Old::Core::Action::BaseAction
	{
	  public:
		ChangeFogColor( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset,
						const Util::Color::Rgba &							 p_color ) :
			_preset( p_preset ),
			_color( p_color ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const Util::Color::Rgba								 _color;
	};

	class ChangeBackgroundColor : public App::Old::Core::Action::BaseAction
	{
	  public:
		ChangeBackgroundColor( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset,
							   const Util::Color::Rgba &							p_color ) :
			_preset( p_preset ),
			_color( p_color ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const Util::Color::Rgba								 _color;
	};

	class ChangeCameraLightColor : public App::Old::Core::Action::BaseAction
	{
	  public:
		ChangeCameraLightColor( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset,
								const Util::Color::Rgba &							 p_color ) :
			_preset( p_preset ),
			_color( p_color ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const Util::Color::Rgba								 _color;
	};

	class AddNewPresetInLibrary : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit AddNewPresetInLibrary( const std::string & p_presetName ) : _presetName( p_presetName ) {}

		virtual void execute() override;

		const std::string _presetName;
	};

	class CopyPresetInLibrary : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit CopyPresetInLibrary( const int p_presetIndex ) : _presetIndex( p_presetIndex ) {}

		virtual void execute() override;

	  private:
		const int _presetIndex;
	};

	class DeletePresetInLibrary : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit DeletePresetInLibrary( const int p_presetIndex ) : _presetIndex( p_presetIndex ) {}

		virtual void execute() override;

	  private:
		const int _presetIndex;
	};
} // namespace VTX::App::Old::Action::Renderer
#endif
