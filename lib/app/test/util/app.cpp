#include "app.hpp"
#include <app/action/scene.hpp>
#include <app/application/scene.hpp>
#include <app/application/system/renderer.hpp>
#include <app/component/render/camera.hpp>
#include <app/component/render/proxy_camera.hpp>
#include <app/component/render/proxy_color_layout.hpp>
#include <app/filesystem.hpp>
#include <app/vtx_app.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace VTX::App::Test::Util
{
	void App::initApp()
	{
		static bool isInit;
		if ( not isInit )
		{
			APP().init();
			APP().start( { 0, nullptr } );

			auto & renderer = RENDERER().facade();
			REQUIRE_THROWS( renderer.build() );

			/*
			// Camera.
			Component::Render::ProxyCamera & proxyCamera
				= MAIN_REGISTRY().getComponent<Component::Render::ProxyCamera>( SCENE().getCamera() );
			proxyCamera.setInRenderer( renderer );

			// Default representation.
			static Renderer::Proxy::Representation		   representation;
			std::vector<Renderer::Proxy::Representation *> representations { &representation };
			renderer.addProxyRepresentations( representations );

			// Default color layout.
			Component::Render::ProxyColorLayout & proxyColorLayout
				= MAIN_REGISTRY().findComponent<Component::Render::ProxyColorLayout>();
			proxyColorLayout.setup( renderer );
			renderer.setProxyColorLayout( proxyColorLayout.getProxy().proxy() );
			*/

			isInit = true;
		}

		SCENE().reset();
	}

	void App::loadMolecule( const std::string & p_moleculePath )
	{
		// Create MoleculeEntity
		const FilePath				moleculePath	   = VTX::App::Filesystem::getInternalDataDir() / p_moleculePath;
		Action::Scene::LoadMolecule loadMoleculeAction = Action::Scene::LoadMolecule( moleculePath );
		loadMoleculeAction.execute();
	}

	void App::loadTestMolecule() { loadMolecule( MOLECULE_TEST_NAME_EXT ); }
	void App::loadTestTrajectoryMolecule() { loadMolecule( MOLECULE_TRAJECTORY_TEST_NAME_EXT ); }

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
