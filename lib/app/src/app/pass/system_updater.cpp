#include "app/system/load.hpp"
// Forward decl
#include "app/events.hpp"
#include "app/pass/system_updater.hpp"
#include "app/services.hpp"
#include "app/system/color.hpp"
#include "app/system/representation.hpp"
#include "app/system/selection.hpp"
#include "app/system/trajectory.hpp"
#include "app/system/uid.hpp"
#include "app/system/visibility.hpp"
#include "app/threading/thread_manager.hpp"
#include <renderer/renderer.hpp>
#include <util/chrono.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Pass
{

	SystemUpdater::SystemUpdater()
	{
		auto & reg = REG();

		reg.on_update<Util::Math::Transform>().connect<&SystemUpdater::_onUpdateTransform>( this );
		reg.on_update<System::Visibility>().connect<&SystemUpdater::_onUpdateVisibility>( this );
		reg.on_update<System::Selection>().connect<&SystemUpdater::_onUpdateSelection>( this );
		reg.on_update<System::Representation>().connect<&SystemUpdater::_onUpdateRepresentation>( this );
		reg.on_update<System::Color>().connect<&SystemUpdater::_onUpdateColor>( this );

		reg.on_update<Renderer::Representation>().connect<&SystemUpdater::_onUpdateRepresentationPreset>( this );
		reg.on_destroy<System::TrajectoryFullBuffer>().connect<&SystemUpdater::_onTrajectoryDestruction>( this );

		HUB().connect<Events::SystemLoad, &SystemUpdater::_onSystemLoaded>( this );
	}

	void SystemUpdater::update( const float p_delta, const float p_total )
	{
		_pendingSystemUpdate();
		return;
		for ( auto & entity : _entities )
		{
			REG().patch<Util::Math::Transform>(
				entity,
				[ p_delta ]( Util::Math::Transform & p_transform ) { p_transform.rotateYaw( p_delta * 0.001f ); }
			);
		}
	}
	void SystemUpdater::_pendingSystemUpdate() noexcept
	{
		for ( auto it_entity : REG().view<System::PendingSystem>() )
		{
			auto & pendingSystem = REG().get<System::PendingSystem>( it_entity );
			if ( pendingSystem.readyToDeliver )
			{
				System::deliver( it_entity, pendingSystem );
				continue;
			}
		}
	}

	void SystemUpdater::_onUpdateTransform( ECS::Registry & p_r, ECS::Entity p_e )
	{
		if ( std::find( _entities.begin(), _entities.end(), p_e ) != _entities.end() )
		{
			const auto & [ transform, uid ] = p_r.get<Util::Math::Transform, System::UID>( p_e );
			RENDERER().setSystemTransform( uid.system, transform.computeMatrix() );
		}
	}

	void SystemUpdater::_onSystemLoaded( const Events::SystemLoad & p_event )
	{
		Util::ScopedChrono timer( "_onSystemLoaded" );
		auto &			   reg	  = REG();
		const ECS::Entity  system = p_event.system;

		timer.start();

		assert( std::find( _entities.begin(), _entities.end(), system ) == _entities.end() );

		_entities.push_back( system );

		std::vector<Renderer::SystemData> systemsData;
		_representations.clear();

		for ( const ECS::Entity system : _entities )
		{
			const auto & data			= reg.get<Core::Struct::System>( system );
			const auto & transform		= reg.get<Util::Math::Transform>( system );
			const auto & uid			= reg.get<System::UID>( system );
			const auto & color			= reg.get<System::Color>( system );
			const auto & representation = reg.get<System::Representation>( system );
			const auto & visibility		= reg.get<System::Visibility>( system );
			const auto & selection		= reg.get<System::Selection>( system );
			const size_t atomCount		= data.getAtomCount();

			assert( atomCount > 0 );

			std::vector<float> radii( atomCount );
			for ( Index i = 0; i < atomCount; ++i )
			{
				// TODO: use glsl constants.
				radii[ i ] = Core::ChemDB::Atom::SYMBOL_VDW_RADIUS[ toUnderlying( data.getAtomSymbol( i ) ) ];
			}

			std::span<const Vec3f> positions = System::getCurrentAtomPositions( system );
			systemsData.push_back(
				Renderer::SystemData { uid.system,
									   transform.computeMatrix(),
									   data,
									   positions,
									   radii,
									   uid.atoms.toStdVector(),
									   uid.residues.toStdVector() }
			);
		}

		// Push systems.
		RENDERER().setSystems( systemsData );

		// Push data.
		for ( const ECS::Entity e : _entities )
		{
			_onUpdateVisibility( reg, e );
			_onUpdateSelection( reg, e );
			_onUpdateColor( reg, e );
			_onUpdateRepresentation( reg, e );
		}
	}

	void SystemUpdater::_onUpdateVisibility( ECS::Registry & p_r, ECS::Entity p_e )
	{
		const auto & [ visibility, uid ] = p_r.get<System::Visibility, System::UID>( p_e );

		RENDERER().setSystemVisibility( uid.system, visibility.atoms );
	}

	void SystemUpdater::_onUpdateSelection( ECS::Registry & p_r, ECS::Entity p_e )
	{
		const auto & [ selection, uid ] = p_r.get<System::Selection, System::UID>( p_e );

		RENDERER().setSystemSelection( uid.system, selection.atoms );
	}

	void SystemUpdater::_onUpdateRepresentation( ECS::Registry & p_r, ECS::Entity p_e )
	{
		using namespace Renderer;

		const auto & [ representation, uid, data ]
			= p_r.get<System::Representation, System::UID, Core::Struct::System>( p_e );

		MapRepresentationRanges mapAtoms;

		bool newRepresentation = false;
		for ( const auto & [ entity, ranges ] : representation.presetAtoms )
		{
			if ( not _representations.contains( entity ) )
			{
				_representations[ entity ] = static_cast<RepresentationIndex>( _representations.size() );
				newRepresentation		   = true;
			}
			mapAtoms.emplace( _representations[ entity ], ranges );
		}

		if ( newRepresentation )
		{
			_setRepresentation();
		}

		RENDERER().setSystemRepresentation( uid.system, mapAtoms );
	}

	void SystemUpdater::_onUpdateColor( ECS::Registry & p_r, ECS::Entity p_e )
	{
		const auto & [ color, uid, data ] = p_r.get<System::Color, System::UID, Core::Struct::System>( p_e );

		std::vector<Renderer::ColorIndex> atoms( data.getAtomCount() );
		size_t							  count = 0;

		for ( const auto & [ scheme, ranges ] : color.colorSchemeAtoms )
		{
			if ( scheme == System::E_COLOR_SCHEME::ATOM )
			{
				for ( Index atom : ranges )
				{
					atoms[ atom ] = Renderer::Color::getColorIndex( data.getAtomSymbol( atom ) );
				}
			}
			else if ( scheme == System::E_COLOR_SCHEME::RESIDUE )
			{
				for ( Index atom : ranges )
				{
					const Index residue = data.atomResidueIndexes[ atom ];
					atoms[ atom ]		= Renderer::Color::getColorIndex( data.getResidueSymbol( residue ) );
				}
			}
			else if ( scheme == System::E_COLOR_SCHEME::CHAIN )
			{
				for ( Index atom : ranges )
				{
					const Index chain = data.getAtomChainIndex( atom );
					atoms[ atom ]	  = Renderer::Color::getColorIndex( data.getChainName( chain ) );
				}
			}
			else
			{
				assert( false && "Unsupported System::E_COLOR_SCHEME type in ColorScheme::Add action." );
			}
			count += ranges.count();
		}

		assert( count == data.getAtomCount() );

		RENDERER().setSystemColors( uid.system, atoms );
	}

	void SystemUpdater::_onUpdateRepresentationPreset( ECS::Registry & p_r, ECS::Entity p_e )
	{
		// Check if entity used.
		const auto it = std::find_if(
			_representations.begin(), _representations.end(), [ p_e ]( const auto & pair ) { return pair.first == p_e; }
		);

		if ( it != _representations.end() )
		{
			_setRepresentation();
		}
	}

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
	void SystemUpdater::_onTrajectoryDestruction( ECS::Registry &, ECS::Entity p_entity )
	{
		if ( auto traj = REG().try_get<System::TrajectoryFullBuffer>( p_entity ) )
		{ // If the trajectory worker is still doing stuff, stop it and join the thread before destroying the component.
			Threading::BaseThread * thr = nullptr;
			THREAD().get( traj->threadId, thr );
			if ( thr )
			{
				thr->stop();
				thr->wait();
			}
		}
	}

} // namespace VTX::App::Pass
