#ifndef __VTX_APP_ACTION_RENDER_SETTINGS__
#define __VTX_APP_ACTION_RENDER_SETTINGS__

#include "app/action/library.hpp"
#include "app/library/preset/render_settings.hpp"

namespace VTX::App::Action::RenderSettings
{
	template<VTX::Renderer::E_RENDER_SETTINGS S, typename T>
	class Change final : public Library::BaseActionPreset<App::Library::Preset::RenderSettings>
	{
	  public:
		Change( App::Library::Preset::RenderSettings * const p_preset, const T p_value ) :
			Library::BaseActionPreset<App::Library::Preset::RenderSettings>( p_preset ), _value( p_value )
		{
		}
		Change( const std::string_view p_preset, const T p_value ) :
			Library::BaseActionPreset<App::Library::Preset::RenderSettings>( p_preset ), _value( p_value )
		{
		}

		void execute() override { _preset->setValue<S>( _value ); }

	  private:
		const T _value;
	};

} // namespace VTX::App::Action::RenderSettings
#endif
