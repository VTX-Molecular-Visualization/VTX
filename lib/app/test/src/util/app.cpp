#include "app.hpp"
#include <app/action/scene.hpp>
#include <app/application/scene.hpp>
#include <app/application/system/renderer.hpp>
#include <app/component/render/camera.hpp>
#include <app/component/render/proxy_camera.hpp>
#include <app/vtx_app.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/chemdb/color.hpp>
#include <io/internal/filesystem.hpp>
#include <string>

namespace VTX::App::Test::Util
{
	void App::initApp()
	{
		static bool isInit;
		if ( !isInit )
		{
			VTXApp::get().start( {} );

			auto & renderer = RENDERER_SYSTEM().facade();
			REQUIRE_THROWS( renderer.build() );

			// Camera.
			Component::Render::ProxyCamera & proxyCamera
				= MAIN_REGISTRY().getComponent<Component::Render::ProxyCamera>( SCENE().getCamera() );
			proxyCamera.setInRenderer( renderer );

			// Default representation.
			static Renderer::Proxy::Representation				representation;
			static std::vector<Renderer::Proxy::Representation> representations = { representation };
			renderer.setProxyRepresentations( representations );

			// Default color layout.
			static auto							JMOL			= VTX::Core::ChemDB::Color::COLOR_LAYOUT_JMOL;
			static Renderer::Proxy::ColorLayout jmolColorLayout = { &JMOL.layout };
			renderer.setProxyColorLayout( jmolColorLayout );

			isInit = true;
		}

		SCENE().reset();
	}

	void App::loadMolecule( const std::string & p_moleculePath )
	{
		// Create MoleculeEntity
		const FilePath				moleculePath = IO::Internal::Filesystem::getInternalDataDir() / p_moleculePath;
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
