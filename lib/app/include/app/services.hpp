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
}
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
} // namespace VTX::App

#endif
