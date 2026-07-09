#include "app/action/scene.hpp"
#include "app/helper/preset.hpp"
#include "app/helper/system.hpp"
#include "app/scene/color_layout.hpp"
#include "app/scene/graphics_config.hpp"
#include "app/scene/tag_root.hpp"
#include "app/system/selection.hpp"
#include <io/metadata.hpp>
#include <renderer/color.hpp>
#include <renderer/graphics_config.hpp>
#include <util/exceptions.hpp>

namespace VTX::App::Action::Scene
{

	void DeleteSystem::execute( const Entity p_e ) { REG().destroy( p_e ); }

	void DeleteSystemSelected::execute()
	{
		REG().view<System::Selection>().each(
			[ & ]( auto p_e, auto & )
			{
				if ( Helper::System::getSelectionState( { p_e } ) == System::E_SELECTION_STATE::FULL )
				{
					REG().destroy( p_e );
				}
			}
		);
	}

	void Clear::execute()
	{
		REG().view<IO::Metadata>().each( [ & ]( auto p_e, auto & ) { REG().destroy( p_e ); } );
	}

	void SetColorLayout::execute( const Entity p_e )
	{
		using CL = App::Scene::ColorLayout;

		auto & reg	 = REG();
		auto   scene = ECS::getFirstEntityOnlyWithComponents<App::Scene::TagRoot>();
		auto   view	 = reg.view<CL>();

		assert( view.size() <= 1 );

		reg.remove<CL>( scene );
		reg.emplace<CL>( scene, p_e );
	}

	void SetColorLayout::execute( const std::string_view p_name )
	{
		const std::optional<Entity> preset = Helper::Preset::getByName<Renderer::Color::Layout>( p_name );
		if ( not preset )
		{
			throw VTXException( "Color layout preset not found: " + std::string( p_name ) );
		}
		execute( *preset );
	}

	void SetGraphicsConfig::execute( const Entity p_e )
	{
		using GC = App::Scene::GraphicsConfig;

		auto & reg	 = REG();
		auto   scene = ECS::getFirstEntityOnlyWithComponents<App::Scene::TagRoot>();
		auto   view	 = reg.view<GC>();

		assert( view.size() <= 1 );

		reg.remove<GC>( scene );
		reg.emplace<GC>( scene, p_e );
	}

	void SetGraphicsConfig::execute( const std::string_view p_name )
	{
		const std::optional<Entity> preset = Helper::Preset::getByName<Renderer::GraphicsConfig>( p_name );
		if ( not preset )
		{
			throw VTXException( "Graphics config preset not found: " + std::string( p_name ) );
		}
		execute( *preset );
	}

} // namespace VTX::App::Action::Scene
