#include "app/services.hpp"
#include "app/ecs.hpp"

namespace VTX::App
{
	const Arguments & ARGS() { return ECS::getCtx<Arguments>(); }

	Util::EventHub & HUB() { return ECS::getCtx<Util::EventHub>(); }

	Util::Monitoring::Stats & STATS() { return ECS::getCtx<Util::Monitoring::Stats>(); }

	Renderer::Renderer & RENDERER() { return ECS::getCtx<Renderer::Renderer>(); }

	Action::ActionManager & ACTION() { return ECS::getCtx<Action::ActionManager>(); }

	Input::InputManager & INPUT() { return ECS::getCtx<Input::InputManager>(); }

	Network::NetworkManager & NETWORK() { return ECS::getCtx<Network::NetworkManager>(); }

	Session & SESSION() { return ECS::getCtx<Session>(); }

	Util::Thread::ThreadManager & THREAD() { return ECS::getCtx<Util::Thread::ThreadManager>(); }

	Uid::UIDManager & UID() { return ECS::getCtx<Uid::UIDManager>(); }

	Pass::PassManager & PASS() { return ECS::getCtx<Pass::PassManager>(); }

	PythonBinding::Interpretor & INTERPRETOR() { return ECS::getCtx<PythonBinding::Interpretor>(); }
} // namespace VTX::App
