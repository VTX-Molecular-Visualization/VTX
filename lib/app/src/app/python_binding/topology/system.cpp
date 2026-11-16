#include "app/action/system.hpp"
#include "app/action/io.hpp"
#include "app/action/scene.hpp"
#include "app/action/selection.hpp"
#include "app/helper/aabb.hpp"
#include "app/python_binding/identity.hpp"
#include "app/python_binding/topology/actions.hpp"
#include "app/python_binding/topology/binding.hpp"
#include "app/python_binding/topology/helpers.hpp"
#include "app/python_binding/trajectory.hpp"
#include <core/chemdb/category.hpp>
#include <io/metadata.hpp>
#include <optional>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>
#include <python_binding/wrapper/arg.hpp>
#include <util/math/transform.hpp>

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

		RangeList selectIndices( const std::optional<RangeList> & p_indices, const Index p_count )
		{
			if ( not p_indices )
			{
				return RangeList( Range::fromFirstCount( 0, p_count ) );
			}
			if ( not p_indices->isEmpty() && p_indices->getLast() > p_count )
			{
				throw pybind11::index_error( "Index is outside of the collection." );
			}
			return *p_indices;
		}

		ChainCollection getChains( const System & p_system, const std::optional<RangeList> & p_indices )
		{
			const auto & topology = getTopology( p_system.entity );
			return { p_system.entity, selectIndices( p_indices, topology.getChainCount() ) };
		}

		ResidueCollection getResidues( const System & p_system, const std::optional<RangeList> & p_indices )
		{
			const auto & topology = getTopology( p_system.entity );
			return { p_system.entity, selectIndices( p_indices, topology.getResidueCount() ) };
		}

		AtomCollection getAtoms( const System & p_system, const std::optional<RangeList> & p_indices )
		{
			const auto & topology = getTopology( p_system.entity );
			return { p_system.entity, selectIndices( p_indices, topology.getAtomCount() ) };
		}

		BondCollection getBonds( const System & p_system, const std::optional<RangeList> & p_indices )
		{
			const auto & topology = getTopology( p_system.entity );
			return { p_system.entity, selectIndices( p_indices, topology.getBondCount() ) };
		}

		CategoryCollection getCategories( const System & p_system, const std::optional<RangeList> & p_indices )
		{
			getTopology( p_system.entity );
			return { p_system.entity, selectIndices( p_indices, toUnderlying( Core::ChemDB::Category::TYPE::COUNT ) ) };
		}

		const IO::Metadata & getMetadata( const Entity p_entity )
		{
			if ( not REG().all_of<IO::Metadata>( p_entity ) )
			{
				throw pybind11::value_error( "Invalid system metadata." );
			}

			return REG().get<IO::Metadata>( p_entity );
		}

		const Util::Math::Transform & getTransform( const Entity p_entity )
		{
			getTopology( p_entity );
			if ( not REG().all_of<Util::Math::Transform>( p_entity ) )
			{
				throw pybind11::value_error( "Invalid system transform." );
			}

			return REG().get<Util::Math::Transform>( p_entity );
		}

		VTX::App::PythonBinding::Trajectory getTrajectory( const System & p_system )
		{
			getTopology( p_system.entity );
			return { p_system.entity };
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
		bindIdentity(
			bindState<SystemItem::SYSTEM>( bindStyle(
				py::class_<System>( p_module, "System", py::module_local() ),
				[]( const System & p_system ) { return Target { p_system.entity, SystemItem::SYSTEM, RangeList() }; }
			) )
		)
			.def(
				"associateTrajectory",
				[]( const System & p_system, const FilePath & p_path )
				{
					validateSystem( p_system.entity );
					App::Action::IO::AssociateTrajectory().execute( p_path, p_system.entity );
				},
				"Load a trajectory for this system and wait until it is associated.",
				py::arg( "path" )
			)
			.def(
				"clearSelection",
				[]( const System & p_system )
				{
					executeSystemAction<App::Action::Selection::Clear>(
						p_system.entity, App::Action::Selection::Clear::E_MODE::THIS
					);
				},
				"Clear the selection of this system."
			)
			.def(
				"delete",
				[]( const System & p_system )
				{ executeSystemAction<App::Action::Scene::DeleteSystem>( p_system.entity ); },
				"Delete this system from the scene."
			)
			.def_property_readonly( "trajectory", synchronized( &getTrajectory ) )
			.def_property_readonly(
				"currentFrame",
				[]( const System & p_system ) { return PythonBinding::getCurrentFrame( p_system.entity ); }
			)
			.def_property_readonly(
				"name",
				synchronized(
					[]( const System & p_system ) -> const std::string & { return getMetadata( p_system.entity ).name; }
				)
			)
			.def(
				"setName",
				[]( const System & p_system, const std::string & p_name )
				{ executeSystemAction<App::Action::System::SetName>( p_system.entity, p_name ); },
				py::arg( "name" )
			)
			.def_property_readonly(
				"position",
				synchronized( []( const System & p_system ) { return getTransform( p_system.entity ).getPosition(); } )
			)
			.def_property_readonly(
				"rotation",
				synchronized( []( const System & p_system ) { return getTransform( p_system.entity ).getRotation(); } )
			)
			.def_property_readonly(
				"scale",
				synchronized( []( const System & p_system ) { return getTransform( p_system.entity ).getScale(); } )
			)
			.def(
				"setPosition",
				[]( const System & p_system, const Vec3f & p_position )
				{ executeSystemAction<App::Action::System::SetPosition>( p_system.entity, p_position ); },
				py::arg( "position" )
			)
			.def(
				"setRotation",
				[]( const System & p_system, const Quatf & p_rotation )
				{ executeSystemAction<App::Action::System::SetRotation>( p_system.entity, p_rotation ); },
				py::arg( "rotation" )
			)
			.def(
				"setScale",
				[]( const System & p_system, const Vec3f & p_scale )
				{ executeSystemAction<App::Action::System::SetScale>( p_system.entity, p_scale ); },
				py::arg( "scale" )
			)
			.def_property_readonly(
				"path",
				synchronized( []( const System & p_system ) { return getMetadata( p_system.entity ).path.string(); } )
			)
			.def_property_readonly(
				"pdbIdCode",
				synchronized(
					[]( const System & p_system ) -> const std::string &
					{ return getMetadata( p_system.entity ).pdbIDCode; }
				)
			)
			.def_property_readonly(
				"chainCount",
				synchronized( []( const System & p_system ) { return getTopology( p_system.entity ).getChainCount(); } )
			)
			.def_property_readonly(
				"residueCount",
				synchronized( []( const System & p_system )
							  { return getTopology( p_system.entity ).getResidueCount(); } )
			)
			.def_property_readonly(
				"atomCount",
				synchronized( []( const System & p_system ) { return getTopology( p_system.entity ).getAtomCount(); } )
			)
			.def_property_readonly(
				"bondCount",
				synchronized( []( const System & p_system ) { return getTopology( p_system.entity ).getBondCount(); } )
			)
			.def_property_readonly(
				"categoryCount",
				synchronized(
					[]( const System & p_system )
					{
						getTopology( p_system.entity );
						return toUnderlying( Core::ChemDB::Category::TYPE::COUNT );
					}
				)
			)
			.def_property_readonly(
				"aabb",
				synchronized(
					[]( const System & p_system )
					{
						getTopology( p_system.entity );
						return Helper::AABB::get( p_system.entity );
					}
				)
			)
			.def_property_readonly(
				"indexFirstChain",
				synchronized(
					[]( const System & p_system )
					{
						getTopology( p_system.entity );
						return static_cast<Index>( 0 );
					}
				)
			)
			.def_property_readonly(
				"indexLastChain",
				synchronized(
					[]( const System & p_system )
					{
						const Index count = getTopology( p_system.entity ).getChainCount();
						return count;
					}
				)
			)
			.def_property_readonly(
				"indexFirstResidue",
				synchronized(
					[]( const System & p_system )
					{
						getTopology( p_system.entity );
						return static_cast<Index>( 0 );
					}
				)
			)
			.def_property_readonly(
				"indexLastResidue",
				synchronized(
					[]( const System & p_system )
					{
						const Index count = getTopology( p_system.entity ).getResidueCount();
						return count;
					}
				)
			)
			.def_property_readonly(
				"indexFirstAtom",
				synchronized(
					[]( const System & p_system )
					{
						getTopology( p_system.entity );
						return static_cast<Index>( 0 );
					}
				)
			)
			.def_property_readonly(
				"indexLastAtom",
				synchronized(
					[]( const System & p_system )
					{
						const Index count = getTopology( p_system.entity ).getAtomCount();
						return count;
					}
				)
			)
			.def_property_readonly(
				"indexFirstBond",
				synchronized(
					[]( const System & p_system )
					{
						getTopology( p_system.entity );
						return static_cast<Index>( 0 );
					}
				)
			)
			.def_property_readonly(
				"indexLastBond",
				synchronized(
					[]( const System & p_system )
					{
						const Index count = getTopology( p_system.entity ).getBondCount();
						return count;
					}
				)
			)
			.def( "getChains", synchronized( &getChains ), py::arg( "indices" ) = py::none() )
			.def( "getResidues", synchronized( &getResidues ), py::arg( "indices" ) = py::none() )
			.def( "getAtoms", synchronized( &getAtoms ), py::arg( "indices" ) = py::none() )
			.def( "getBonds", synchronized( &getBonds ), py::arg( "indices" ) = py::none() )
			.def( "getCategories", synchronized( &getCategories ), py::arg( "indices" ) = py::none() )
			.def( "getChain", synchronized( &System::getChain ), py::arg( "index" ) )
			.def( "getResidue", synchronized( &System::getResidue ), py::arg( "index" ) )
			.def( "getAtom", synchronized( &System::getAtom ), py::arg( "index" ) )
			.def( "getBond", synchronized( &System::getBond ), py::arg( "index" ) )
			.def( "getCategory", synchronized( &System::getCategory ), py::arg( "index" ) );

		p_vtxModule.def(
			"getSystem",
			synchronized( &getSystem ),
			"Return a topology system handle matching given name.",
			VTX::PythonBinding::Wrapper::Arg( "name" )
		);
	}
} // namespace VTX::App::PythonBinding::Topology
