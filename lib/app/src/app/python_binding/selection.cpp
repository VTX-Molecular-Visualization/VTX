#include "app/python_binding/selection.hpp"
#include "app/action/color.hpp"
#include "app/action/representation.hpp"
#include "app/action/selection.hpp"
#include "app/action/visibility.hpp"
#include "app/python_binding/preset/helpers.hpp"
#include "app/python_binding/preset/types.hpp"
#include "app/python_binding/topology/actions.hpp"
#include "app/python_binding/topology/types.hpp"
#include "app/system/selection.hpp"
#include <core/struct/topology.hpp>
#include <optional>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <python_binding/binding/helper.hpp>
#include <renderer/color.hpp>
#include <string>
#include <vector>

namespace VTX::App::PythonBinding
{
	namespace
	{
		using SystemItem = Core::Struct::E_SYSTEM_ITEM;

		struct SelectionView
		{
			std::vector<Topology::System> getSystems() const;
			Topology::AtomCollection	  getAtoms( const Topology::System & p_system ) const;
			Topology::ResidueCollection	  getResidues( const Topology::System & p_system ) const;
			Topology::ChainCollection	  getChains( const Topology::System & p_system ) const;
			Topology::CategoryCollection  getCategories( const Topology::System & p_system ) const;
			bool						  isEmpty( const Topology::System & p_system ) const;
			void						  show() const;
			void						  hide() const;
			void						  setRepresentation( const std::string & p_name ) const;
			void						  setRepresentation( const Preset::RepresentationHandle & p_preset ) const;
			void						  setColorScheme(
				Renderer::E_COLOR_SCHEME			p_scheme,
				std::optional<Renderer::ColorIndex> p_colorIndex
			) const;
		};

		const App::System::Selection & _getSelection( const Topology::System & p_system )
		{
			if ( not REG().all_of<Core::Struct::Topology, App::System::Selection>( p_system.entity ) )
			{
				throw pybind11::value_error( "Invalid system." );
			}

			return REG().get<App::System::Selection>( p_system.entity );
		}

		std::vector<Topology::System> SelectionView::getSystems() const
		{
			std::vector<Topology::System> systems;

			REG().view<Core::Struct::Topology, App::System::Selection>().each(
				[ &systems ](
					const Entity p_ent, const Core::Struct::Topology &, const App::System::Selection & p_selection
				)
				{
					if ( p_selection.atoms.any() )
					{
						systems.push_back( Topology::System { p_ent } );
					}
				}
			);

			return systems;
		}

		Topology::AtomCollection SelectionView::getAtoms( const Topology::System & p_system ) const
		{
			const App::System::Selection & selection = _getSelection( p_system );
			return { p_system.entity, selection.atoms.toRangeList<Index>() };
		}

		template<typename Collection, typename AtomToIndex>
		Collection _collectFromSelectedAtoms( const Topology::System & p_system, const AtomToIndex & p_atomToIndex )
		{
			const App::System::Selection & selection = _getSelection( p_system );
			const Core::Struct::Topology & topology	 = REG().get<Core::Struct::Topology>( p_system.entity );
			Topology::RangeList			   indices;

			for ( const Index atom : selection.atoms.toRangeList<Index>() )
			{
				indices.addValue( p_atomToIndex( topology, atom ) );
			}

			return { p_system.entity, indices };
		}

		Topology::ResidueCollection SelectionView::getResidues( const Topology::System & p_system ) const
		{
			return _collectFromSelectedAtoms<Topology::ResidueCollection>(
				p_system,
				[]( const Core::Struct::Topology & p_topology, const Index p_atom )
				{ return p_topology.getAtomResidueIndex( p_atom ); }
			);
		}

		Topology::ChainCollection SelectionView::getChains( const Topology::System & p_system ) const
		{
			return _collectFromSelectedAtoms<Topology::ChainCollection>(
				p_system,
				[]( const Core::Struct::Topology & p_topology, const Index p_atom )
				{ return p_topology.getAtomChainIndex( p_atom ); }
			);
		}

