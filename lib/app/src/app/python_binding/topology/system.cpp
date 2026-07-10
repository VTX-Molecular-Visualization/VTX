#include "app/python_binding/topology/actions.hpp"
#include "app/python_binding/topology/binding.hpp"
#include "app/python_binding/topology/helpers.hpp"
#include <core/chemdb/category.hpp>
#include <io/metadata.hpp>
#include <python_binding/binding/entity_caster.hpp>
#include <python_binding/wrapper/arg.hpp>

namespace VTX::App::PythonBinding::Topology
{
	namespace py = pybind11;

	namespace
	{
		void validateRange( const Range & p_range, const size_t p_size )
		{
			if ( p_range.getLast() > p_size )
			{
				throw pybind11::index_error( "Range is outside of the collection." );
			}
		}

		ChainCollection getChains( const System & p_system )
		{
			const auto & topology = getTopology( p_system.entity );
			return { p_system.entity, RangeList( topology.getChainRange() ) };
		}

		ResidueCollection getResidues( const System & p_system )
		{
			const auto & topology = getTopology( p_system.entity );
			return { p_system.entity, RangeList( Range::fromFirstCount( 0, topology.getResidueCount() ) ) };
		}

		AtomCollection getAtoms( const System & p_system )
		{
			const auto & topology = getTopology( p_system.entity );
			return { p_system.entity, RangeList( topology.getAtomRange() ) };
		}

		BondCollection getBonds( const System & p_system )
		{
			const auto & topology = getTopology( p_system.entity );
			return { p_system.entity, RangeList( Range::fromFirstCount( 0, topology.getBondCount() ) ) };
		}

		CategoryCollection getCategories( const System & p_system )
		{
			getTopology( p_system.entity );
			return { p_system.entity,
					 RangeList( Range::fromFirstCount( 0, toUnderlying( Core::ChemDB::Category::TYPE::COUNT ) ) ) };
		}

		const IO::Metadata & getMetadata( const Entity p_entity )
		{
			if ( not REG().all_of<IO::Metadata>( p_entity ) )
			{
				throw pybind11::value_error( "Invalid system metadata." );
			}

			return REG().get<IO::Metadata>( p_entity );
		}
	} // namespace

	Chain System::getChain( const Index p_index ) const
	{
		const auto & topology = getTopology( entity );
		validateRange( Range( p_index ), topology.getChainCount() );
		return { entity, p_index };
	}

	Residue System::getResidue( const Index p_index ) const
	{
		const auto & topology = getTopology( entity );
		validateRange( Range( p_index ), topology.getResidueCount() );
		return { entity, p_index };
	}

	Atom System::getAtom( const Index p_index ) const
	{
		const auto & topology = getTopology( entity );
		validateRange( Range( p_index ), topology.getAtomCount() );
		return { entity, p_index };
	}

	Bond System::getBond( const Index p_index ) const
	{
		const auto & topology = getTopology( entity );
		validateRange( Range( p_index ), topology.getBondCount() );
		return { entity, p_index };
	}

	Category System::getCategory( const Index p_index ) const
	{
		if ( p_index >= toUnderlying( Core::ChemDB::Category::TYPE::COUNT ) )
		{
			throw pybind11::index_error( "Index is outside of the collection." );
		}

		getTopology( entity );
		return { entity, p_index };
	}

