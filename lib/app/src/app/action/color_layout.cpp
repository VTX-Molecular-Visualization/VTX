#include "app/action/color_layout.hpp"
#include "app/component/representation/color_layout.hpp"
#include "app/core/ecs/registry.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Action::ColorLayout
{
	SetCurrent::SetCurrent( App::Library::Preset::ColorLayout * const p_preset ) :
		App::Action::Library::BaseActionPreset<App::Library::Preset::ColorLayout>( p_preset )
	{
	}
	SetCurrent::SetCurrent( const std::string_view p_preset ) :
		App::Action::Library::BaseActionPreset<App::Library::Preset::ColorLayout>( p_preset )
	{
	}

	void SetCurrent::execute()
	{
		// auto & c = App::ECS_REGISTRY().getComponent<Component::Representation::ColorLayout>();
	}

	Change::Change(
		App::Library::Preset::ColorLayout * const p_preset,
		const Index								  p_index,
		const Util::Color::Rgba &				  p_color
	) :
		App::Action::Library::BaseActionPreset<App::Library::Preset::ColorLayout>( p_preset ), _index( p_index ),
		_color( p_color )
	{
	}

	Change::Change( const std::string_view p_preset, const Index p_index, const Util::Color::Rgba & p_color ) :
		App::Action::Library::BaseActionPreset<App::Library::Preset::ColorLayout>( p_preset ), _index( p_index ),
		_color( p_color )
	{
	}

	void Change::execute() { _preset->setColor( _index, _color ); }

	ChangeAll::ChangeAll(
		App::Library::Preset::ColorLayout * const	p_preset,
		const VTX::Core::Struct::ColorLayoutArray & p_colors
	) : App::Action::Library::BaseActionPreset<App::Library::Preset::ColorLayout>( p_preset ), _colors( p_colors )
	{
	}

	ChangeAll::ChangeAll( const std::string_view p_preset, const VTX::Core::Struct::ColorLayoutArray & p_colors ) :
		App::Action::Library::BaseActionPreset<App::Library::Preset::ColorLayout>( p_preset ), _colors( p_colors )
	{
	}

	void ChangeAll::execute() { _preset->setColors( _colors ); }

	Randomize::Randomize( App::Library::Preset::ColorLayout * const p_preset ) :
		App::Action::Library::BaseActionPreset<App::Library::Preset::ColorLayout>( p_preset )
	{
	}

	Randomize::Randomize( const std::string_view p_preset ) :
		App::Action::Library::BaseActionPreset<App::Library::Preset::ColorLayout>( p_preset )
	{
	}
	void Randomize::execute()
	{
		VTX::Core::Struct::ColorLayoutArray randomColors;
		std::generate( randomColors.begin(), randomColors.end(), [] { return Util::Color::Rgba::random(); } );
		_preset->setColors( randomColors );
	}

} // namespace VTX::App::Action::ColorLayout
