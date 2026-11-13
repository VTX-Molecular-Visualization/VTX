#include "app/python_binding/scene.hpp"
#include "app/action/scene.hpp"

namespace VTX::App::PythonBinding
{
	void SceneBinder::bind( Module & p_vtxModule )
	{
		p_vtxModule.bindAction<App::Action::Scene::Clear>( "newScene", "Clear scene." );
		p_vtxModule.bindAction<App::Action::Scene::DeleteSystemSelected>(
			"deleteSystemSelected", "Delete all selected systems from the scene."
		);
	}
} // namespace VTX::App::PythonBinding
