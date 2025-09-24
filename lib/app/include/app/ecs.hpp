#ifndef __VTX_APP_ECS__
#define __VTX_APP_ECS__

#include <entt/entt.hpp>

namespace VTX::App::ECS
{
	using Registry = entt::registry;
	using Entity   = entt::entity;

	void	   setRegistry( Registry & );
	Registry & registry() noexcept;
} // namespace VTX::App::ECS

namespace VTX::App
{
	inline ECS::Registry & REG() { return ECS::registry(); }
} // namespace VTX::App

#endif
