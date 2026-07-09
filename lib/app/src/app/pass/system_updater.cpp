#include "app/pass/system_updater.hpp"
#include "app/events.hpp"
#include "app/services.hpp"
#include "app/system/color.hpp"
#include "app/system/representation.hpp"
#include "app/system/selection.hpp"
#include "app/system/trajectory.hpp"
#include "app/system/uid.hpp"
#include "app/system/visibility.hpp"
#include <algorithm>
#include <core/chemdb/atom.hpp>
#include <renderer/renderer.hpp>
#include <span>
#include <util/chrono.hpp>
#include <util/math/aabb.hpp>
#include <util/math/transform.hpp>
#include <util/types.hpp>

namespace VTX::App::Pass
{
	namespace
	{
		void _patchAABB( const Entity & p_entity, const std::span<const Vec3f> p_positions ) noexcept
		{
			REG().patch<Util::Math::AABB>(
				p_entity,
				[ p_positions ]( Util::Math::AABB & p_aabb )
				{
					p_aabb.invalidate();
					for ( const Vec3f & atomPosition : p_positions )
					{
						p_aabb.extend( atomPosition, Core::ChemDB::Atom::VDW_RADIUS_MIN );
					}
				}
			);
		}

	} // namespace

	SystemUpdater::SystemUpdater()
	{
		auto & reg = REG();
		auto & hub = HUB();

		// System.
		hub.connect<Events::SystemLoad, &SystemUpdater::_onSystemLoad>( this );
		hub.connect<Events::TrajectoryLoad, &SystemUpdater::_onTrajectoryLoad>( this );
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

	void SystemUpdater::update( const float, const float )
	{
		auto & reg = REG();

		const bool systemChanged		 = not _systemAdded.empty() || not _systemRemoved.empty();
		const bool representationChanged = not _representationAdded.empty() || not _representationRemoved.empty();

		const auto getSystemData = [ & ]( const Entity p_ent ) -> Renderer::Cache::System::Data
		{
			const auto & topology		= reg.get<Core::Struct::Topology>( p_ent );
			const auto & uid			= reg.get<System::UID>( p_ent );
			const auto & color			= reg.get<System::Color>( p_ent );
			const auto & representation = reg.get<System::Representation>( p_ent );
			const auto & visibility		= reg.get<System::Visibility>( p_ent );
			const auto & selection		= reg.get<System::Selection>( p_ent );

			std::span<const Vec3f> positions = System::getCurrentAtomPositions( p_ent );
			assert( topology.getAtomCount() > 0 );

			return { &topology,
					 positions,
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
		};

		// Add pending.
		for ( const auto system : _systemAdded )
		{
			assert( not _systems.contains( system ) );

			const auto & transform = reg.get<Util::Math::Transform>( system );

			const Renderer::Desc::Handle systemHandle
				= RENDERER().addSystem( { transform.computeMatrix(), getSystemData( system ) } );

			_systems.emplace( system, systemHandle );
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
				RENDERER().patchSystem( pair.second, getSystemData( pair.first ) );
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

		_patchAABB( p_event.system, System::getCurrentAtomPositions( p_event.system ) );
		_systemAdded.emplace_back( p_event.system );
	}

	void SystemUpdater::_onTrajectoryLoad( const Events::TrajectoryLoad & p_event )
	{
		assert( _systems.contains( p_event.system ) );

		_patchAABB( p_event.system, p_event.frame );
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
