#ifndef VTX_UTIL_ECS_HPP
#define VTX_UTIL_ECS_HPP

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
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
