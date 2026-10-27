#ifndef VTX_APP_SERVICES_HPP
#define VTX_APP_SERVICES_HPP

/**
 * @brief Forward declarations of services stored in the ECS context.
 */
namespace VTX::App
{
	struct Arguments;

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
	namespace Thread
	{
		class ThreadManager;
	}

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
	const Arguments & ARGS();

	/**
	 * @brief Main event bus.
	 */
	Util::EventHub & HUB();

	/**
	 * @brief Statistics.
	 */
	Util::Monitoring::Stats & STATS();

	/**
	 * @brief Renderer facade.
	 */
	Renderer::Renderer & RENDERER();

	/**
	 * @brief Action manager.
	 */
	Action::ActionManager & ACTION();

	/**
	 * @brief Input manager.
	 */
	Input::InputManager & INPUT();

	/**
	 * @brief Network manager.
	 */
	Network::NetworkManager & NETWORK();

	/**
	 * @brief Runtime session.
	 */
	Session & SESSION();

	/**
	 * @brief Thread manager.
	 */
	Util::Thread::ThreadManager & THREAD();

	/**
	 * @brief UID manager.
	 */
	Uid::UIDManager & UID();

	/**
	 * @brief Pass manager.
	 */
	Pass::PassManager & PASS();

	/**
	 * @brief Python interpretor.
	 */
	PythonBinding::Interpretor & INTERPRETOR();
} // namespace VTX::App

#endif
