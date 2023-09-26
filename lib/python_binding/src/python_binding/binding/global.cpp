#include "python_binding/binding/global.hpp"
#include <app/application/scene.hpp>
#include <app/internal/action/ecs.hpp>
#include <app/vtx_app.hpp>

namespace VTX::PythonBinding::Binding
{
	void Global::openFile( const std::string & p_path )
	{
		App::Internal::Action::ECS::Open openAction = App::Internal::Action::ECS::Open( p_path );
		openAction.execute();
	}
} // namespace VTX::PythonBinding::Binding
