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

namespace VTX::Util::ECS
{
	/**
	 * @brief Helper type to check if a view get function returns void (i.e. tag component).
	 */
	template<typename View, typename T, typename Entity>
	using view_get_t = decltype( std::declval<View &>().template get<T>( std::declval<Entity>() ) );
	template<typename View, typename T, typename Entity>
	inline constexpr bool view_get_is_void_v = std::is_void_v<view_get_t<View, T, Entity>>;

	/**
	 * @brief Add a global service to the registry context.
	 */
	template<class T, class... Args>
	T & setCtx( Registry & p_registry, Args &&... p_args )
	{
		VTX_TRACE( "Adding service {}", Util::typeName<T>() );
		return p_registry.ctx().emplace<T>( std::forward<Args>( p_args )... );
	}

	/**
	 * @brief Add a global service to the registry context.
	 */
	template<class T>
	bool eraseCtx( Registry & p_registry )
	{
		VTX_TRACE( "Removing service {}", Util::typeName<T>() );
		return p_registry.ctx().erase<T>();
	}

	/**
	 * @brief Get a global service from the registry context.
	 */
	template<class T>
	T & getCtx( Registry & p_registry )
	{
		return p_registry.ctx().get<T>();
	}

	/**
	 * @brief Check for existence of a global service in the registry context.
	 */
	template<class T>
	bool hasCtx( Registry & p_registry )
	{
		return p_registry.ctx().contains<T>();
	}

	/**
	 * @brief Remove a global service from the registry context.
	 */
	template<class T>
	void removeCtx( Registry & p_registry )
	{
		p_registry.ctx().erase<T>();
	}

	/**
	 * @brief Get the first entity with components (references) of type in the registry.
	 */
	template<typename... T>
	auto getFirstEntityWithComponents( Registry & p_registry )
	{
		auto view = p_registry.view<T...>();
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
	Entity getFirstEntityOnlyWithComponents( Registry & p_registry )
	{
		auto view = p_registry.view<T...>();
		assert( view.begin() != view.end() );
		return *view.begin();
	}

	/**
	 * @brief Get the first component of type T in the registry.
	 */
	template<typename T>
	T & getFirstComponent( Registry & p_registry )
	{
		return p_registry.get<T>( getFirstEntityOnlyWithComponents( p_registry ) );
	}

	/**
	 * @brief Debug infos.
	 */
	inline void toString( Registry & p_registry )
	{
		for ( const entt::entity e : p_registry.view<entt::entity>() )
		{
			std::cout << "entity " << int( e ) << '\n';

			for ( auto && [ id, storage ] : p_registry.storage() )
			{
				if ( storage.contains( e ) )
				{
					std::cout << " - component id: " << id << '\n';
				}
			}
		}
	}

} // namespace VTX::Util::ECS

#endif
