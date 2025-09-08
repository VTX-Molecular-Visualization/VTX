#ifndef __VTX_UI_ACTION_COLOR_LAYOUT__
#define __VTX_UI_ACTION_COLOR_LAYOUT__

#include "app/action/library.hpp"
#include "app/library/preset/color_layout.hpp"
#include <util/color/rgba.hpp>

namespace VTX::App::Action::ColorLayout
{
	class Change final : public App::Action::Library::BaseActionPreset<App::Library::Preset::ColorLayout>
	{
	  public:
		Change( App::Library::Preset::ColorLayout * const, const Index, const Util::Color::Rgba & );
		Change( const std::string_view, const Index, const Util::Color::Rgba & );
		void execute() override;

	  private:
		const Index				  _index;
		const Util::Color::Rgba & _color;
	};

	class ChangeAll final : public App::Action::Library::BaseActionPreset<App::Library::Preset::ColorLayout>
	{
	  public:
		ChangeAll( App::Library::Preset::ColorLayout * const, const VTX::Core::Struct::ColorLayoutArray & p_colors );
		ChangeAll( const std::string_view, const VTX::Core::Struct::ColorLayoutArray & p_colors );
		void execute() override;

	  private:
		const VTX::Core::Struct::ColorLayoutArray & _colors;
	};

	class Randomize final : public App::Action::Library::BaseActionPreset<App::Library::Preset::ColorLayout>
	{
	  public:
		Randomize( App::Library::Preset::ColorLayout * const );
		Randomize( const std::string_view );

		void execute() override;
	};

} // namespace VTX::App::Action::ColorLayout
#endif
