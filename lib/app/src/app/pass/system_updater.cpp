#include "app/pass/system_updater.hpp"
#include "app/events.hpp"
#include "app/services.hpp"
#include "app/system/color.hpp"
#include "app/system/representation.hpp"
#include "app/system/selection.hpp"
#include "app/system/trajectory.hpp"
#include "app/system/uid.hpp"
#include "app/system/visibility.hpp"
#include <renderer/renderer.hpp>
#include <util/chrono.hpp>
#include <util/math/transform.hpp>

namespace
{
	using namespace VTX;
	using namespace VTX::App;
	using namespace VTX::Renderer;
	using namespace VTX::Core;

	/**
	 * @brief Generate vectors from IndexRangeList.
	 */
	std::vector<std::byte> _toByteVector( const Struct::IndexRangeList & p_ranges, const size_t p_size )
	{
		std::vector<std::byte> atoms( p_size, std::byte { 0 } );

		for ( auto it = p_ranges.rangeBegin(); it != p_ranges.rangeEnd(); ++it )
		{
			std::fill_n( atoms.begin() + it->getFirst(), it->getCount(), std::byte { 1 } );
		}

		return atoms;
	}

	std::vector<PickingUID> _toVector( const Util::Math::Range<PickingUID> & p_range, const size_t p_size )
	{
		std::vector<Index> atoms( p_size );

		assert( p_range.getCount() == p_size );

		size_t i = 0;
		for ( const PickingUID index : p_range )
		{
			atoms[ i++ ] = index;
		}

		return atoms;
	}

	std::unordered_map<RepresentationIndex, Struct::IndexRangeList> _toRepresentationIndexMap(
		const std::unordered_map<ECS::Entity, Struct::IndexRangeList> p_ranges,
		Pass::SystemUpdater::RepresentationMap &					  p_outRepresentationMap
	)
	{
		std::unordered_map<RepresentationIndex, Struct::IndexRangeList> map;
		size_t															count = 0;

		for ( const auto & [ entity, ranges ] : p_ranges )
		{
			if ( not p_outRepresentationMap.contains( entity ) )
			{
				p_outRepresentationMap[ entity ] = static_cast<RepresentationIndex>( p_outRepresentationMap.size() );
			}
			map.emplace( p_outRepresentationMap[ entity ], ranges );
		}

		return map;
	}

	std::vector<ColorIndex> _toVector(
		const std::unordered_map<System::E_COLOR_SCHEME, Struct::IndexRangeList> p_ranges,
		const Core::Struct::System &											 p_data
	)
	{
		std::vector<ColorIndex> atoms( p_data.getAtomCount() );
		size_t					count = 0;

		for ( const auto & [ scheme, ranges ] : p_ranges )
		{
			if ( scheme == System::E_COLOR_SCHEME::ATOM )
			{
				for ( Index atom : ranges )
				{
					atoms[ atom ] = Color::getColorIndex( p_data.getAtomSymbol( atom ) );
				}
			}
			else if ( scheme == System::E_COLOR_SCHEME::RESIDUE )
			{
				for ( Index atom : ranges )
				{
					const Index residue = p_data.atomResidueIndexes[ atom ];
					atoms[ atom ]		= Color::getColorIndex( p_data.getResidueSymbol( residue ) );
				}
			}
			else if ( scheme == System::E_COLOR_SCHEME::CHAIN )
			{
				for ( Index atom : ranges )
				{
					const Index chain = p_data.getAtomChainIndex( atom );
					atoms[ atom ]	  = Color::getColorIndex( p_data.getChainName( chain ) );
				}
			}
			else
			{
				assert( false && "Unsupported System::E_COLOR_SCHEME type in ColorScheme::Add action." );
			}
			count += ranges.count();
		}

		assert( count == p_data.getAtomCount() );

		return atoms;
	}
} // namespace

namespace VTX::App::Pass
{

