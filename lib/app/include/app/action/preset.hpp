#ifndef __VTX_APP_ACTION_PRESET__
#define __VTX_APP_ACTION_PRESET__

#include "app/action/action_manager.hpp"
#include "app/helper/preset.hpp"
#include "app/preset/instance.hpp"
#include "app/preset/name.hpp"
#include "app/services.hpp"
#include <optional>
#include <renderer/color.hpp>
#include <renderer/representation.hpp>
#include <renderer/settings.hpp>

namespace VTX::App::Action::Preset
{
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
			std::string name = std::string { p_name.has_value() ? p_name.value() : "New preset" };

			while ( Helper::Preset::exists<T>( name ) )
			{
				name += " (2)";
			}

			auto e = reg.create();
			reg.emplace<App::Preset::Name>( e, name );
			if ( p_data )
			{
				reg.emplace<T>( e, *p_data );
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
		void execute( const ECS::Entity p_e, const std::string_view p_dest )
		{
			std::string name { p_dest };

			auto & src = REG().get<App::Preset::Name>( p_e );
			if ( src.name == p_dest )
			{
				return;
			}

			while ( Helper::Preset::exists<T>( name ) )
			{
				name += " (2)";
			}

			REG().patch<App::Preset::Name>( p_e, [ &name ]( App::Preset::Name & p_name ) { p_name.name = name; } );
		}
	};

	/**
	 * @brief Duplicate a preset in a library.
	 */
	template<typename T>
	struct Duplicate
	{
		void execute( const ECS::Entity p_e, const std::optional<std::string_view> p_dest = std::nullopt )
		{
			auto & reg = REG();
			auto & src = reg.get<App::Preset::Name>( p_e );

			std::string name = std::string { p_dest.has_value() ? p_dest.value() : src.name + "_copy" };

			while ( Helper::Preset::exists<T>( name ) )
			{
				name += "_copy";
			}

			auto e = reg.create();
			reg.emplace<App::Preset::Name>( e, name );
			reg.emplace<T>( e, reg.get<T>( p_e ) );
		}
	};

	/**
	 * @brief Delete a preset from a library.
	 */
	template<typename T>
	struct Delete
	{
		void execute( const ECS::Entity p_e )
		{
			auto view = REG().view<App::Preset::Name, T>();

			if ( view.size_hint() == 1 )
			{
				VTX_ERROR( "Cannot delete the last preset." );
				return;
			}

			auto viewInstance = REG().view<App::Preset::Instance<T>>();
			for ( const ECS::Entity entity : viewInstance )
			{
				const auto & presetInstance = viewInstance.get<App::Preset::Instance<T>>( entity );
				if ( presetInstance.entity == p_e )
				{
					VTX_ERROR( "Cannot delete a preset in use." );
					return;
				}
			}

			REG().destroy( p_e );
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
		void execute() { ACTION().execute<Add<Renderer::Color::Layout>>( "JMol", Renderer::Color::Layouts::JMOL ); }
	};

	template<>
	struct CreateDefault<Renderer::Representation>
	{
		void execute()
		{
			ACTION().execute<Add<Renderer::Representation>>( "Sticks", Renderer::Representations::STICKS );
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
	struct CreateDefault<Renderer::RenderSettings>
	{
		void execute() { ACTION().execute<Add<Renderer::RenderSettings>>( "Default" ); }
	};
} // namespace VTX::App::Action::Preset
#endif
