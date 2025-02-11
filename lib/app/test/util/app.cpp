#include "app.hpp"
#include <app/action/scene.hpp>
#include <app/application/scene.hpp>
#include <app/component/render/camera.hpp>
#include <app/core/renderer/renderer_system.hpp>
#include <app/filesystem.hpp>
#include <app/vtx_app.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace VTX::App::Test::Util
{

	void App::loadSystem( const std::string & p_systemPath )
	{
		// Create System
		const FilePath				systemPath	   = VTX::App::Filesystem::getInternalDataDir() / p_systemPath;
		Action::Scene::LoadSystem loadSystemAction = Action::Scene::LoadSystem( systemPath );
		loadSystemAction.execute();
	}

	void App::loadTestSystem() { loadSystem( MOLECULE_TEST_NAME_EXT ); }
	void App::loadTestTrajectorySystem() { loadSystem( MOLECULE_TRAJECTORY_TEST_NAME_EXT ); }

	bool App::checkItemOrder(
		const VTX::App::Application::Scene &	   p_scene,
		const std::vector<Core::ECS::BaseEntity> & p_entities
	)
	{
		if ( p_entities.size() >= 2 )
		{
			size_t prevIndex = p_scene.getItemIndex( p_entities[ 0 ] );
			for ( size_t i = 1; i < p_entities.size(); i++ )
			{
				const size_t index = p_scene.getItemIndex( p_entities[ i ] );

				if ( index <= prevIndex )
				{
					return false;
				}
			}
		}
		return true;
	}

} // namespace VTX::App::Test::Util
