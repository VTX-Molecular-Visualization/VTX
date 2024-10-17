#ifndef __VTX_UI_ACTION_COLOR__
#define __VTX_UI_ACTION_COLOR__

#include "app/core/action/base_action.hpp"
#include <util/color/rgba.hpp>

namespace VTX::App::Action::Color
{
	class ChangeLayoutColor final : public App::Core::Action::BaseAction
	{
	  public:
		ChangeLayoutColor( const size_t, const Util::Color::Rgba & );
		void execute() override;

	  private:
		const size_t			  _index;
		const Util::Color::Rgba & _color;
	};

	class ChangeLayoutColors final : public App::Core::Action::BaseAction
	{
	  public:
		ChangeLayoutColors( const std::vector<Util::Color::Rgba> & p_colors );
		void execute() override;

	  private:
		const std::vector<Util::Color::Rgba> & _colors;
	};

	class RandomizeLayoutColors final : public App::Core::Action::BaseAction
	{
	  public:
		RandomizeLayoutColors() {}
		void execute() override;
	};

} // namespace VTX::App::Action::Color
#endif