	void bindSystem( Module & p_vtxModule, pybind11::module_ & p_module )
	{
		py::class_<System>( p_module, "System", py::module_local() )
			.def_property_readonly( "id", []( const System & p_system ) { return p_system.entity; } )
			.def_property_readonly(
				"name",
				[]( const System & p_system ) -> const std::string & { return getMetadata( p_system.entity ).name; }
			)
			.def_property_readonly(
				"path", []( const System & p_system ) { return getMetadata( p_system.entity ).path.string(); }
			)
			.def_property_readonly(
				"pdbIdCode",
				[]( const System & p_system ) -> const std::string &
				{ return getMetadata( p_system.entity ).pdbIDCode; }
			)
			.def_property_readonly(
				"chainCount", []( const System & p_system ) { return getTopology( p_system.entity ).getChainCount(); }
			)
			.def_property_readonly(
				"residueCount",
				[]( const System & p_system ) { return getTopology( p_system.entity ).getResidueCount(); }
			)
			.def_property_readonly(
				"atomCount", []( const System & p_system ) { return getTopology( p_system.entity ).getAtomCount(); }
			)
			.def_property_readonly(
				"bondCount", []( const System & p_system ) { return getTopology( p_system.entity ).getBondCount(); }
			)
			.def_property_readonly(
				"categoryCount",
				[]( const System & p_system )
				{
					getTopology( p_system.entity );
					return toUnderlying( Core::ChemDB::Category::TYPE::COUNT );
				}
			)
			.def_property_readonly(
				"indexFirstChain",
				[]( const System & p_system )
				{
					getTopology( p_system.entity );
					return Index( 0 );
				}
			)
			.def_property_readonly(
				"indexLastChain",
				[]( const System & p_system )
				{
					const Index count = getTopology( p_system.entity ).getChainCount();
					return count;
				}
			)
			.def_property_readonly(
				"indexFirstResidue",
				[]( const System & p_system )
				{
					getTopology( p_system.entity );
					return Index( 0 );
				}
			)
			.def_property_readonly(
				"indexLastResidue",
				[]( const System & p_system )
				{
					const Index count = getTopology( p_system.entity ).getResidueCount();
					return count;
				}
			)
			.def_property_readonly(
				"indexFirstAtom",
				[]( const System & p_system )
				{
					getTopology( p_system.entity );
					return Index( 0 );
				}
			)
			.def_property_readonly(
				"indexLastAtom",
				[]( const System & p_system )
				{
					const Index count = getTopology( p_system.entity ).getAtomCount();
					return count;
				}
			)
			.def_property_readonly(
				"indexFirstBond",
				[]( const System & p_system )
				{
					getTopology( p_system.entity );
					return Index( 0 );
				}
			)
			.def_property_readonly(
				"indexLastBond",
				[]( const System & p_system )
				{
					const Index count = getTopology( p_system.entity ).getBondCount();
					return count;
				}
			)
			.def( "getChains", &getChains )
			.def( "getResidues", &getResidues )
			.def( "getAtoms", &getAtoms )
			.def( "getBonds", &getBonds )
			.def( "getCategories", &getCategories )
			.def( "getChain", &System::getChain, py::arg( "index" ) )
			.def( "getResidue", &System::getResidue, py::arg( "index" ) )
			.def( "getAtom", &System::getAtom, py::arg( "index" ) )
			.def( "getBond", &System::getBond, py::arg( "index" ) )
			.def( "getCategory", &System::getCategory, py::arg( "index" ) )
			.def(
				"setVisible",
				[]( const System & p_system, const bool p_visible )
				{ setVisible<SystemItem::SYSTEM>( p_system.entity, RangeList(), p_visible ); },
				py::arg( "visible" ) = true
			)
			.def(
				"setSelected",
				[]( const System & p_system, const bool p_selected, const bool p_append )
				{ setSelected<SystemItem::SYSTEM>( p_system.entity, RangeList(), p_selected, p_append ); },
				py::arg( "selected" ) = true,
				py::arg( "append" )	  = false
			)
			.def_property_readonly(
				"visibleState",
				[]( const System & p_system )
				{ return getVisibleState( p_system.entity, SystemItem::SYSTEM, std::nullopt ); }
			)
			.def_property_readonly(
				"selectionState",
				[]( const System & p_system )
				{ return getSelectionState( p_system.entity, SystemItem::SYSTEM, std::nullopt ); }
			)
			.def(
				"isVisible",
				[]( const System & p_system ) { return isVisible( p_system.entity, SystemItem::SYSTEM, std::nullopt ); }
			)
			.def(
				"isFullyVisible",
				[]( const System & p_system )
				{ return isFullyVisible( p_system.entity, SystemItem::SYSTEM, std::nullopt ); }
			)
			.def(
				"isSelected",
				[]( const System & p_system )
				{ return isSelected( p_system.entity, SystemItem::SYSTEM, std::nullopt ); }
			)
			.def(
				"isFullySelected",
				[]( const System & p_system )
				{ return isFullySelected( p_system.entity, SystemItem::SYSTEM, std::nullopt ); }
			);

		p_vtxModule.def(
			"getSystem",
			&getSystem,
			"Return a topology system handle matching given name.",
			VTX::PythonBinding::Wrapper::Arg( "name" )
		);
	}
} // namespace VTX::App::PythonBinding::Topology
