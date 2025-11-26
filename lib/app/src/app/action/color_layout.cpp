#include "app/action/color_layout.hpp"

namespace
{
	VTX::App::Library::Preset::ColorLayout & _getColorLayoutPreset( const std::string_view p_preset )
	{
		return VTX::App::LIBRARY().getLibrary<VTX::App::Library::Preset::ColorLayout>().getPreset( p_preset );
	}
} // namespace

namespace VTX::App::Action::ColorLayout
{

	void SetCurrent::execute( const std::string_view p_preset )
	{
		// TODO: dangerous
		/*
		ECS_REGISTRY().removeComponent<Component::Representation::ColorLayout>( ECS_REGISTRY().getEntity( SCENE() ) );
		auto & comp = ECS_REGISTRY().addComponent<Component::Representation::ColorLayout>(
			ECS_REGISTRY().getEntity( SCENE() ), *_preset
		);
		comp.setupProxy();
		*/
	}

	void Change::execute( const std::string_view p_preset, const Index p_index, const Util::Color::Rgba & p_color )
	{
		_getColorLayoutPreset( p_preset ).setColor( p_index, p_color );
	}

	void ChangeAll::execute( const std::string_view p_preset, const Renderer::Color::LayoutArray & p_colors )
	{
		_getColorLayoutPreset( p_preset ).setColors( p_colors );
	}

	void Randomize::execute( const std::string_view p_preset )
	{
		Renderer::Color::LayoutArray randomColors;
		std::generate( randomColors.begin(), randomColors.end(), [] { return Util::Color::Rgba::random(); } );
		_getColorLayoutPreset( p_preset ).setColors( randomColors );
	}

} // namespace VTX::App::Action::ColorLayout
