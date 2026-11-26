#ifndef VTX_APP_SERVICES_HPP
#define VTX_APP_SERVICES_HPP

/**
 * @brief Forward declarations of services stored in the ECS context.
 */
namespace VTX::App
{
	struct Arguments;

	namespace Input
	{
		class InputManager;
	}

	namespace Pass
	{
		class PassManager;
	}

	class PendingUpdates;

	namespace PythonBinding
	{
		class Interpretor;
	}

	class Session;

	namespace Uid
	{
		class UIDManager;
	}
} // namespace VTX::App

namespace VTX::Renderer
{
	class Renderer;
}

namespace VTX::Util
{
	class ActionManager;
	class EventHub;

	namespace Thread
	{
		class ThreadManager;
	}
} // namespace VTX::Util

namespace VTX::App
{
	/**
	 * @brief Action manager.
	 */
	Util::ActionManager & ACTION();

	/**
	 * @brief Arguments launching the application with.
	 */
	const Arguments & ARGS();

	/**
	 * @brief Main event bus.
	 */
	Util::EventHub & HUB();

	/**
	 * @brief Input manager.
	 */
	Input::InputManager & INPUT();

	/**
	 * @brief Python interpretor.
	 */
	PythonBinding::Interpretor & INTERPRETOR();

	/**
	 * @brief Pass manager.
	 */
	Pass::PassManager & PASS();

	PendingUpdates & PENDING_UPDATES();

	/**
	 * @brief Renderer facade.
	 */
	Renderer::Renderer & RENDERER();

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

} // namespace VTX::App

#endif
