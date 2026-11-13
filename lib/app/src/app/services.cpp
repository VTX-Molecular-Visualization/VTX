#include "app/services.hpp"
#include "app/ecs.hpp"
#include <string>
#include <util/exceptions.hpp>

namespace VTX::App
{
	Util::ActionManager & ACTION() { return ECS::getCtx<Util::ActionManager>(); }

	const Arguments & ARGS() { return ECS::getCtx<Arguments>(); }

	Util::EventHub & HUB() { return ECS::getCtx<Util::EventHub>(); }

	Input::InputManager & INPUT() { return ECS::getCtx<Input::InputManager>(); }

	PythonBinding::Interpretor & INTERPRETOR()
	{
		if ( not ECS::hasCtx<PythonBinding::Interpretor>() )
		{
			throw VTXException( std::string( "Python interpretor is not available." ) );
		}
		return ECS::getCtx<PythonBinding::Interpretor>();
	}


	Pass::PassManager & PASS() { return ECS::getCtx<Pass::PassManager>(); }

	PendingUpdates & PENDING_UPDATES() { return ECS::getCtx<PendingUpdates>(); }

	Renderer::Renderer & RENDERER() { return ECS::getCtx<Renderer::Renderer>(); }

	Session & SESSION() { return ECS::getCtx<Session>(); }

	Util::Thread::ThreadManager & THREAD() { return ECS::getCtx<Util::Thread::ThreadManager>(); }

	Uid::UIDManager & UID() { return ECS::getCtx<Uid::UIDManager>(); }
} // namespace VTX::App
