#ifndef __VTX_APP_HELPER_PRESET__
#define __VTX_APP_HELPER_PRESET__

#include "app/ecs.hpp"
#include "app/generic/name.hpp"
#include <optional>
#include <util/string.hpp>
#include <util/types.hpp>

namespace VTX::App::Helper::Preset
{
	/**
	 * @brief Check if a preset with the given name exists in the library.
	 */
	template<typename T>
	bool exists( const std::string_view p_name )
	{
		const std::string name = Util::String::toLower( std::string( p_name ) );
		auto			  view = REG().view<App::Generic::Name, T>();
		for ( const Entity entity : view )
		{
			const auto & presetName = view.template get<App::Generic::Name>( entity ).name;
			if ( Util::String::toLower( presetName ) == name )
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
	std::optional<Entity> getByName( const std::string_view p_name )
	{
		const std::string name = Util::String::toLower( std::string( p_name ) );
		auto			  view = REG().view<App::Generic::Name, T>();
		for ( const Entity entity : view )
		{
			const auto & presetName = view.template get<App::Generic::Name>( entity ).name;
			if ( Util::String::toLower( presetName ) == name )
			{
				return entity;
			}
		}
		return std::nullopt;
	}

} // namespace VTX::App::Helper::Preset

#endif
