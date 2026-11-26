#ifndef VTX_APP_HELPER_PRESET_HPP
#define VTX_APP_HELPER_PRESET_HPP

#include "app/ecs.hpp"
#include "app/generic/name.hpp"
#include <algorithm>
#include <optional>
#include <renderer/color.hpp>
#include <renderer/graphics_config.hpp>
#include <renderer/material.hpp>
#include <renderer/representation.hpp>
#include <string_view>
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
		return std::ranges::any_of(
			view,
			[ &view, &name ]( const Entity entity )
			{
				const auto & presetName = view.template get<App::Generic::Name>( entity ).name;
				return Util::String::toLower( presetName ) == name;
			}
		);

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

	/**
	 * @brief Default preset values.
	 * // TODO: move to settings.
	 */
	template<typename T>
	inline constexpr uint VERSION = 1;

	template<typename T>
	inline constexpr std::string_view TYPE_NAME = "";

	template<>
	inline constexpr std::string_view TYPE_NAME<Renderer::Representation> = "representation";

	template<>
	inline constexpr std::string_view TYPE_NAME<Renderer::Color::Layout> = "color_layout";

	template<>
	inline constexpr std::string_view TYPE_NAME<Renderer::GraphicsConfig> = "graphics_config";

	template<>
	inline constexpr std::string_view TYPE_NAME<Renderer::Material> = "material";

	template<typename T>
	inline constexpr std::string_view DEFAULT_NAME = "Default";

	template<>
	inline constexpr std::string_view DEFAULT_NAME<Renderer::Representation> = "Sticks and Ribbons";

	template<>
	inline constexpr std::string_view DEFAULT_NAME<Renderer::Color::Layout> = "JMol";

	template<typename T>
	std::optional<Entity> getDefault()
	{
		if ( const std::optional<Entity> entity = getByName<T>( DEFAULT_NAME<T> ) )
		{
			return entity;
		}

		auto view = REG().view<App::Generic::Name, T>();
		if ( view.begin() == view.end() )
		{
			return std::nullopt;
		}
		return *view.begin();
	}

} // namespace VTX::App::Helper::Preset

#endif
