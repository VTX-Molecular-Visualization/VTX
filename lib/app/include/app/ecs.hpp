#ifndef __VTX_APP_ECS__
#define __VTX_APP_ECS__

#include <entt/entt.hpp>
#include <iostream>

namespace VTX::App::ECS
{
	/**
	 * @brief Aliases.
	 */
	using Registry = entt::registry;
	using Entity   = entt::entity;

	/**
	 * @brief Helper type to check if a view get function returns void (i.e. tag component).
	 */
	template<typename View, typename T, typename Entity>
	using view_get_t = decltype( std::declval<View &>().template get<T>( std::declval<Entity>() ) );
	template<typename View, typename T, typename Entity>
	inline constexpr bool view_get_is_void_v = std::is_void_v<view_get_t<View, T, Entity>>;

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
	 * @brief Add a global service to the registry context.
	 */
	template<class T>
	bool eraseCtx()
	{
		return registry().ctx().erase<T>();
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
	 * @brief Get the first entity with components (references) of type in the registry.
	 */
	template<typename... T>
	auto getFirstEntityWithComponents()
	{
		auto view = registry().view<T...>();
		assert( view.begin() != view.end() );

		Entity e = *view.begin();

		auto make = [ & ]<typename C>() -> decltype( auto )
		{
			if constexpr ( std::is_void_v<decltype( view.template get<C>( e ) )> )
			{
				// Tag.
				return std::tuple<> {};
			}
			else
			{
				// Component.
				return std::tuple<C &> { view.template get<C>( e ) };
			}
		};

		return std::tuple_cat( std::tuple<Entity> { e }, make.template operator()<T>()... );
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

	/**
	 * @brief Debug infos.
	 */
	inline void regToString()
	{
		for ( const entt::entity e : registry().view<entt::entity>() )
		{
			std::cout << "entity " << int( e ) << '\n';

			for ( auto && [ id, storage ] : registry().storage() )
			{
				if ( storage.contains( e ) )
				{
					std::cout << " - component id: " << id << '\n';
				}
			}
		}
	}

} // namespace VTX::App::ECS

namespace VTX::App
{
	inline ECS::Registry & REG() { return ECS::registry(); }

} // namespace VTX::App

#endif
