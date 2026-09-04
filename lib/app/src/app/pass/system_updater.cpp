#include "app/pass/system_updater.hpp"
#include "app/events.hpp"
#include "app/helper/trajectory.hpp"
#include "app/services.hpp"
#include "app/system/color.hpp"
#include "app/system/representation.hpp"
#include "app/system/selection.hpp"
#include "app/system/uid.hpp"
#include "app/system/visibility.hpp"
#include <algorithm>
#include <core/chemdb/atom.hpp>
#include <renderer/renderer.hpp>
#include <util/chrono.hpp>
#include <util/math/aabb.hpp>
#include <util/math/grid.hpp>
#include <util/math/transform.hpp>
#include <util/types.hpp>
#include <utility>

namespace VTX::App::Pass
{
	SystemUpdater::SystemUpdater()
	{
		auto & reg = REG();
		auto & hub = HUB();

		// System.
		hub.connect<Events::SystemLoad, &SystemUpdater::_onSystemLoad>( this );
		hub.connect<Events::TrajectoryCurrentFrameChange, &SystemUpdater::_onTrajectoryCurrentFrameChange>( this );
		reg.on_update<Core::Struct::Topology>().connect<&SystemUpdater::_onUpdateTopology>( this );
		reg.on_update<Util::Math::Transform>().connect<&SystemUpdater::_onUpdateTransform>( this );
		reg.on_update<System::Visibility>().connect<&SystemUpdater::_onUpdateVisibility>( this );
		reg.on_update<System::Selection>().connect<&SystemUpdater::_onUpdateSelection>( this );
		reg.on_update<System::Representation>().connect<&SystemUpdater::_onUpdateRepresentation>( this );
		reg.on_update<System::Color>().connect<&SystemUpdater::_onUpdateColor>( this );
		reg.on_destroy<Core::Struct::Topology>().connect<&SystemUpdater::_onDestroySystem>( this );

		// Representation preset.
		reg.on_construct<Renderer::Representation>().connect<&SystemUpdater::_onConstructRepresentationPreset>( this );
		reg.on_update<Renderer::Representation>().connect<&SystemUpdater::_onUpdateRepresentationPreset>( this );
		reg.on_destroy<Renderer::Representation>().connect<&SystemUpdater::_onDestroyRepresentationPreset>( this );
	}

	Renderer::Cache::System::Data SystemUpdater::_getSystemData(
		const Entity				  p_entity,
		const Core::Struct::FrameView p_positions
	) const
	{
		const auto & topology		= REG().get<Core::Struct::Topology>( p_entity );
		const auto & uid			= REG().get<System::UID>( p_entity );
		const auto & color			= REG().get<System::Color>( p_entity );
		const auto & representation = REG().get<System::Representation>( p_entity );
		const auto & visibility		= REG().get<System::Visibility>( p_entity );
		const auto & selection		= REG().get<System::Selection>( p_entity );

		assert( topology.getAtomCount() > 0 );

		return { &topology,
				 p_positions,
				 &uid.atoms,
				 &uid.residues,
				 &color.colorSchemeAtoms,
				 &color.customColorAtoms,
				 &color.carbonCustomColorAtoms,
				 &color.colorSchemeSecondaryStructureResidues,
				 &color.customSecondaryStructureColorResidues,
				 &_representations,
				 &representation.presetAtoms,
				 &visibility.atoms,
				 &selection.atoms };
	}

	void SystemUpdater::update( const float, const float )
	{
		auto & reg = REG();

		const bool systemChanged		 = not _systemAdded.empty() || not _systemRemoved.empty();
		const bool representationChanged = not _representationAdded.empty() || not _representationRemoved.empty();

		// Add pending.
		for ( const auto system : _systemAdded )
		{
			assert( not _systems.contains( system ) );

			const auto & transform = reg.get<Util::Math::Transform>( system );

			const bool currentFrameVisited = Helper::Trajectory::visitCurrentFrame(
				system,
				[ this, system, &transform ]( const Core::Struct::FrameView p_positions )
				{
					_systems.emplace(
						system,
						RENDERER().addSystem( { transform.computeMatrix(), _getSystemData( system, p_positions ) } )
					);
				}
			);
			assert( currentFrameVisited );
		}
		for ( const auto representation : _representationAdded )
		{
			assert( not _representations.contains( representation ) );

			const auto & rep = REG().get<Renderer::Representation>( representation );
			_representations.emplace( representation, RENDERER().addRepresentation( rep ) );
		}

		// Remove pending.
		for ( const auto system : _systemRemoved )
		{
			RENDERER().removeSystem( system );
		}
		for ( const auto representation : _representationRemoved )
		{
			RENDERER().removeRepresentation( representation );
		}

		// Patch because renderer use views to raw data.
		// entt components are not guaranteed to be contiguous in memory.
		// So ptr can dangle after add/remove.
		if ( systemChanged )
		{
			for ( const auto & pair : _systems )
			{
				Helper::Trajectory::visitCurrentFrame(
					pair.first,
					[ this, entity = pair.first, handle = pair.second ]( const Core::Struct::FrameView p_positions )
					{ RENDERER().patchSystem( handle, _getSystemData( entity, p_positions ) ); }
				);
			}
		}
		if ( representationChanged )
		{
			for ( const auto & pair : _representations )
			{
				const auto & rep = reg.get<Renderer::Representation>( pair.first );
				RENDERER().patchRepresentation( pair.second, { &rep } );
			}
		}

		// Clear pending.
		_systemAdded.clear();
		_systemRemoved.clear();
		_representationAdded.clear();
		_representationRemoved.clear();
	}

