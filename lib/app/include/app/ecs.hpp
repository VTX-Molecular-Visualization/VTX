#ifndef __VTX_APP_ECS__
#define __VTX_APP_ECS__

#include <entt/entt.hpp>

namespace VTX::App::ECS
{
	using Registry = entt::registry;
	using Entity   = entt::entity;

	void	   setRegistry( Registry & );
	Registry & registry() noexcept;

	template<class T, class... Args>
	T & setCtx( Args &&... p_args )
	{
		return registry().ctx().emplace<T>( std::forward<Args>( p_args )... );
	}

	template<class T>
	T & getCtx()
	{
		return registry().ctx().get<T>();
	}

	template<class T>
	bool hasCtx()
	{
		return registry().ctx().contains<T>();
	}

} // namespace VTX::App::ECS

namespace VTX::App
{
	inline ECS::Registry & REG() { return ECS::registry(); }
} // namespace VTX::App

#endif
