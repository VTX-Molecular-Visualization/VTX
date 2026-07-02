#ifndef __VTX_APP_ACTION_PRESET__
#define __VTX_APP_ACTION_PRESET__

#include "app/action/action_manager.hpp"
#include "app/action/representation.hpp"
#include "app/action/scene.hpp"
#include "app/events.hpp"
#include "app/generic/name.hpp"
#include "app/helper/preset.hpp"
#include "app/scene/tag_root.hpp"
#include "app/services.hpp"
#include <optional>
#include <renderer/color.hpp>
#include <renderer/graphics_config.hpp>
#include <renderer/material.hpp>
#include <renderer/renderer.hpp>
#include <renderer/representation.hpp>
#include <util/event_hub.hpp>
#include <util/exceptions.hpp>
#include <util/type_traits.hpp>

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
			else if constexpr ( std::is_same_v<T, Renderer::Color::Layout> )
			{
				reg.emplace<T>( e, Renderer::Color::Layouts::JMOL );
			}
			else if constexpr ( std::is_same_v<T, Renderer::GraphicsConfig> )
			{
				reg.emplace<T>( e, Renderer::GraphicsConfigs::DEFAULT );
			}
			else if constexpr ( std::is_same_v<T, Renderer::Representation> )
			{
				reg.emplace<T>( e, Renderer::Representations::STICKS_AND_RIBBONS );
			}
			else if constexpr ( std::is_same_v<T, Renderer::Material> )
			{
				reg.emplace<T>( e, Renderer::Materials::DEFAULT );
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
			else if constexpr ( std::is_same_v<T, Renderer::Representation> )
			{
				// Loop over all systems and apply the representation preset to each of them.
				auto view = REG().view<System::Representation>();
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
	 * @brief Create default preset.
	 */
	template<typename T>
	struct CreateDefault
	{
		void execute() {}
	};

	/**
	 * @brief Create default color layout preset.
	 */
	template<>
	struct CreateDefault<Renderer::Color::Layout>
	{
		void execute()
		{
			ACTION().execute<Add<Renderer::Color::Layout>>( "JMol", Renderer::Color::Layouts::JMOL );
			ACTION().execute<Add<Renderer::Color::Layout>>( "CPK", Renderer::Color::Layouts::CPK );
			ACTION().execute<Add<Renderer::Color::Layout>>( "Rasmol", Renderer::Color::Layouts::RASMOL );
		}
	};

	template<>
	struct CreateDefault<Renderer::Representation>
	{
		void execute()
		{
			ACTION().execute<Add<Renderer::Representation>>( "Sticks", Renderer::Representations::STICKS );
			ACTION().execute<Add<Renderer::Representation>>(
				"Sticks and Ribbons", Renderer::Representations::STICKS_AND_RIBBONS
			);
			ACTION().execute<Add<Renderer::Representation>>(
				"Balls and sticks", Renderer::Representations::BALLS_AND_STICKS
			);
			ACTION().execute<Add<Renderer::Representation>>(
				"Van der Waals", Renderer::Representations::VAN_DER_WAALS
			);
			ACTION().execute<Add<Renderer::Representation>>( "Ribbons", Renderer::Representations::RIBBONS );
			ACTION().execute<Add<Renderer::Representation>>( "SES", Renderer::Representations::SES );
		}
	};

	template<>
	struct CreateDefault<Renderer::GraphicsConfig>
	{
		void execute()
		{
			ACTION().execute<Add<Renderer::GraphicsConfig>>( "Default", Renderer::GraphicsConfigs::DEFAULT );
			ACTION().execute<Add<Renderer::GraphicsConfig>>( "Presentation", Renderer::GraphicsConfigs::PRESENTATION );
			ACTION().execute<Add<Renderer::GraphicsConfig>>( "Illustration", Renderer::GraphicsConfigs::ILLUSTRATION );
			ACTION().execute<Add<Renderer::GraphicsConfig>>( "PBR Metallic", Renderer::GraphicsConfigs::PBR_METALLIC );
		}
	};

	template<>
	struct CreateDefault<Renderer::Material>
	{
		void execute()
		{
			ACTION().execute<Add<Renderer::Material>>( "Default", Renderer::Materials::DEFAULT );
			ACTION().execute<Add<Renderer::Material>>( "Matte", Renderer::Materials::MATTE );
			ACTION().execute<Add<Renderer::Material>>( "Plastic", Renderer::Materials::PLASTIC );
			ACTION().execute<Add<Renderer::Material>>( "Polished metal", Renderer::Materials::POLISHED_METAL );
			ACTION().execute<Add<Renderer::Material>>( "Brushed metal", Renderer::Materials::BRUSHED_METAL );
		}
	};
} // namespace VTX::App::Action::Preset
#endif
