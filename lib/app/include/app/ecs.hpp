#ifndef __VTX_APP_ECS__
#define __VTX_APP_ECS__

#include <entt/entt.hpp>

namespace VTX::App::ECS
{
	using Registry = entt::registry;
	using Entity   = entt::entity;
	using Null	   = entt::null_t;
	constexpr Null NullEntity {};

	/**
	 * @brief Set the global registry.
	 */
	void setRegistry( Registry & );

	/**
	 * @brief Get the global registry.
	 */
	Registry & registry() noexcept;

	/**
	 * @brief Add a global service to the registry context.
	 */
	template<class T, class... Args>
	T & setCtx( Args &&... p_args )
	{
		return registry().ctx().emplace<T>( std::forward<Args>( p_args )... );
	}

	/**
	 * @brief Get a global service from the registry context.
	 */
	template<class T>
	T & getCtx()
	{
		return registry().ctx().get<T>();
	}

	/**
	 * @brief Check for existence of a global service in the registry context.
	 */
	template<class T>
	bool hasCtx()
	{
		return registry().ctx().contains<T>();
	}

	/**
	 * @brief Remove a global service from the registry context.
	 */
	template<class T>
	void removeCtx()
	{
		registry().ctx().erase<T>();
	}

	/**
	 * @brief Get the first entity with components of type in the registry.
	 */
	template<typename... T>
	auto getFirstEntityWithComponents()
	{
		auto view = registry().view<T...>();
		assert( view.begin() != view.end() );
		Entity e = *view.begin();
		return std::tuple<Entity, T &...> { e, view.template get<T>( e )... };
	}

	/**
	 * @brief Get only the first entity in the registry with components.
	 */
	template<typename... T>
	Entity getFirstEntityOnlyWithComponents()
	{
		auto view = registry().view<T...>();
		assert( view.begin() != view.end() );
		return *view.begin();
	}

	/**
	 * @brief Get the first component of type T in the registry.
	 */
	template<typename T>
	T & getFirstComponent()
	{
		return registry().get<T>( getFirstEntityOnlyWithComponents<T>() );
	}

} // namespace VTX::App::ECS

namespace VTX::App
{
	inline ECS::Registry & REG() { return ECS::registry(); }

} // namespace VTX::App

#endif
