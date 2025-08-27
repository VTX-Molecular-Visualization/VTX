#ifndef __VTX_UI_ACTION_COLOR_LAYOUT__
#define __VTX_UI_ACTION_COLOR_LAYOUT__

#include "app/action/library.hpp"
#include "app/library/preset/color_layout.hpp"
#include <util/color/rgba.hpp>

namespace VTX::App::Action::ColorLayout
{
	class Change final : public App::Action::Library::BaseActionPreset<VTX::App::Library::Preset::ColorLayout>
	{
	  public:
		Change( const Index, const Util::Color::Rgba & );
		void execute() override;

	  private:
		const Index				  _index;
		const Util::Color::Rgba & _color;
	};

	class ChangeAll final : public App::Core::Action::BaseAction
	{
	  public:
		ChangeAll( const std::vector<Util::Color::Rgba> & p_colors );
		void execute() override;

	  private:
		const std::vector<Util::Color::Rgba> & _colors;
	};

	class Randomize final : public App::Core::Action::BaseAction
	{
	  public:
		Randomize() {}
		void execute() override;
	};

} // namespace VTX::App::Action::ColorLayout
#endif
