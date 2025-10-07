#ifndef __VTX_APP_SERVICES__
#define __VTX_APP_SERVICES__

#include "app/ecs.hpp"

/**
 * @brief Forward declarations of services stored in the ECS context.
 */
namespace VTX::Renderer
{
	class Facade;
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
	class Args;
	class PassManager;
	namespace Action
	{
		class ActionManager;
	}
	namespace Input
	{
		class InputManager;
	}
	namespace Library
	{
		class LibraryManager;
	}
	namespace Network
	{
		class NetworkManager;
	}
	namespace Settings
	{
		class SettingsManager;
	}
	namespace Threading
	{
		class ThreadManager;
	}
	namespace Uid
	{
		class UIDManager;
	}
} // namespace VTX::App
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
	inline Renderer::Facade & RENDERER() { return ECS::getCtx<Renderer::Facade>(); }
	/**
	 * @brief Action manager.
	 */
	inline Action::ActionManager & ACTION() { return ECS::getCtx<Action::ActionManager>(); }
	/**
	 * @brief Input manager.
	 */
	inline Input::InputManager & INPUT() { return ECS::getCtx<Input::InputManager>(); }
	/**
	 * @brief Library manager.
	 */
	inline Library::LibraryManager & LIBRARY() { return ECS::getCtx<Library::LibraryManager>(); }
	/**
	 * @brief Network manager.
	 */
	inline Network::NetworkManager & NETWORK() { return ECS::getCtx<Network::NetworkManager>(); }
	/**
	 * @brief Settings manager.
	 */
	inline Settings::SettingsManager & SETTINGS() { return ECS::getCtx<Settings::SettingsManager>(); }
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
	inline PassManager & PASS() { return ECS::getCtx<PassManager>(); }
} // namespace VTX::App

#endif
