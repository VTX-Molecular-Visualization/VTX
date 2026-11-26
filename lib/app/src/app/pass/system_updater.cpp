#include "app/pass/system_updater.hpp"
#include "app/events.hpp"
#include "app/helper/aabb.hpp"
#include "app/helper/system.hpp"
#include "app/helper/trajectory.hpp"
#include "app/pending_updates.hpp"
#include "app/services.hpp"
#include "app/system/color.hpp"
#include "app/system/representation.hpp"
#include "app/system/selection.hpp"
#include "app/system/uid.hpp"
#include "app/system/visibility.hpp"
#include <renderer/renderer.hpp>
#include <util/exceptions.hpp>
#include <util/math/transform.hpp>
#include <utility>

namespace VTX::App::Pass
{
	SystemUpdater::SystemUpdater()
	{
		auto & reg = REG();
		auto & hub = HUB();

		// System.
		_systemLoadConnection = hub.connect<Events::SystemLoad, &SystemUpdater::_onSystemLoad>( this );
		_trajectoryFrameChangedConnection
			= hub.connect<Events::TrajectoryCurrentFrameChange, &SystemUpdater::_onTrajectoryCurrentFrameChange>(
				this
			);
		_topologyUpdatedConnection
			= reg.on_update<Core::Struct::Topology>().connect<&SystemUpdater::_onUpdateTopology>( this );
		_transformUpdatedConnection
			= reg.on_update<Util::Math::Transform>().connect<&SystemUpdater::_onUpdateTransform>( this );
		_visibilityUpdatedConnection
			= reg.on_update<System::Visibility>().connect<&SystemUpdater::_onUpdateVisibility>( this );
		_selectionUpdatedConnection
			= reg.on_update<System::Selection>().connect<&SystemUpdater::_onUpdateSelection>( this );
		_representationUpdatedConnection
			= reg.on_update<System::Representation>().connect<&SystemUpdater::_onUpdateRepresentation>( this );
		_colorUpdatedConnection = reg.on_update<System::Color>().connect<&SystemUpdater::_onUpdateColor>( this );
		_systemDestroyedConnection
			= reg.on_destroy<Core::Struct::Topology>().connect<&SystemUpdater::_onDestroySystem>( this );

		// Representation preset.
		_representationPresetConstructedConnection
			= reg.on_construct<Renderer::Representation>().connect<&SystemUpdater::_onConstructRepresentationPreset>(
				this
			);
		_representationPresetUpdatedConnection
			= reg.on_update<Renderer::Representation>().connect<&SystemUpdater::_onUpdateRepresentationPreset>( this );
		_representationPresetDestroyedConnection
			= reg.on_destroy<Renderer::Representation>().connect<&SystemUpdater::_onDestroyRepresentationPreset>(
				this
			);
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

		Util::ExceptionCollector errors;

		// Add pending.
		while ( not _systemAdded.empty() )
		{
			const Entity system = _systemAdded.front();
			errors.run(
				"System add failed",
				[ & ]()
				{
					assert( not _systems.contains( system ) );

					const auto & transform = reg.get<Util::Math::Transform>( system );

					[[maybe_unused]] const bool currentFrameVisited = Helper::Trajectory::visitCurrentFrame(
						system,
						[ this, system, &transform ]( const Core::Struct::FrameView p_positions )
						{
							_systems.emplace(
								system,
								RENDERER().addSystem(
									{ transform.computeMatrix(), _getSystemData( system, p_positions ) }
								)
							);
						}
					);
					assert( currentFrameVisited );
				}
			);
			_systemAdded.erase( _systemAdded.begin() );
		}
		while ( not _representationAdded.empty() )
		{
			const Entity representation = _representationAdded.front();
			errors.run(
				"Representation add failed",
				[ & ]()
				{
					assert( not _representations.contains( representation ) );

					const auto & rep = REG().get<Renderer::Representation>( representation );
					_representations.emplace( representation, RENDERER().addRepresentation( rep ) );
				}
			);
			_representationAdded.erase( _representationAdded.begin() );
		}

		// Remove pending.
		while ( not _systemRemoved.empty() )
		{
			errors.run( "System removal failed", [ & ]() { RENDERER().removeSystem( _systemRemoved.front() ); } );
			_systemRemoved.erase( _systemRemoved.begin() );
		}
		while ( not _representationRemoved.empty() )
		{
			errors.run(
				"Representation removal failed",
				[ & ]() { RENDERER().removeRepresentation( _representationRemoved.front() ); }
			);
			_representationRemoved.erase( _representationRemoved.begin() );
		}

		errors.run(
			"System patch failed",
			[ & ]()
			{
				// Patch because renderer use views to raw data.
				// entt components are not guaranteed to be contiguous in memory.
				// So ptr can dangle after add/remove.
				if ( systemChanged )
				{
					for ( const auto & pair : _systems )
					{
						Helper::Trajectory::visitCurrentFrame(
							pair.first,
							[ this, entity = pair.first, handle = pair.second ](
								const Core::Struct::FrameView p_positions
							) { RENDERER().patchSystem( handle, _getSystemData( entity, p_positions ) ); }
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
			}
		);

		// Clear pending.
		_systemAdded.clear();
		_systemRemoved.clear();
		_representationAdded.clear();
		_representationRemoved.clear();

		errors.rethrow();
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

		Helper::AABB::invalidateScene();
		_systemAdded.emplace_back( p_event.system );
		PENDING_UPDATES().mark( E_PENDING_UPDATE::PASSES );
	}

	void SystemUpdater::_onTrajectoryCurrentFrameChange( const Events::TrajectoryCurrentFrameChange & p_event )
	{
		const auto systemIt = _systems.find( p_event.system );
		assert( systemIt != _systems.end() || std::ranges::find( _systemAdded, p_event.system ) != _systemAdded.end() );

		Helper::AABB::invalidate( p_event.system );
		Helper::System::invalidateAtomGrid( p_event.system );
		if ( systemIt != _systems.end() )
		{
			RENDERER().setSystemPositions( systemIt->second, p_event.frame );
		}
	}

	void SystemUpdater::_onDestroySystem( Registry &, const Entity p_e )
	{
		if ( std::erase( _systemAdded, p_e ) > 0 )
		{
			return;
		}

		const auto it = _systems.find( p_e );
		if ( it == _systems.end() )
		{
			return;
		}

		_systemRemoved.emplace_back( it->second );
		PENDING_UPDATES().mark( E_PENDING_UPDATE::PASSES );
		_systems.erase( it );
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
		PENDING_UPDATES().mark( E_PENDING_UPDATE::PASSES );
	}

	void SystemUpdater::_onUpdateRepresentationPreset( Registry &, const Entity p_e )
	{
		if ( const auto it = _representations.find( p_e ); it != _representations.end() )
		{
			RENDERER().setRepresentationDirty( it->second, Renderer::Cache::E_REPRESENTATION_DIRTY::ALL );
		}
		else
		{
			assert( std::ranges::find( _representationAdded, p_e ) != _representationAdded.end() );
		}
	}

	void SystemUpdater::_onDestroyRepresentationPreset( Registry &, const Entity p_e )
	{
		if ( std::erase( _representationAdded, p_e ) > 0 )
		{
			return;
		}

		const auto it = _representations.find( p_e );
		if ( it == _representations.end() )
		{
			return;
		}

		_representationRemoved.emplace_back( it->second );
		PENDING_UPDATES().mark( E_PENDING_UPDATE::PASSES );
		_representations.erase( it );
	}

} // namespace VTX::App::Pass
