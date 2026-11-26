#ifndef VTX_APP_ACTION_PRESET_HPP
#define VTX_APP_ACTION_PRESET_HPP

#include "app/action/representation.hpp"
#include "app/action/scene.hpp"
#include "app/events.hpp"
#include "app/generic/file.hpp"
#include "app/generic/name.hpp"
#include "app/generic/readonly.hpp"
#include "app/helper/preset.hpp"
#include "app/scene/tag_root.hpp"
#include "app/serialization/renderer.hpp"
#include "app/services.hpp"
#include <algorithm>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <optional>
#include <renderer/color.hpp>
#include <renderer/graphics_config.hpp>
#include <renderer/material.hpp>
#include <renderer/renderer.hpp>
#include <renderer/representation.hpp>
#include <util/action_manager.hpp>
#include <util/event_hub.hpp>
#include <util/exceptions.hpp>
#include <util/filesystem.hpp>
#include <util/json.hpp>
#include <util/logger.hpp>
#include <util/type_traits.hpp>
#include <vector>

namespace VTX::App::Action::Preset
{

	/**
	 * @brief Default name for a preset when no name is provided.
	 */
	constexpr std::string_view DEFAULT_PRESET_NAME = "New preset";

	/**
	 * @brief Add a new preset to a library.
	 */
	template<typename T>
	struct Add
	{
	  public:
		void execute(
			const std::optional<std::string_view> p_name = std::nullopt,
			const std::optional<T> &			  p_data = std::nullopt
		)
		{
			auto &		reg	 = REG();
			std::string name = std::string { p_name.has_value() ? p_name.value() : DEFAULT_PRESET_NAME };

			while ( Helper::Preset::exists<T>( name ) )
			{
				name += " (2)";
			}

			auto e = reg.create();
			reg.emplace<App::Generic::Name>( e, name );
			if ( p_data )
			{
				reg.emplace<T>( e, *p_data );
			}
			else if ( const std::optional<Entity> source = Helper::Preset::getDefault<T>() )
			{
				const T data = reg.get<T>( *source );
				reg.emplace<T>( e, data );
			}
			else
			{
				reg.emplace<T>( e );
			}
		}
	};

	/**
	 * @brief Rename a preset in a library.
	 */
	template<typename T>
	struct Rename
	{
		void execute( const Entity p_e, const std::string_view p_dest )
		{
			if ( REG().all_of<Generic::ReadOnly>( p_e ) )
			{
				throw VTXException( "Cannot rename a read-only preset." );
			}

			std::string name { p_dest };

			auto & src = REG().get<App::Generic::Name>( p_e );
			if ( src.name == p_dest )
			{
				return;
			}

			while ( Helper::Preset::exists<T>( name ) )
			{
				name += " (2)";
			}

			auto & nameComponent = REG().get<App::Generic::Name>( p_e );
			nameComponent.name	 = name;
			HUB().trigger<App::Events::PresetRename<T>>( p_e, name );
		}
	};

	/**
	 * @brief Duplicate a preset in a library.
	 */
	template<typename T>
	struct Duplicate
	{
		void execute( const Entity p_e, const std::optional<std::string_view> p_dest = std::nullopt )
		{
			auto & reg = REG();
			auto & src = reg.get<App::Generic::Name>( p_e );

			std::string name = std::string { p_dest.has_value() ? p_dest.value() : src.name + "_copy" };

			while ( Helper::Preset::exists<T>( name ) )
			{
				name += "_copy";
			}

			auto e = reg.create();
			reg.emplace<App::Generic::Name>( e, name );
			reg.emplace<T>( e, reg.get<T>( p_e ) );
		}
	};

	/**
	 * @brief Delete a preset from a library.
	 */
	template<typename T>
	struct Delete
	{
		void execute( const Entity p_e )
		{
			if ( REG().all_of<Generic::ReadOnly>( p_e ) )
			{
				throw VTXException( "Cannot delete a read-only preset." );
			}

			auto view = REG().view<App::Generic::Name, T>();

			if ( view.size_hint() == 1 )
			{
				throw VTXException( "Cannot delete the last preset." );
			}

			// TODO: check if preset is used in an instance.
			/*
			auto viewInstance = REG().view<App::Preset::Instance<T>>();
			for ( const Entity entity : viewInstance )
			{
				const auto & presetInstance = viewInstance.template get<App::Preset::Instance<T>>( entity );
				if ( presetInstance.entity == p_e )
				{
					throw VTXException( "Cannot delete a preset in use." );
				}
			}
			*/

			REG().destroy( p_e );
		}
	};

