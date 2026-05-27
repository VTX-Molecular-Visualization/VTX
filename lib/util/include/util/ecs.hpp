#ifndef __VTX_UTIL_ECS__
#define __VTX_UTIL_ECS__

#include <entt/entt.hpp>
#include <util/hashing.hpp>
#include <util/logger.hpp>

namespace VTX
{ /**
   * @brief Aliases.
   */
	using Registry				   = entt::registry;
	using Entity				   = entt::entity;
	constexpr Entity InvalidEntity = entt::null;
} // namespace VTX

#endif
