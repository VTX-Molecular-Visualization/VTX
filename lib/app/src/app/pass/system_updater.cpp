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
#include "renderer/types.hpp"
#include <renderer/renderer.hpp>
#include <util/chrono.hpp>
#include <util/math/transform.hpp>
#include <util/types.hpp>

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

		reg.on_destroy<Core::Struct::Topology>().connect<&SystemUpdater::_onSystemDestroyed>( this );
		HUB().connect<Events::SystemLoad, &SystemUpdater::_onSystemLoaded>( this );
	}

	void SystemUpdater::update( const float p_delta, const float p_total )
	{
		if ( _needPush )
		{
			_pushSystems();
			_needPush = false;
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
		_needPush = true;
	}

	void SystemUpdater::_onSystemDestroyed( ECS::Registry &, ECS::Entity p_e )
	{
		const auto pushedIt = _pushedSystemUids.find( p_e );
		if ( pushedIt != _pushedSystemUids.end() )
		{
			RENDERER().removeSystem( pushedIt->second );
			_pushedSystemUids.erase( pushedIt );
		}

		_entities.erase( std::remove( _entities.begin(), _entities.end(), p_e ), _entities.end() );
		_pushedEntities.erase( std::remove( _pushedEntities.begin(), _pushedEntities.end(), p_e ), _pushedEntities.end() );
		_representations.erase( p_e );
		_needPush = true;
	}

	void SystemUpdater::_pushSystems()
	{
		auto & reg = REG();

		bool representationChanged = false;

		for ( const ECS::Entity system : _entities )
		{
			if ( std::find( _pushedEntities.begin(), _pushedEntities.end(), system ) != _pushedEntities.end() )
			{
				continue;
			}

			const auto & data				  = reg.get<Core::Struct::Topology>( system );
			const auto & transform			  = reg.get<Util::Math::Transform>( system );
			const auto & uid				  = reg.get<System::UID>( system );
			const auto & color				  = reg.get<System::Color>( system );
			const auto & representation		  = reg.get<System::Representation>( system );
			const auto & visibility			  = reg.get<System::Visibility>( system );
			const auto & selection			  = reg.get<System::Selection>( system );
			const size_t atomCount			  = data.getAtomCount();
			const size_t representationCountBefore = _representations.size();
			const auto	 representationRanges = _buildRepresentationRanges( representation );

			assert( atomCount > 0 );

			std::span<const Vec3f> positions = System::getCurrentAtomPositions( system );
			RENDERER().addSystem( Renderer::SystemData { uid.system,
														 transform.computeMatrix(),
														 data,
														 positions,
														 uid.atoms.toStdVector(),
														 uid.residues.toStdVector(),
														 _buildAtomColors( color, data ),
														 representationRanges,
														 _buildAtomRepresentations( data, representationRanges ),
														 visibility.atoms,
														 _buildAtomFlags( selection, atomCount ) } );

			_pushedEntities.push_back( system );
			_pushedSystemUids.emplace( system, uid.system );
			representationChanged = representationChanged || _representations.size() != representationCountBefore;
		}

		if ( representationChanged )
		{
			_setRepresentation();
		}
	}

	void SystemUpdater::_onUpdateVisibility( ECS::Registry & p_r, ECS::Entity p_e )
	{
		const auto & [ visibility, uid ] = p_r.get<System::Visibility, System::UID>( p_e );

		RENDERER().setSystemVisibility( uid.system, visibility.atoms );
	}

	void SystemUpdater::_onUpdateSelection( ECS::Registry & p_r, ECS::Entity p_e )
	{
		const auto & [ selection, uid, data ] = p_r.get<System::Selection, System::UID, Core::Struct::Topology>( p_e );

		RENDERER().setSystemSelection( uid.system, _buildAtomFlags( selection, data.getAtomCount() ) );
	}

	void SystemUpdater::_onUpdateRepresentation( ECS::Registry & p_r, ECS::Entity p_e )
	{
		using namespace Renderer;

		const auto & [ representation, uid, data ]
			= p_r.get<System::Representation, System::UID, Core::Struct::Topology>( p_e );

		const size_t representationCountBefore = _representations.size();
		const auto	 mapAtoms				   = _buildRepresentationRanges( representation );

		if ( _representations.size() != representationCountBefore )
		{
			_setRepresentation();
		}

		RENDERER().setSystemRepresentation( uid.system, mapAtoms, _buildAtomRepresentations( data, mapAtoms ) );
	}

	void SystemUpdater::_onUpdateColor( ECS::Registry & p_r, ECS::Entity p_e )
	{
		const auto & [ color, uid, data ] = p_r.get<System::Color, System::UID, Core::Struct::Topology>( p_e );

		RENDERER().setSystemColors( uid.system, _buildAtomColors( color, data ) );
	}

	Renderer::MapRepresentationRanges SystemUpdater::_buildRepresentationRanges(
		const System::Representation & p_representation
	)
	{
		Renderer::MapRepresentationRanges mapAtoms;

		for ( const auto & [ entity, ranges ] : p_representation.presetAtoms )
		{
			if ( not _representations.contains( entity ) )
			{
				_representations[ entity ] = static_cast<Renderer::RepresentationIndex>( _representations.size() );
			}
			mapAtoms.emplace( _representations[ entity ], ranges );
		}

		return mapAtoms;
	}

	std::vector<Renderer::RepresentationIndex> SystemUpdater::_buildAtomRepresentations(
		const Core::Struct::Topology &			  p_data,
		const Renderer::MapRepresentationRanges & p_representations
	) const
	{
		const size_t							   atomCount = p_data.getAtomCount();
		std::vector<Renderer::RepresentationIndex> atoms( atomCount );
		size_t									   count = 0;

		for ( const auto & [ index, ranges ] : p_representations )
		{
			for ( auto it = ranges.rangeBegin(); it != ranges.rangeEnd(); ++it )
			{
				std::fill_n( atoms.begin() + it->getFirst(), it->getCount(), index );
			}
			count += ranges.count();
		}

		assert( count == atomCount );

		return atoms;
	}

	std::vector<Renderer::ColorIndex> SystemUpdater::_buildAtomColors(
		const System::Color &		   p_color,
		const Core::Struct::Topology & p_data
	) const
	{
		std::vector<Renderer::ColorIndex> atoms( p_data.getAtomCount() );
		size_t							  count = 0;

		for ( const auto & [ scheme, ranges ] : p_color.colorSchemeAtoms )
		{
			if ( scheme == System::E_COLOR_SCHEME::ATOM )
			{
				for ( Index atom : ranges )
				{
					atoms[ atom ] = Renderer::Color::getColorIndex( p_data.getAtomSymbol( atom ) );
				}
			}
			else if ( scheme == System::E_COLOR_SCHEME::RESIDUE )
			{
				for ( Index atom : ranges )
				{
					const Index residue = p_data.atomResidueIndexes[ atom ];
					atoms[ atom ]		= Renderer::Color::getColorIndex( p_data.getResidueSymbol( residue ) );
				}
			}
			else if ( scheme == System::E_COLOR_SCHEME::CHAIN )
			{
				for ( Index atom : ranges )
				{
					const Index chain = p_data.getAtomChainIndex( atom );
					atoms[ atom ]	  = Renderer::Color::getColorIndex( p_data.getChainName( chain ) );
				}
			}
			else
			{
				assert( false && "Unsupported System::E_COLOR_SCHEME type in ColorScheme::Add action." );
			}
			count += ranges.count();
		}
		for ( const auto & [ colorIndex, ranges ] : p_color.customColorAtoms )
		{
			for ( Index atom : ranges )
			{
				atoms[ atom ] = colorIndex;
			}
			count += ranges.count();
		}

		assert( count == p_data.getAtomCount() );

		return atoms;
	}

	std::vector<Renderer::Flag> SystemUpdater::_buildAtomFlags(
		const System::Selection & p_selection,
		const size_t			  p_atomCount
	) const
	{
		std::vector<Renderer::Flag> atomFlags( p_atomCount, 0 );

		for ( auto i : p_selection.atoms )
		{
			atomFlags[ i ] |= toUnderlying( Renderer::E_ELEMENT_FLAGS::SELECTION );
		}

		return atomFlags;
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
