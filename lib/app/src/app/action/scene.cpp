#include "app/action/scene.hpp"
#include "app/events.hpp"
#include "app/helper/system.hpp"
#include "app/scene/color_layout.hpp"
#include "app/scene/graphics_config.hpp"
#include "app/scene/material.hpp"
#include "app/scene/tag_root.hpp"
#include "app/services.hpp"
#include "app/system/selection.hpp"
#include <core/struct/topology.hpp>
#include <io/metadata.hpp>
#include <util/event_hub.hpp>
#include <vector>

namespace VTX::App::Action::Scene
{
	namespace
	{
		void destroySystems( const std::vector<Entity> & p_systems )
		{
			for ( const Entity system : p_systems )
			{
				REG().destroy( system );
			}

			for ( const Entity system : p_systems )
			{
				HUB().trigger<Events::SystemDestroyed>( { system } );
			}
		}
	} // namespace

	void DeleteSystem::execute( const Entity p_e ) { destroySystems( { p_e } ); }

	void DeleteSystemSelected::execute()
	{
		std::vector<Entity> systems;
		REG().view<System::Selection>().each(
			[ & ]( auto p_e, auto & )
			{
				if ( Helper::System::getSelectionState( { p_e } ) == System::E_SELECTION_STATE::FULL )
				{
					systems.push_back( p_e );
				}
			}
		);
		destroySystems( systems );
	}

	void Clear::execute()
	{
		std::vector<Entity> systems;
		std::vector<Entity> others;
		REG().view<IO::Metadata>().each(
			[ & ]( auto p_e, auto & )
			{ ( REG().all_of<Core::Struct::Topology>( p_e ) ? systems : others ).push_back( p_e ); }
		);

		for ( const Entity entity : others )
		{
			REG().destroy( entity );
		}
		destroySystems( systems );
	}

	void SetColorLayout::execute( const Entity p_e )
	{
		using CL = App::Scene::ColorLayout;

		auto &				  reg	= REG();
		auto				  scene = ECS::getFirstEntityOnlyWithComponents<App::Scene::TagRoot>();
		[[maybe_unused]] auto view	= reg.view<CL>();

		assert( view.size() <= 1 );

		reg.remove<CL>( scene );
		reg.emplace<CL>( scene, p_e );
	}

	void SetGraphicsConfig::execute( const Entity p_e )
	{
		using GC = App::Scene::GraphicsConfig;

		auto &				  reg	= REG();
		auto				  scene = ECS::getFirstEntityOnlyWithComponents<App::Scene::TagRoot>();
		[[maybe_unused]] auto view	= reg.view<GC>();

		assert( view.size() <= 1 );

		reg.remove<GC>( scene );
		reg.emplace<GC>( scene, p_e );
	}

	void SetMaterial::execute( const Entity p_e )
	{
		using MA = App::Scene::Material;

		auto &				  reg	= REG();
		auto				  scene = ECS::getFirstEntityOnlyWithComponents<App::Scene::TagRoot>();
		[[maybe_unused]] auto view	= reg.view<MA>();

		assert( view.size() <= 1 );

		reg.remove<MA>( scene );
		reg.emplace<MA>( scene, p_e );
	}

} // namespace VTX::App::Action::Scene
