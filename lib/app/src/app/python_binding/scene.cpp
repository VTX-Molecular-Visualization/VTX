#include "app/python_binding/scene.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/scene.hpp"
#include "app/helper/preset.hpp"
#include <python_binding/binding/entity_caster.hpp>
#include <python_binding/wrapper/arg.hpp>
#include <renderer/color.hpp>
#include <renderer/graphics_config.hpp>
#include <string>
#include <util/exceptions.hpp>

namespace VTX::App::PythonBinding
{
	namespace
	{
		template<typename Preset>
		Entity _getPresetByName( const std::string & p_name, const std::string & p_type )
		{
			const std::optional<Entity> preset = Helper::Preset::getByName<Preset>( p_name );
			if ( not preset )
			{
				throw VTXException( p_type + " preset not found: " + p_name );
			}
			return *preset;
		}

		struct SetColorLayoutByName
		{
			void execute( const std::string & p_name )
			{
				App::Action::Scene::SetColorLayout().execute(
					_getPresetByName<Renderer::Color::Layout>( p_name, "Color layout" )
				);
			}
		};

		struct SetGraphicsConfigByName
		{
			void execute( const std::string & p_name )
			{
				App::Action::Scene::SetGraphicsConfig().execute(
					_getPresetByName<Renderer::GraphicsConfig>( p_name, "Graphics config" )
				);
			}
		};
	} // namespace

	void SceneBinder::bind( Module & p_vtxModule )
	{
		p_vtxModule.bindAction<App::Action::Scene::Clear>( "newScene", "Clear scene." );
		p_vtxModule.bindAction<App::Action::Scene::DeleteSystem, const Entity>(
			"deleteSystem", "Delete a system from the scene.", VTX::PythonBinding::Wrapper::Arg( "systemId" )
		);
		p_vtxModule.bindAction<App::Action::Scene::DeleteSystemSelected>(
			"deleteSystemSelected", "Delete all selected systems from the scene."
		);
		p_vtxModule.bindAction<SetColorLayoutByName, const std::string &>(
			"setColorLayout", "Set the scene color layout preset.", VTX::PythonBinding::Wrapper::Arg( "name" )
		);
		p_vtxModule.bindAction<SetGraphicsConfigByName, const std::string &>(
			"setGraphicsConfig", "Set the scene graphics config preset.", VTX::PythonBinding::Wrapper::Arg( "name" )
		);
	}
} // namespace VTX::App::PythonBinding
