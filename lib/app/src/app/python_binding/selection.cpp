#include "app/python_binding/selection.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/selection.hpp"
#include "app/helper/system.hpp"
#include "app/python_binding/topology/types.hpp"
#include "app/services.hpp"
#include "app/system/selection.hpp"
#include <core/struct/topology.hpp>
#include <optional>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <python_binding/binding/entity_caster.hpp>
#include <python_binding/binding/helper.hpp>
#include <python_binding/wrapper/arg.hpp>
#include <vector>

namespace VTX::App::PythonBinding
{
	namespace
	{
		using SystemItem	= Core::Struct::E_SYSTEM_ITEM;
		using IndexList		= std::vector<Index>;
		using SelectionMode = App::Action::Selection::Clear::E_MODE;

		struct SelectionView
		{
		};

		const App::System::Selection & _getSelection( const Topology::System & p_system )
		{
			if ( not REG().all_of<Core::Struct::Topology, App::System::Selection>( p_system.entity ) )
			{
				throw pybind11::value_error( "Invalid system." );
			}

			return REG().get<App::System::Selection>( p_system.entity );
		}

		pybind11::list _getSelectedSystems( const SelectionView & )
		{
			pybind11::list systems;

			REG().view<Core::Struct::Topology, App::System::Selection>().each(
				[ &systems ](
					const Entity p_ent, const Core::Struct::Topology &, const App::System::Selection & p_selection
				)
				{
					if ( p_selection.atoms.any() )
					{
						systems.append( Topology::System { p_ent } );
					}
				}
			);

			return systems;
		}

		Topology::AtomCollection _getSelectedAtoms( const SelectionView &, const Topology::System & p_system )
		{
			const App::System::Selection & selection = _getSelection( p_system );
			return { p_system.entity, selection.atoms.toRangeList<Index>() };
		}

		Topology::ResidueCollection _getSelectedResidues( const SelectionView &, const Topology::System & p_system )
		{
			const App::System::Selection & selection = _getSelection( p_system );
			const Core::Struct::Topology & topology	 = REG().get<Core::Struct::Topology>( p_system.entity );
			Topology::RangeList			   residues;

			for ( const Index atom : selection.atoms.toRangeList<Index>() )
			{
				residues.addValue( topology.getAtomResidueIndex( atom ) );
			}

			return { p_system.entity, residues };
		}

		Topology::ChainCollection _getSelectedChains( const SelectionView &, const Topology::System & p_system )
		{
			const App::System::Selection & selection = _getSelection( p_system );
			const Core::Struct::Topology & topology	 = REG().get<Core::Struct::Topology>( p_system.entity );
			Topology::RangeList			   chains;

			for ( const Index atom : selection.atoms.toRangeList<Index>() )
			{
				chains.addValue( topology.getAtomChainIndex( atom ) );
			}

			return { p_system.entity, chains };
		}

		Topology::CategoryCollection _getSelectedCategories( const SelectionView &, const Topology::System & p_system )
		{
			const App::System::Selection & selection = _getSelection( p_system );
			const Core::Struct::Topology & topology	 = REG().get<Core::Struct::Topology>( p_system.entity );
			Topology::RangeList			   categories;

			for ( const Index atom : selection.atoms.toRangeList<Index>() )
			{
				const Index residue = topology.getAtomResidueIndex( atom );
				categories.addValue( toUnderlying( topology.getResidueCategory( residue ) ) );
			}

			return { p_system.entity, categories };
		}

		bool _isSelectionEmpty( const SelectionView &, const Topology::System & p_system )
		{ return _getSelection( p_system ).atoms.none(); }

		App::System::E_SELECTION_STATE _getSelectionState(
			const Entity			   p_ent,
			const SystemItem		   p_item,
			const std::optional<Index> p_index = std::nullopt
		)
		{
			if ( not REG().all_of<Core::Struct::Topology, App::System::Selection>( p_ent ) )
			{
				throw pybind11::value_error( "Invalid system id." );
			}

			return Helper::System::getSelectionState( { p_ent, p_item, p_index } );
		}
	} // namespace

	void SelectionBinder::bind( Module & p_vtxModule )
	{
		pybind11::module_ & module = p_vtxModule.pyModule();

		VTX::PythonBinding::Helper::declareEnum<SystemItem>( module, "SYSTEM_ITEM" );
		VTX::PythonBinding::Helper::declareEnum<App::Action::Selection::E_GRANULARITY>(
			module, "SELECTION_GRANULARITY"
		);
		VTX::PythonBinding::Helper::declareEnum<SelectionMode>( module, "SELECTION_CLEAR_MODE" );
		VTX::PythonBinding::Helper::declareEnum<App::System::E_SELECTION_STATE>( module, "SELECTION_STATE" );

		pybind11::class_<SelectionView>( module, "Selection", pybind11::module_local() )
			.def( "getSystems", &_getSelectedSystems )
			.def( "getAtoms", &_getSelectedAtoms, pybind11::arg( "system" ) )
			.def( "getResidues", &_getSelectedResidues, pybind11::arg( "system" ) )
			.def( "getChains", &_getSelectedChains, pybind11::arg( "system" ) )
			.def( "getCategories", &_getSelectedCategories, pybind11::arg( "system" ) )
			.def( "isEmpty", &_isSelectionEmpty, pybind11::arg( "system" ) );
		module.attr( "selection" ) = SelectionView {};

		p_vtxModule.bindAction<App::Action::Selection::SelectAll>( "selectAll", "Select all loaded systems." );
		p_vtxModule.bindAction<App::Action::Selection::Clear>(
			"clearSelection", "Clear the current selection on all loaded systems."
		);
		p_vtxModule.def(
			"clearSelection",
			[]( const Entity p_ent, const SelectionMode p_mode )
			{ executeAction<App::Action::Selection::Clear>( p_ent, p_mode ); },
			"Clear selection for one system.",
			VTX::PythonBinding::Wrapper::Arg( "systemId" ),
			VTX::PythonBinding::Wrapper::VArg<SelectionMode>( "mode", SelectionMode::THIS )
		);
		p_vtxModule.def(
			"select",
			[]( const Entity	  p_ent,
				const SystemItem  p_item,
				const IndexList & p_indices,
				const bool		  p_selected,
				const bool		  p_append )
			{
				if ( not REG().all_of<Core::Struct::Topology, App::System::Selection>( p_ent ) )
				{
					throw pybind11::value_error( "Invalid system id." );
				}

				executeAction<App::Action::Selection::SetSelectedItem>(
					p_ent, p_item, p_indices, p_selected, p_append
				);
			},
			"Set item selection for a system.",
			VTX::PythonBinding::Wrapper::Arg( "systemId" ),
			VTX::PythonBinding::Wrapper::Arg( "item" ),
			VTX::PythonBinding::Wrapper::VArg<IndexList>( "indices", IndexList() ),
			VTX::PythonBinding::Wrapper::VArg<bool>( "selected", true ),
			VTX::PythonBinding::Wrapper::VArg<bool>( "append", false )
		);
		p_vtxModule.def(
			"getSelectionState",
			&_getSelectionState,
			"Return the selection state of an item.",
			VTX::PythonBinding::Wrapper::Arg( "systemId" ),
			VTX::PythonBinding::Wrapper::VArg<SystemItem>( "item", SystemItem::SYSTEM ),
			VTX::PythonBinding::Wrapper::VArg<std::optional<Index>>( "index", std::optional<Index>() )
		);
	}
} // namespace VTX::App::PythonBinding