		Topology::CategoryCollection SelectionView::getCategories( const Topology::System & p_system ) const
		{
			return _collectFromSelectedAtoms<Topology::CategoryCollection>(
				p_system,
				[]( const Core::Struct::Topology & p_topology, const Index p_atom )
				{
					return static_cast<Index>(
						toUnderlying( p_topology.getResidueCategory( p_topology.getAtomResidueIndex( p_atom ) ) )
					);
				}
			);
		}

		bool SelectionView::isEmpty( const Topology::System & p_system ) const
		{ return _getSelection( p_system ).atoms.none(); }

		void SelectionView::show() const { executeAction<App::Action::Visibility::SetVisibleSelected>( true ); }

		void SelectionView::hide() const { executeAction<App::Action::Visibility::SetVisibleSelected>( false ); }

		void SelectionView::setRepresentation( const std::string & p_name ) const
		{
			THREAD().synchronize(
				[ &p_name ]()
				{
					Preset::executePresetAction<App::Action::Representation::AddSelected>(
						Topology::getRepresentationPreset( p_name )
					);
				}
			);
		}

		void SelectionView::setRepresentation( const Preset::RepresentationHandle & p_preset ) const
		{ Preset::executePresetAction<App::Action::Representation::AddSelected>( p_preset ); }

		void SelectionView::setColorScheme(
			const Renderer::E_COLOR_SCHEME			  p_scheme,
			const std::optional<Renderer::ColorIndex> p_colorIndex
		) const
		{
			Topology::validateColorScheme( p_scheme, p_colorIndex );
			executeAction<App::Action::Color::AddSelected>( p_scheme, p_colorIndex );
		}
	} // namespace

	void SelectionBinder::bind( Module & p_vtxModule )
	{
		const pybind11::module_ & module = p_vtxModule.pyModule();

		VTX::PythonBinding::Helper::declareEnum<SystemItem>( module, "SYSTEM_ITEM" );
		VTX::PythonBinding::Helper::declareEnum<App::System::E_SELECTION_STATE>( module, "SELECTION_STATE" );

		pybind11::class_<SelectionView>( module, "Selection", pybind11::module_local() )
			.def( "getSystems", synchronized( &SelectionView::getSystems ) )
			.def( "getAtoms", synchronized( &SelectionView::getAtoms ), pybind11::arg( "system" ) )
			.def( "getResidues", synchronized( &SelectionView::getResidues ), pybind11::arg( "system" ) )
			.def( "getChains", synchronized( &SelectionView::getChains ), pybind11::arg( "system" ) )
			.def( "getCategories", synchronized( &SelectionView::getCategories ), pybind11::arg( "system" ) )
			.def( "isEmpty", synchronized( &SelectionView::isEmpty ), pybind11::arg( "system" ) )
			.def( "show", &SelectionView::show, "Show the current selection." )
			.def( "hide", &SelectionView::hide, "Hide the current selection." )
			.def(
				"setRepresentation",
				pybind11::overload_cast<const std::string &>( &SelectionView::setRepresentation, pybind11::const_ ),
				"Set the representation preset of the current selection.",
				pybind11::arg( "name" )
			)
			.def(
				"setRepresentation",
				pybind11::overload_cast<const Preset::RepresentationHandle &>(
					&SelectionView::setRepresentation, pybind11::const_
				),
				"Set the representation preset of the current selection.",
				pybind11::arg( "preset" )
			)
			.def(
				"setColorScheme",
				&SelectionView::setColorScheme,
				"Set the color scheme of the current selection. A color index is required for custom schemes.",
				pybind11::arg( "scheme" ),
				pybind11::arg( "colorIndex" ) = pybind11::none()
			);
		module.attr( "selection" ) = SelectionView {};

		p_vtxModule.bindAction<App::Action::Selection::SelectAll>( "selectAll", "Select all loaded systems." );
		p_vtxModule.bindAction<App::Action::Selection::Clear>(
			"clearSelection", "Clear the current selection on all loaded systems."
		);
	}
} // namespace VTX::App::PythonBinding
