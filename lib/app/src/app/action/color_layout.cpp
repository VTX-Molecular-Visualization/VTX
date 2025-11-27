#include "app/action/color_layout.hpp"
#include "app/preset/instance.hpp"
#include "app/scene/tag_root.hpp"

namespace VTX::App::Action::ColorLayout
{

	void SetCurrent::execute( const ECS::Entity p_e )
	{
		auto & reg	 = REG();
		auto   scene = ECS::getFirstEntityOnlyWithComponents<Scene::TagRoot>();

		auto view = reg.view<Preset::Instance<Renderer::Color::Layout>>();

		reg.remove<Preset::Instance<Renderer::Color::Layout>>( scene );
		reg.emplace<Preset::Instance<Renderer::Color::Layout>>( scene, p_e );
	}

	void Change::execute( const ECS::Entity p_e, const Index p_index, const Util::Color::Rgba & p_color )
	{
		REG().patch<Renderer::Color::Layout>(
			p_e,
			[ p_index, p_color ]( Renderer::Color::Layout & p_layout )
			{
				assert( p_index >= 0 && p_index < Renderer::Color::COLOR_LAYOUT_SIZE );
				p_layout.colors[ p_index ] = p_color;
			}
		);
	}

	void ChangeAll::execute( const ECS::Entity p_e, const Renderer::Color::LayoutArray & p_colors )
	{
		REG().patch<Renderer::Color::Layout>(
			p_e, [ p_colors ]( Renderer::Color::Layout & p_layout ) { p_layout.colors = p_colors; }
		);
	}

	void Randomize::execute( const ECS::Entity p_e )
	{
		REG().patch<Renderer::Color::Layout>(
			p_e,
			[]( Renderer::Color::Layout & p_layout )
			{
				std::generate(
					p_layout.colors.begin(), p_layout.colors.end(), [] { return Util::Color::Rgba::random(); }
				);
			}
		);
	}

} // namespace VTX::App::Action::ColorLayout
