#include "app/pass/system_updater.hpp"
#include "app/events.hpp"
#include "app/services.hpp"
#include "app/system/color.hpp"
#include "app/system/load.hpp"
#include "app/system/representation.hpp"
#include "app/system/selection.hpp"
#include "app/system/trajectory.hpp"
#include "app/system/uid.hpp"
#include "app/system/visibility.hpp"
#include <renderer/renderer.hpp>
#include <util/chrono.hpp>
#include <util/math/transform.hpp>
#include <util/types.hpp>

namespace VTX::App::Pass
{

	SystemUpdater::SystemUpdater()
	{
		auto & reg = REG();

		HUB().connect<Events::SystemLoad, &SystemUpdater::_onSystemLoad>( this );
		reg.on_construct<Renderer::Representation>().connect<&SystemUpdater::_onConstructRepresentationPreset>( this );

		reg.on_update<Util::Math::Transform>().connect<&SystemUpdater::_onUpdateTransform>( this );
		reg.on_update<System::Visibility>().connect<&SystemUpdater::_onUpdateVisibility>( this );
		reg.on_update<System::Selection>().connect<&SystemUpdater::_onUpdateSelection>( this );
		reg.on_update<System::Representation>().connect<&SystemUpdater::_onUpdateRepresentation>( this );
		reg.on_update<System::Color>().connect<&SystemUpdater::_onUpdateColor>( this );

		reg.on_update<Renderer::Representation>().connect<&SystemUpdater::_onUpdateRepresentationPreset>( this );

		reg.on_destroy<System::UID>().connect<&SystemUpdater::_onDestroySystem>( this );
	}

	void SystemUpdater::_onUpdateTransform( Registry & p_r, Entity p_e )
	{
		if ( _systems.contains( p_e ) )
		{
			const auto & transform = p_r.get<Util::Math::Transform>( p_e );
			RENDERER().setSystemTransform( _systems[ p_e ], transform.computeMatrix() );
		}
	}

	void SystemUpdater::_onSystemLoad( const Events::SystemLoad & p_event )
	{
		Util::ScopedChrono timer( "_onSystemLoaded" );
		auto &			   reg	  = REG();
		const Entity	   system = p_event.system;

		assert( not _systems.contains( system ) );

		const auto & topology		= reg.get<Core::Struct::Topology>( system );
		const auto & transform		= reg.get<Util::Math::Transform>( system );
		const auto & uid			= reg.get<System::UID>( system );
		const auto & color			= reg.get<System::Color>( system );
		const auto & representation = reg.get<System::Representation>( system );
		const auto & visibility		= reg.get<System::Visibility>( system );
		const auto & selection		= reg.get<System::Selection>( system );

		std::span<const Vec3f> positions = System::getCurrentAtomPositions( system );

		assert( topology.getAtomCount() > 0 );

		const Renderer::Desc::Handle systemHandle = RENDERER().addSystem(
			Renderer::Cache::System { transform.computeMatrix(),
									  topology,
									  positions,
									  uid.atoms,
									  uid.residues,
									  color.colorSchemeAtoms,
									  color.customColorAtoms,
									  representation.presetAtoms,
									  visibility.atoms,
									  selection.atoms }
		);

		_systems.emplace( system, systemHandle );
	}

	void SystemUpdater::_onDestroySystem( Registry &, Entity p_e )
	{
		assert( _systems.contains( p_e ) );

		RENDERER().removeSystem( _systems[ p_e ] );
		_systems.erase( p_e );
	}

	void SystemUpdater::_onUpdateVisibility( Registry & p_r, Entity p_e )
	{
		assert( _systems.contains( p_e ) );

		RENDERER().setSystemDirty( _systems[ p_e ], Renderer::Cache::E_SYSTEM_DIRTY::VISIBILITY );
	}

	void SystemUpdater::_onUpdateSelection( Registry & p_r, Entity p_e )
	{
		assert( _systems.contains( p_e ) );

		RENDERER().setSystemDirty( _systems[ p_e ], Renderer::Cache::E_SYSTEM_DIRTY::SELECTION );
	}

	void SystemUpdater::_onUpdateRepresentation( Registry & p_r, Entity p_e )
	{
		assert( _systems.contains( p_e ) );

		RENDERER().setSystemDirty( _systems[ p_e ], Renderer::Cache::E_SYSTEM_DIRTY::REPRESENTATION );
	}

	void SystemUpdater::_onUpdateColor( Registry & p_r, Entity p_e )
	{
		assert( _systems.contains( p_e ) );

		RENDERER().setSystemDirty( _systems[ p_e ], Renderer::Cache::E_SYSTEM_DIRTY::COLOR );
	}

	void SystemUpdater::_onConstructRepresentationPreset( Registry &, Entity p_e )
	{
		assert( not _representations.contains( p_e ) );

		const auto & rep = REG().get<Renderer::Representation>( p_e );
		_representations.emplace( p_e, RENDERER().addRepresentation( rep ) );
	}

	void SystemUpdater::_onUpdateRepresentationPreset( Registry & p_r, Entity p_e )
	{
		// Check if entity used.
		const auto it = std::find_if(
			_representations.begin(), _representations.end(), [ p_e ]( const auto & pair ) { return pair.first == p_e; }
		);

		if ( it != _representations.end() )
		{
			//_setRepresentation();
		}
	}

	void SystemUpdater::_onDestroyRepresentationPreset( Registry &, Entity p_e )
	{
		assert( _representations.contains( p_e ) );

		RENDERER().removeRepresentation( _representations[ p_e ] );
		_representations.erase( p_e );
	}

	/*
	void SystemUpdater::_setRepresentation()
	{
		std::vector<const Renderer::Representation *> representations( _representations.size() );

		for ( auto & [ ent, index ] : _representations )
		{
			const auto * const rep = REG().try_get<Renderer::Representation>( ent );

			assert( rep );

			representations[ index ] = rep;
		}

		RENDERER().setRepresentations( representations );
	}

	*/

} // namespace VTX::App::Pass
