#include "app/action/color_layout.hpp"
#include "app/scene/tag_root.hpp"
#include <algorithm>

namespace VTX::App::Action::ColorLayout
{

	void Change::execute( const Entity p_e, const Index p_index, const Util::Color::Rgba & p_color )
	{
		REG().patch<Renderer::Color::Layout>(
			p_e,
			[ p_index, p_color ]( Renderer::Color::Layout & p_layout )
			{
				assert( p_index >= 0 && p_index < Renderer::Color::COLOR_LAYOUT_SIZE );
				p_layout.colors[ p_index ] = p_color;
			} );
	}

	void ChangeAll::execute( const Entity p_e, const Renderer::Color::LayoutArray & p_colors )
	{
		REG().patch<Renderer::Color::Layout>(
			p_e, [ p_colors ]( Renderer::Color::Layout & p_layout ) { p_layout.colors = p_colors; } );
	}

	void Randomize::execute( const Entity p_e )
	{
		REG().patch<Renderer::Color::Layout>(
			p_e,
			[]( Renderer::Color::Layout & p_layout )
			{ std::ranges::generate( p_layout.colors, [] { return Util::Color::Rgba::random(); } ); } );
	}

} // namespace VTX::App::Action::ColorLayout
