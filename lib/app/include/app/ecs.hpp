#ifndef __VTX_APP_ECS__
#define __VTX_APP_ECS__

#include <util/ecs.hpp>

namespace VTX::App::ECS
{
	/**
	 * @brief Set the global registry.
	 */
	void setRegistry( Registry & );

	/**
	 * @brief Get the global registry.
	 */
	Registry & registry() noexcept;

} // namespace VTX::App::ECS

namespace VTX::App
{
	inline Registry & REG() { return Registry(); }

} // namespace VTX::App

#endif
