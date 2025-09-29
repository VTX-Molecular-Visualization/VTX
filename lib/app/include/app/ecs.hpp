#ifndef __VTX_APP_ECS__
#define __VTX_APP_ECS__

#include <util/event_hub.hpp>

namespace VTX::App::ECS
{
	using Registry = entt::registry;
	using Entity   = entt::entity;

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

} // namespace VTX::App::ECS

namespace VTX::App
{
	inline ECS::Registry &	REG() { return ECS::registry(); }
	inline Util::EventHub & HUB() { return ECS::getCtx<Util::EventHub>(); }
} // namespace VTX::App

#endif