	void SystemUpdater::_onUpdateTransform( Registry & p_r, Entity p_e )
	{
		// Filter entities that are not systems (can be optimized with custom event).
		if ( _systems.contains( p_e ) )
		{
			const auto & transform = p_r.get<Util::Math::Transform>( p_e );
			RENDERER().setSystemTransform( _systems[ p_e ], transform.computeMatrix() );
		}
	}

	void SystemUpdater::_onUpdateTopology( Registry &, Entity p_e )
	{
		if ( _systems.contains( p_e ) )
		{
			RENDERER().setSystemDirty( _systems[ p_e ], Renderer::Cache::E_SYSTEM_DIRTY::STRUCTURE );
		}
	}

	void SystemUpdater::_onSystemLoad( const Events::SystemLoad & p_event )
	{
		assert( not _systems.contains( p_event.system ) );

		REG().patch<Util::Math::AABB>( p_event.system, []( Util::Math::AABB & _ ) {} );
		_systemAdded.emplace_back( p_event.system );
	}

	void SystemUpdater::_onTrajectoryCurrentFrameChange( const Events::TrajectoryCurrentFrameChange & p_event )
	{
		assert( _systems.contains( p_event.system ) );

		Util::Math::AABB		aabb;
		Util::Math::Grid<Index> atomGrid;
		for ( Index atomIndex = 0; atomIndex < static_cast<Index>( p_event.frame.size() ); ++atomIndex )
		{
			const Vec3f & position = p_event.frame[ atomIndex ];
			aabb.extend( position, Core::ChemDB::Atom::VDW_RADIUS_MIN );
			atomGrid.add( atomIndex, position );
		}

		REG().patch<Util::Math::Grid<Index>>(
			p_event.system,
			[ &atomGrid ]( Util::Math::Grid<Index> & p_atomGrid ) { p_atomGrid = std::move( atomGrid ); }
		);
		REG().patch<Util::Math::AABB>(
			p_event.system, [ &aabb ]( Util::Math::AABB & p_aabb ) { p_aabb = std::move( aabb ); }
		);
		RENDERER().setSystemPositions( _systems[ p_event.system ], p_event.frame );
	}

	void SystemUpdater::_onDestroySystem( Registry &, Entity p_e )
	{
		assert( _systems.contains( p_e ) );

		_systemRemoved.emplace_back( _systems[ p_e ] );
		_systems.erase( p_e );
	}

	void SystemUpdater::_onUpdateVisibility( Registry & p_r, Entity p_e )
	{
		if ( _systems.contains( p_e ) )
		{
			RENDERER().setSystemDirty( _systems[ p_e ], Renderer::Cache::E_SYSTEM_DIRTY::VISIBILITY );
		}
		else
		{
			assert( std::ranges::find( _systemAdded, p_e ) != _systemAdded.end() );
		}
	}

	void SystemUpdater::_onUpdateSelection( Registry & p_r, Entity p_e )
	{
		if ( _systems.contains( p_e ) )
		{
			RENDERER().setSystemDirty( _systems[ p_e ], Renderer::Cache::E_SYSTEM_DIRTY::SELECTION );
		}
		else
		{
			assert( std::ranges::find( _systemAdded, p_e ) != _systemAdded.end() );
		}
	}

	void SystemUpdater::_onUpdateRepresentation( Registry & p_r, Entity p_e )
	{
		if ( _systems.contains( p_e ) )
		{
			RENDERER().setSystemDirty( _systems[ p_e ], Renderer::Cache::E_SYSTEM_DIRTY::REPRESENTATION );
		}
		else
		{
			assert( std::ranges::find( _systemAdded, p_e ) != _systemAdded.end() );
		}
	}

	void SystemUpdater::_onUpdateColor( Registry & p_r, Entity p_e )
	{
		if ( _systems.contains( p_e ) )
		{
			RENDERER().setSystemDirty( _systems[ p_e ], Renderer::Cache::E_SYSTEM_DIRTY::COLOR );
		}
		else
		{
			assert( std::ranges::find( _systemAdded, p_e ) != _systemAdded.end() );
		}
	}

	void SystemUpdater::_onConstructRepresentationPreset( Registry &, Entity p_e )
	{
		assert( not _representations.contains( p_e ) );

		_representationAdded.emplace_back( p_e );
	}

	void SystemUpdater::_onUpdateRepresentationPreset( Registry & p_r, Entity p_e )
	{
		assert( _representations.contains( p_e ) );

		RENDERER().setRepresentationDirty( _representations[ p_e ], Renderer::Cache::E_REPRESENTATION_DIRTY::ALL );
	}

	void SystemUpdater::_onDestroyRepresentationPreset( Registry &, Entity p_e )
	{
		assert( _representations.contains( p_e ) );

		_representationRemoved.emplace_back( _representations[ p_e ] );
		_representations.erase( p_e );
	}

} // namespace VTX::App::Pass