	SystemUpdater::SystemUpdater()
	{
		auto & reg = REG();

		reg.on_update<System::Visibility>().connect<&SystemUpdater::_onUpdateVisibility>( this );
		reg.on_update<System::Selection>().connect<&SystemUpdater::_onUpdateSelection>( this );
		reg.on_update<System::Representation>().connect<&SystemUpdater::_onUpdateRepresentation>( this );
		reg.on_update<System::Color>().connect<&SystemUpdater::_onUpdateColor>( this );

		reg.on_update<Renderer::Representation>().connect<&SystemUpdater::_onUpdateRepresentationPreset>( this );

		HUB().connect<Events::SystemLoad, &SystemUpdater::_onSystemLoaded>( this );
	}

	void SystemUpdater::_onUpdateVisibility( ECS::Registry & p_r, ECS::Entity p_e )
	{
		const auto & [ visibility, selection, uid, data ]
			= p_r.get<System::Visibility, System::Selection, System::UID, Core::Struct::System>( p_e );
		RENDERER().setSystemVisibility(
			uid.system,
			_toByteVector( visibility.atoms, data.getAtomCount() ),
			_toByteVector( selection.atoms, data.getAtomCount() )
		);
	}

	void SystemUpdater::_onUpdateSelection( ECS::Registry & p_r, ECS::Entity p_e )
	{
		const auto & [ selection, visibility, uid, data ]
			= p_r.get<System::Selection, System::Visibility, System::UID, Core::Struct::System>( p_e );
		RENDERER().setSystemSelection(
			uid.system,
			_toByteVector( selection.atoms, data.getAtomCount() ),
			_toByteVector( visibility.atoms, data.getAtomCount() )
		);
	}

	void SystemUpdater::_onUpdateRepresentation( ECS::Registry & p_r, ECS::Entity p_e )
	{
		const auto & [ representation, uid, data ]
			= p_r.get<System::Representation, System::UID, Core::Struct::System>( p_e );
		// RENDERER().setSystemRepresentation(
		//	uid.system, _toRepresentationIndexMap( representation.presetAtoms, data.getAtomCount(), _representations )
		//);
		_setRepresentation();
	}

	void SystemUpdater::_onUpdateColor( ECS::Registry & p_r, ECS::Entity p_e )
	{
		const auto & [ color, uid, data ] = p_r.get<System::Color, System::UID, Core::Struct::System>( p_e );
		RENDERER().setSystemColors( uid.system, _toVector( color.colorSchemeAtoms, data ) );
	}

	void SystemUpdater::_onSystemLoaded( const Events::SystemLoad & p_event )
	{
		Util::Chrono timer;
		auto &		 reg = REG();

		timer.start();

		assert( std::find( _entities.begin(), _entities.end(), p_event.system ) == _entities.end() );

		_entities.push_back( p_event.system );

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

			const std::vector<Vec3f> * frame = nullptr;
			if ( const auto * const c = reg.try_get<System::TrajectorySingleFrame>( system ) )
			{
				frame = &c->atomPositions;
			}
			else if ( const auto * const c = reg.try_get<System::TrajectoryFullBuffer>( system ) )
			{
				frame = &c->frameCollection.front();
			}

			assert( frame != nullptr );
			assert( atomCount > 0 );
			assert( atomCount == frame->size() );

			std::vector<float> radii;
			for ( Index i = 0; i < atomCount; ++i )
			{
				// TODO: use glsl constants.
				radii.push_back( 1.0f );
			}

			systemsData.push_back(
				Renderer::SystemData { uid.system,
									   transform.computeMatrix(),
									   data,
									   *frame,
									   radii,
									   _toVector( uid.atoms, atomCount ),
									   _toVector( color.colorSchemeAtoms, data ),
									   _toRepresentationIndexMap( representation.presetAtoms, _representations ),
									   _toByteVector( visibility.atoms, atomCount ),
									   _toByteVector( selection.atoms, atomCount ) }
			);
		}

		VTX_INFO( "Systems GPU upload preparation: {} ms", timer.elapsedTime() );

		RENDERER().setSystems( systemsData );
		_setRepresentation();
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
} // namespace VTX::App::Pass
