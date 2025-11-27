#ifndef __VTX_APP_HELPER_PRESET__
#define __VTX_APP_HELPER_PRESET__

#include "app/ecs.hpp"
#include "app/preset/name.hpp"
#include <optional>
#include <util/types.hpp>

namespace VTX::App::Helper::Preset
{
	/**
	 * @brief Check if a preset with the given name exists in the library.
	 */
	template<typename T>
	bool exists( const std::string_view p_name )
	{
		auto view = REG().view<App::Preset::Name, T>();
		for ( const ECS::Entity entity : view )
		{
			const auto & presetName = view.get<App::Preset::Name>( entity ).name;
			if ( presetName == p_name )
			{
				return true;
			}
		}
		return false;

	} // namespace VTX::App::Helper::Scene

	/**
	 * @brief Get a preset entity by its name.
	 */
	template<typename T>
	std::optional<ECS::Entity> getByName( const std::string_view p_name )
	{
		auto view = REG().view<App::Preset::Name, T>();
		for ( const ECS::Entity entity : view )
		{
			const auto & presetName = view.get<App::Preset::Name>( entity ).name;
			if ( presetName == p_name )
			{
				return entity;
			}
		}
		return std::nullopt;
	}

} // namespace VTX::App::Helper::Preset

#endif
