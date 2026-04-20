#include "app/action/scene.hpp"
#include "app/helper/system.hpp"
#include "app/scene/color_layout.hpp"
#include "app/scene/graphics_config.hpp"
#include "app/scene/tag_root.hpp"
#include "app/system/metadata.hpp"
#include "app/system/selection.hpp"

namespace VTX::App::Action::Scene
{

	void DeleteSystem::execute( const ECS::Entity p_e ) { REG().destroy( p_e ); }

	void DeleteSystemSelected::execute()
	{
		REG().view<System::Metadata, System::Selection>().each(
			[ & ]( auto p_e, auto &, auto & )
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
		REG().view<System::Metadata>().each( [ & ]( auto p_e, auto & ) { REG().destroy( p_e ); } );
	}

	void SetColorLayout::execute( const ECS::Entity p_e )
	{
		using CL = App::Scene::ColorLayout;

		auto & reg	 = REG();
		auto   scene = ECS::getFirstEntityOnlyWithComponents<App::Scene::TagRoot>();
		auto   view	 = reg.view<CL>();

		assert( view.size() <= 1 );

		reg.remove<CL>( scene );
		reg.emplace<CL>( scene, p_e );
	}

	void SetGraphicsConfig::execute( const ECS::Entity p_e )
	{
		using GC = App::Scene::GraphicsConfig;

		auto & reg	 = REG();
		auto   scene = ECS::getFirstEntityOnlyWithComponents<App::Scene::TagRoot>();
		auto   view	 = reg.view<GC>();

		assert( view.size() <= 1 );

		reg.remove<GC>( scene );
		reg.emplace<GC>( scene, p_e );
	}

} // namespace VTX::App::Action::Scene
