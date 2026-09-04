#include "app/python_binding/representation.hpp"
#include "app/action/preset.hpp"
#include "app/action/representation.hpp"
#include "app/ecs.hpp"
#include "app/helper/preset.hpp"
#include "app/system/representation.hpp"
#include <core/struct/topology.hpp>
#include <pybind11/pybind11.h>
#include <python_binding/binding/entity_caster.hpp>
#include <python_binding/wrapper/arg.hpp>
#include <renderer/representation.hpp>
#include <string>
#include <util/exceptions.hpp>

namespace VTX::App::PythonBinding
{
	namespace
	{
		using SystemItem = Core::Struct::E_SYSTEM_ITEM;
		using RangeList	 = Core::Struct::IndexRangeList;

		Entity _getRepresentationPreset( const std::string & p_name )
		{
			const std::optional<Entity> preset = Helper::Preset::getByName<Renderer::Representation>( p_name );
			if ( not preset )
			{
				throw VTXException( "Representation preset not found: " + p_name );
			}
			return *preset;
		}

		void _validateSystem( const Entity p_ent )
		{
			if ( not REG().all_of<Core::Struct::Topology, App::System::Representation>( p_ent ) )
			{
				throw pybind11::value_error( "Invalid system id." );
			}
		}

		struct SetRepresentationByName
		{
			void execute(
				const Entity		p_ent,
				const std::string & p_presetName,
				const SystemItem	p_item,
				const RangeList &	p_ranges
			)
			{
				_validateSystem( p_ent );
				App::Action::Representation::AddItem().execute(
					p_ent, p_item, _getRepresentationPreset( p_presetName ), p_ranges
				);
			}
		};

		struct SetRepresentationSelectedByName
		{
			void execute( const std::string & p_presetName )
			{ App::Action::Representation::AddSelected().execute( _getRepresentationPreset( p_presetName ) ); }
		};

		struct SetRepresentationAllByName
		{
			void execute( const std::string & p_presetName )
			{
				App::Action::Preset::Apply<Renderer::Representation>().execute(
					_getRepresentationPreset( p_presetName )
				);
			}
		};
	} // namespace

	void RepresentationBinder::bind( Module & p_vtxModule )
	{
		p_vtxModule.bindAction<SetRepresentationAllByName, const std::string &>(
			"setRepresentation",
			"Set the representation preset of all systems.",
			VTX::PythonBinding::Wrapper::Arg( "presetName" )
		);
		p_vtxModule.bindAction<
			SetRepresentationByName,
			const Entity,
			const std::string &,
			const SystemItem,
			const RangeList &>(
			"setRepresentation",
			"Set the representation preset of system items.",
			VTX::PythonBinding::Wrapper::Arg( "systemId" ),
			VTX::PythonBinding::Wrapper::Arg( "presetName" ),
			VTX::PythonBinding::Wrapper::VArg<SystemItem>( "item", SystemItem::SYSTEM ),
			VTX::PythonBinding::Wrapper::VArg<RangeList>( "ranges", RangeList() )
		);
		p_vtxModule.bindAction<SetRepresentationSelectedByName, const std::string &>(
			"setRepresentationSelected",
			"Set the representation preset of the current selection.",
			VTX::PythonBinding::Wrapper::Arg( "presetName" )
		);
	}
} // namespace VTX::App::PythonBinding
