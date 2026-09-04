#include "app/python_binding/preset.hpp"
#include "app/action/preset.hpp"
#include "app/generic/name.hpp"
#include "app/helper/preset.hpp"
#include "app/python_binding/preset/color_layout.hpp"
#include "app/python_binding/preset/graphics_config.hpp"
#include "app/python_binding/preset/helpers.hpp"
#include "app/python_binding/preset/representation.hpp"
#include "app/python_binding/preset/types.hpp"
#include <optional>
#include <pybind11/stl.h>
#include <python_binding/binding/entity_caster.hpp>
#include <python_binding/wrapper/arg.hpp>
#include <renderer/color.hpp>
#include <renderer/graphics_config.hpp>
#include <renderer/representation.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace VTX::App::PythonBinding
{
	namespace py = pybind11;

	namespace
	{
		template<typename T>
		using PresetHandle = Preset::Handle<T>;

		template<typename T>
		std::string _getName( const PresetHandle<T> & p_preset )
		{
			Preset::validate( p_preset );
			return REG().get<App::Generic::Name>( p_preset.entity ).name;
		}

		template<typename T>
		PresetHandle<T> _getPreset( const std::string & p_name, const std::string_view p_typeName )
		{
			const std::optional<Entity> entity = Helper::Preset::getByName<T>( p_name );
			if ( not entity )
			{
				throw py::value_error( std::string( p_typeName ) + " preset not found: " + p_name );
			}
			return { *entity };
		}

		template<typename T>
		std::vector<PresetHandle<T>> _getPresets()
		{
			auto						 view = REG().view<App::Generic::Name, T>();
			std::vector<PresetHandle<T>> presets;
			presets.reserve( view.size_hint() );
			for ( const Entity entity : view )
			{
				presets.emplace_back( entity );
			}
			return presets;
		}

		template<typename T>
		void _rename( const PresetHandle<T> & p_preset, const std::string & p_name )
		{
			Preset::validate( p_preset );
			executeAction<Action::Preset::Rename<T>>( Entity( p_preset.entity ), std::string( p_name ) );
		}

		template<typename T>
		void _duplicate( const PresetHandle<T> & p_preset, const std::optional<std::string_view> p_name )
		{
			Preset::validate( p_preset );
			executeAction<Action::Preset::Duplicate<T>>(
				Entity( p_preset.entity ), std::optional<std::string_view>( p_name )
			);
		}

		template<typename T>
		void _deletePreset( const PresetHandle<T> & p_preset )
		{
			Preset::validate( p_preset );
			executeAction<Action::Preset::Delete<T>>( Entity( p_preset.entity ) );
		}

		template<typename T>
		py::class_<PresetHandle<T>> _bindPreset(
			Module &		   p_vtxModule,
			const char * const p_className,
			const char * const p_typeName,
			const char * const p_getFunctionName,
			const char * const p_getAllFunctionName,
			const char * const p_createFunctionName
		)
		{
			py::class_<PresetHandle<T>> binding(
				p_vtxModule.pyModule(), p_className, "Reference to an application preset.", py::module_local()
			);
			binding.def_property_readonly( "id", []( const PresetHandle<T> & p_preset ) { return p_preset.entity; } )
				.def_property_readonly( "name", &_getName<T> )
				.def( "rename", &_rename<T>, "Rename the preset.", py::arg( "name" ) )
				.def( "duplicate", &_duplicate<T>, "Duplicate the preset.", py::arg( "name" ) = py::none() )
				.def( "delete", &_deletePreset<T>, "Delete the preset." );

			p_vtxModule.def(
				p_getFunctionName,
				[ p_typeName ]( const std::string & p_name ) { return _getPreset<T>( p_name, p_typeName ); },
				"Get a preset by name.",
				VTX::PythonBinding::Wrapper::Arg( "name" )
			);
			p_vtxModule.def( p_getAllFunctionName, &_getPresets<T>, "Get all presets of this type." );
			p_vtxModule.def(
				p_createFunctionName,
				[]( const std::optional<std::string_view> p_name )
				{ executeAction<Action::Preset::Add<T>>( std::optional<std::string_view>( p_name ) ); },
				"Create a preset.",
				VTX::PythonBinding::Wrapper::VArg<std::optional<std::string_view>>(
					"name", std::optional<std::string_view>()
				)
			);
			return binding;
		}
	}

	void PresetBinder::bind( Module & p_vtxModule )
	{
		auto representation = _bindPreset<Renderer::Representation>(
			p_vtxModule,
			"RepresentationPreset",
			"Representation",
			"getRepresentationPreset",
			"getRepresentationPresets",
			"createRepresentationPreset"
		);
		Preset::bindRepresentation( p_vtxModule, representation );

		auto colorLayout = _bindPreset<Renderer::Color::Layout>(
			p_vtxModule,
			"ColorLayoutPreset",
			"Color layout",
			"getColorLayoutPreset",
			"getColorLayoutPresets",
			"createColorLayoutPreset"
		);
		Preset::bindColorLayout( p_vtxModule, colorLayout );

		auto graphicsConfig = _bindPreset<Renderer::GraphicsConfig>(
			p_vtxModule,
			"GraphicsConfigPreset",
			"Graphics config",
			"getGraphicsConfigPreset",
			"getGraphicsConfigPresets",
			"createGraphicsConfigPreset"
		);
		Preset::bindGraphicsConfig( p_vtxModule, graphicsConfig );
	}
}
