#include "app/helper/aabb.hpp"
#include "app/generic/stale.hpp"
#include "app/helper/trajectory.hpp"
#include "app/scene/tag_root.hpp"
#include <core/chemdb/atom.hpp>
#include <core/struct/mesh.hpp>
#include <cstddef>
#include <stdexcept>
#include <util/math/transform.hpp>

namespace VTX::App::Helper::AABB
{
	namespace
	{
		void _extend( Util::Math::AABB & p_aabb, const Core::Struct::FrameView p_positions, const std::size_t p_atom )
		{
			if ( p_atom >= p_positions.size() )
			{
				throw std::out_of_range( "Atom position is unavailable." );
			}

			p_aabb.extend( p_positions[ p_atom ], Core::ChemDB::Atom::VDW_RADIUS_MIN );
		}

		void _extend(
			Util::Math::AABB &				 p_aabb,
			const Core::Struct::FrameView	 p_positions,
			const Core::Struct::IndexRange & p_atoms
		)
		{
			for ( const Index atom : p_atoms )
			{
				_extend( p_aabb, p_positions, atom );
			}
		}

		void _extend(
			Util::Math::AABB &					 p_aabb,
			const Core::Struct::FrameView		 p_positions,
			const Core::Struct::IndexRangeList & p_atoms
		)
		{
			for ( const Index atom : p_atoms )
			{
				_extend( p_aabb, p_positions, atom );
			}
		}
	} // namespace

	Util::Math::AABB compute( const Core::Struct::FrameView p_positions ) noexcept
	{
		Util::Math::AABB aabb;
		for ( const Vec3f & position : p_positions )
		{
			aabb.extend( position, Core::ChemDB::Atom::VDW_RADIUS_MIN );
		}
		return aabb;
	}

	Util::Math::AABB get( const Entity p_entity )
	{
		Registry & registry = REG();
		if ( not registry.all_of<Util::Math::AABB>( p_entity ) )
		{
			throw std::runtime_error( "System AABB is unavailable." );
		}

		if ( registry.all_of<Generic::StaleAABB>( p_entity ) )
		{
			Util::Math::AABB aabb;
			if ( not App::Helper::Trajectory::visitCurrentFrame(
					 p_entity, [ &aabb ]( const Core::Struct::FrameView p_positions ) { aabb = compute( p_positions ); }
				 ) )
			{
				throw std::out_of_range( "Trajectory frame is unavailable." );
			}
			registry.replace<Util::Math::AABB>( p_entity, aabb );
			registry.remove<Generic::StaleAABB>( p_entity );
		}

		return registry.get<Util::Math::AABB>( p_entity );
	}

	Util::Math::AABB getScene()
	{
		Registry &	 registry = REG();
		const Entity scene	  = ECS::getFirstEntityOnlyWithComponents<App::Scene::TagRoot, Util::Math::AABB>();
		if ( registry.all_of<Generic::StaleAABB>( scene ) )
		{
			Util::Math::AABB sceneAABB;
			for ( const Entity system :
				  registry.view<Core::Struct::Topology, Util::Math::AABB, Util::Math::Transform>() )
			{
				sceneAABB.extend( get( system ).transformed( registry.get<Util::Math::Transform>( system ) ) );
			}
			for ( const Entity mesh : registry.view<Core::Struct::Mesh, Util::Math::AABB, Util::Math::Transform>() )
			{
				sceneAABB.extend( get( mesh ).transformed( registry.get<Util::Math::Transform>( mesh ) ) );
			}
			registry.replace<Util::Math::AABB>( scene, sceneAABB );
			registry.remove<Generic::StaleAABB>( scene );
		}

		return registry.get<Util::Math::AABB>( scene );
	}

	void invalidate( const Entity p_entity )
	{
		REG().emplace_or_replace<Generic::StaleAABB>( p_entity );
		invalidateScene();
	}

	void invalidateScene()
	{
		Registry & registry = REG();
		for ( const Entity scene : registry.view<App::Scene::TagRoot, Util::Math::AABB>() )
		{
			registry.emplace_or_replace<Generic::StaleAABB>( scene );
		}
	}

	Util::Math::AABB get( const Entity p_entity, const Util::Math::BitSet & p_atoms )
	{
		Util::Math::AABB aabb;
		if ( not App::Helper::Trajectory::visitCurrentFrame(
				 p_entity,
				 [ & ]( const Core::Struct::FrameView p_positions )
				 {
					 for ( const std::size_t atom : p_atoms )
					 {
						 _extend( aabb, p_positions, atom );
					 }
				 }
			 ) )
		{
			throw std::out_of_range( "Trajectory frame is unavailable." );
		}

		return aabb;
	}

	Util::Math::AABB get(
		const Entity						 p_entity,
		const Core::Struct::E_SYSTEM_ITEM	 p_item,
		const Core::Struct::IndexRangeList & p_ranges
	)
	{
		const Core::Struct::Topology & topology = REG().get<Core::Struct::Topology>( p_entity );
		Util::Math::AABB			   aabb;
		if ( not App::Helper::Trajectory::visitCurrentFrame(
				 p_entity,
				 [ & ]( const Core::Struct::FrameView p_positions )
				 {
					 switch ( p_item )
					 {
					 case Core::Struct::E_SYSTEM_ITEM::SYSTEM:
						 _extend( aabb, p_positions, topology.getAtomRange() );
						 break;
					 case Core::Struct::E_SYSTEM_ITEM::CATEGORY:
						 for ( const Index category : p_ranges )
						 {
							 _extend( aabb, p_positions, topology.getCategoryAtomRangeList( category ) );
						 }
						 break;
					 case Core::Struct::E_SYSTEM_ITEM::CHAIN:
						 for ( const Index chain : p_ranges )
						 {
							 _extend( aabb, p_positions, topology.getChainAtomRange( chain ) );
						 }
						 break;
					 case Core::Struct::E_SYSTEM_ITEM::RESIDUE:
						 for ( const Index residue : p_ranges )
						 {
							 _extend( aabb, p_positions, topology.getResidueAtomRange( residue ) );
						 }
						 break;
					 case Core::Struct::E_SYSTEM_ITEM::ATOM: _extend( aabb, p_positions, p_ranges ); break;
					 default: throw std::invalid_argument( "Unsupported system item." );
					 }
				 }
			 ) )
		{
			throw std::out_of_range( "Trajectory frame is unavailable." );
		}

		return aabb;
	}

	Util::Math::AABB getBonds( const Entity p_entity, const Core::Struct::IndexRangeList & p_ranges )
	{
		const Core::Struct::Topology & topology = REG().get<Core::Struct::Topology>( p_entity );
		Util::Math::AABB			   aabb;
		if ( not App::Helper::Trajectory::visitCurrentFrame(
				 p_entity,
				 [ & ]( const Core::Struct::FrameView p_positions )
				 {
					 for ( const Index bond : p_ranges )
					 {
						 _extend( aabb, p_positions, topology.getBondFirstAtom( bond ) );
						 _extend( aabb, p_positions, topology.getBondSecondAtom( bond ) );
					 }
				 }
			 ) )
		{
			throw std::out_of_range( "Trajectory frame is unavailable." );
		}

		return aabb;
	}
} // namespace VTX::App::Helper::AABB