	/**
	 * @brief Apply a preset to the scene.
	 */
	template<typename T>
	struct Apply
	{
		void execute( const Entity p_e )
		{
			if constexpr ( std::is_same_v<T, Renderer::Color::Layout> )
			{
				ACTION().execute<Scene::SetColorLayout>( p_e );
			}
			else if constexpr ( std::is_same_v<T, Renderer::GraphicsConfig> )
			{
				ACTION().execute<Scene::SetGraphicsConfig>( p_e );
			}
			else if constexpr ( std::is_same_v<T, Renderer::Material> )
			{
				ACTION().execute<Scene::SetMaterial>( p_e );
			}
			else if constexpr ( std::is_same_v<T, Renderer::Representation> )
			{
				// Loop over all systems and apply the representation preset to each of them.
				auto view = REG().view<App::System::Representation>();
				for ( const Entity entity : view )
				{
					ACTION().execute<Action::Representation::Add<Core::Struct::E_SYSTEM_ITEM::SYSTEM>>( entity, p_e );
				}
			}
			else
			{
				static_assert( always_false_t<T>, "Unsupported preset type." );
			}
		}
	};

	/**
	 * @brief Load all presets from a directory.
	 */
	template<typename T>
	struct LoadDirectory
	{
		void execute( const FilePath & p_directory, const bool p_readOnly )
		{
			if ( not std::filesystem::is_directory( p_directory ) )
			{
				return;
			}

			std::vector<FilePath> files;
			for ( const std::filesystem::directory_entry & entry : std::filesystem::directory_iterator( p_directory ) )
			{
				if ( entry.is_regular_file() && entry.path().extension() == ".json" )
				{
					files.push_back( entry.path() );
				}
			}
			std::ranges::sort( files );

			for ( const FilePath & file : files )
			{
				_load( file, p_readOnly );
			}
		}

	  private:
		void _load( const FilePath & p_path, const bool p_readOnly )
		{
			try
			{
				const Util::Json::Document document = Util::Json::read( p_path );

				const uint32_t version = document.at( "version" ).get<uint32_t>();
				if ( version > Helper::Preset::VERSION<T> )
				{
					VTX_WARNING(
						"Preset {} skipped: version {} is newer than supported version {}",
						p_path.string(),
						version,
						Helper::Preset::VERSION<T>
					);
					return;
				}

				const std::string type = document.at( "type" ).get<std::string>();
				if ( type != Helper::Preset::TYPE_NAME<T> )
				{
					VTX_WARNING(
						"Preset {} skipped: type \"{}\" does not match expected type \"{}\"",
						p_path.string(),
						type,
						Helper::Preset::TYPE_NAME<T>
					);
					return;
				}

				const std::string name = document.at( "name" ).get<std::string>();
				if ( Helper::Preset::exists<T>( name ) )
				{
					VTX_WARNING( "Preset {} skipped: a preset named \"{}\" already exists", p_path.string(), name );
					return;
				}

				T data = document.at( "data" ).get<T>();

				Registry &	 reg	= REG();
				const Entity entity = reg.create();
				reg.emplace<Generic::Name>( entity, name );
				reg.emplace<T>( entity, std::move( data ) );
				reg.emplace<Generic::File>( entity, p_path );
				if ( p_readOnly )
				{
					reg.emplace<Generic::ReadOnly>( entity );
				}
			}
			catch ( const std::exception & p_e )
			{
				VTX_WARNING( "Can not load preset {}: {}", p_path.string(), p_e.what() );
			}
		}
	};

	/**
	 * @brief Load all presets of a given type (default an user).
	 */
	template<typename T>
	struct Load
	{
		void execute( const FilePath & p_defaultDirectory, const FilePath & p_userDirectory )
		{
			ACTION().execute<LoadDirectory<T>>( p_defaultDirectory, true );
			ACTION().execute<LoadDirectory<T>>( p_userDirectory, false );

			if ( Helper::Preset::getDefault<T>() )
			{
				return;
			}

			VTX_ERROR( "No preset found in {}, using default values", p_defaultDirectory.string() );

			Registry &	 reg	= REG();
			const Entity entity = reg.create();
			reg.emplace<Generic::Name>( entity, std::string( Helper::Preset::DEFAULT_NAME<T> ) );
			reg.emplace<T>( entity );
			reg.emplace<Generic::ReadOnly>( entity );
		}
	};

	/**
	 * @brief Load all presets of all types.
	 */
	struct LoadAll
	{ void execute(); };
} // namespace VTX::App::Action::Preset
#endif
