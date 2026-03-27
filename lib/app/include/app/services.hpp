#ifndef __VTX_APP_SERVICES__
#define __VTX_APP_SERVICES__

#include "app/ecs.hpp"

/**
 * @brief Forward declarations of services stored in the ECS context.
 */
namespace VTX::App
{
	class Args;
	namespace Action
	{
		class ActionManager;
	}
	namespace Input
	{
		class InputManager;
	}
	namespace Network
	{
		class NetworkManager;
	}
	namespace Pass
	{
		class PassManager;
	}
	namespace PythonBinding
	{
		class Interpretor;
	}
	class Session;
	namespace Threading
	{
		class ThreadManager;
	}
	namespace Uid
	{
		class UIDManager;
	}
	namespace PythonBinding
	{
		class Interpretor;
	}
} // namespace VTX::App
namespace VTX::Renderer
{
	class Renderer;
}
namespace VTX::Util
{
	namespace Monitoring
	{
		class Stats;
	}
	class EventHub;
} // namespace VTX::Util

namespace VTX::App
{
	/**
	 * @brief Arguments launching the application with.
	 */
	inline Args & ARGS() { return ECS::getCtx<Args>(); }
	/**
	 * @brief Main event bus.
	 */
	inline Util::EventHub & HUB() { return ECS::getCtx<Util::EventHub>(); }
	/**
	 * @brief Statistics.
	 */
	inline Util::Monitoring::Stats & STATS() { return ECS::getCtx<Util::Monitoring::Stats>(); }
	/**
	 * @brief Renderer facade.
	 */
	inline Renderer::Renderer & RENDERER() { return ECS::getCtx<Renderer::Renderer>(); }
	/**
	 * @brief Action manager.
	 */
	inline Action::ActionManager & ACTION() { return ECS::getCtx<Action::ActionManager>(); }
	/**
	 * @brief Input manager.
	 */
	inline Input::InputManager & INPUT() { return ECS::getCtx<Input::InputManager>(); }
	/**
	 * @brief Network manager.
	 */
	inline Network::NetworkManager & NETWORK() { return ECS::getCtx<Network::NetworkManager>(); }
	/**
	 * @brief Runtime session.
	 */
	inline Session & SESSION() { return ECS::getCtx<Session>(); }
	/**
	 * @brief Thread manager.
	 */
	inline Threading::ThreadManager & THREAD() { return ECS::getCtx<Threading::ThreadManager>(); }
	/**
	 * @brief UID manager.
	 */
	inline Uid::UIDManager & UID() { return ECS::getCtx<Uid::UIDManager>(); }
	/**
	 * @brief Pass manager.
	 */
	inline Pass::PassManager & PASS() { return ECS::getCtx<Pass::PassManager>(); }
	/**
	 * @brief Python interpretor.
	 */
	inline PythonBinding::Interpretor & INTERPRETOR() { return ECS::getCtx<PythonBinding::Interpretor>(); }
} // namespace VTX::App

#endif
