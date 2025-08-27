#include "app/action/color_layout.hpp"

namespace VTX::App::Action::ColorLayout
{

	Change::Change( const Index p_index, const Util::Color::Rgba & p_color ) : _index( p_index ), _color( p_color ) {}

	void Change::execute()
	{
		const auto & scene	   = App::SCENE();
		auto &		 component = App::ECS_REGISTRY().getComponent<App::Component::Representation::ColorLayout>(
			  App::ECS_REGISTRY().getEntity( scene )
		  );
		component.setColor( _index, _color );
	}

	ChangeAll::ChangeAll( const std::vector<Util::Color::Rgba> & p_colors ) : _colors( p_colors ) {}

	void ChangeAll::execute()
	{
		const auto & scene	   = App::SCENE();
		auto &		 component = App::ECS_REGISTRY().getComponent<App::Component::Representation::ColorLayout>(
			  App::ECS_REGISTRY().getEntity( scene )
		  );
		component.setColors( _colors );
	}

	void Randomize::execute()
	{
		std::vector<Util::Color::Rgba> randomColors( VTX::Core::Struct::ColorLayout::LAYOUT_SIZE );
		std::generate( randomColors.begin(), randomColors.end(), [] { return Util::Color::Rgba::random(); } );
		const auto & scene	   = App::SCENE();
		auto &		 component = App::ECS_REGISTRY().getComponent<App::Component::Representation::ColorLayout>(
			  App::ECS_REGISTRY().getEntity( scene )
		  );
		component.setColors( randomColors );
	}

} // namespace VTX::App::Action::ColorLayout
