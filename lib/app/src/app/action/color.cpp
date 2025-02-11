#include "app/action/color.hpp"
#include <app/application/scene.hpp>
#include <app/component/representation/color_layout.hpp>

namespace VTX::App::Action::Color
{

	ChangeLayoutColor::ChangeLayoutColor( const size_t p_index, const Util::Color::Rgba & p_color ) :
		_index( p_index ), _color( p_color )
	{
	}

	void ChangeLayoutColor::execute()
	{
		const auto & scene	   = App::SCENE();
		auto &		 component = App::ECS_REGISTRY().getComponent<App::Component::Representation::ColorLayout>(
			  App::ECS_REGISTRY().getEntity( scene )
		  );
		component.setColor( _index, _color );
	}

	ChangeLayoutColors::ChangeLayoutColors( const std::vector<Util::Color::Rgba> & p_colors ) : _colors( p_colors ) {}

	void ChangeLayoutColors::execute()
	{
		const auto & scene	   = App::SCENE();
		auto &		 component = App::ECS_REGISTRY().getComponent<App::Component::Representation::ColorLayout>(
			  App::ECS_REGISTRY().getEntity( scene )
		  );
		component.setColors( _colors );
	}

	void RandomizeLayoutColors::execute()
	{
		std::vector<Util::Color::Rgba> randomColors( VTX::Core::Struct::ColorLayout::LAYOUT_SIZE );
		std::generate( randomColors.begin(), randomColors.end(), [] { return Util::Color::Rgba::random(); } );
		const auto & scene	   = App::SCENE();
		auto &		 component = App::ECS_REGISTRY().getComponent<App::Component::Representation::ColorLayout>(
			  App::ECS_REGISTRY().getEntity( scene )
		  );
		component.setColors( randomColors );
	}

} // namespace VTX::App::Action::Color
