#include "scene.hpp"
#include "util.hpp"
#include <io/util/secondary_structure.hpp>
#include <numeric>
#include <renderer/color.hpp>
#include <renderer/renderer.hpp>
#include <util/math.hpp>

namespace VTX::Bench
{

	Scene::Scene( const size_t p_width, const size_t p_height ) : _camera( p_width, p_height )
	/*,
	_proxyCamera(
									  { _camera.getMatrixViewPtr(),
										_camera.getMatrixProjectionPtr(),
										_camera.getPosition(),
										VEC2I_ZERO,
										_camera.getNear(),
										_camera.getFar(),
										_camera.isPerspective() }
								  )*/

	{
		/*
		_camera.callbackMatrixView += [ & ]( const Mat4f & p_matrix ) { _proxyCamera.onMatrixView(); };
		_camera.callbackMatrixProjection += [ & ]( const Mat4f & p_matrix ) { _proxyCamera.onMatrixProjection(); };
		_camera.callbackTranslation +=
			[ & ]( const Vec3f & p_position ) { _proxyCamera.onCameraPosition( p_position ); };
		_camera.callbackClipInfos +=
			[ & ]( const float p_near, const float p_far ) { _proxyCamera.onCameraNearFar( p_near, p_far ); };
		_camera.callbackPerspective +=
			[ & ]( const bool p_isPerspective ) { _proxyCamera.onPerspective( p_isPerspective ); };
			*/
	}

	void Scene::addSystem( const std::string & p_name )
	{
		using namespace Util;

		SystemEntry	 system;
		LoadedSystem loadedSystem;
		if ( p_name.find( '.' ) != std::string::npos )
		{
			loadedSystem = loadSystem( p_name );
		}
		else
		{
			loadedSystem = downloadSystem( p_name );
		}

		system.topology	 = std::make_unique<Core::Struct::Topology>( std::move( loadedSystem.topology ) );
		system.positions = std::move( loadedSystem.positions );
		system.uid		 = _nextSystemUID++;

		const size_t residueCount = system.topology->getResidueCount();
		system.residueUids		  = Util::Math::Range<UID32>::fromFirstCount( _nextUID32, UID32( residueCount ) );
		_nextUID32 += UID32( residueCount );

		const size_t atomCount = system.topology->getAtomCount();
		system.atomUids		   = Util::Math::Range<UID32>::fromFirstCount( _nextUID32, UID32( atomCount ) );
		_nextUID32 += UID32( atomCount );

		system.colorSchemeAtoms.emplace(
			Renderer::E_COLOR_SCHEME::ATOM, Core::Struct::IndexRangeList( system.topology->getAtomRange() )
		);
		system.representationHandles.emplace( Entity( 0 ), Renderer::Desc::Handle( 0 ) );
		system.presetAtoms.emplace( Entity( 0 ), Core::Struct::IndexRangeList( system.topology->getAtomRange() ) );
		system.visibility = Util::Math::BitSet( atomCount, true );
		system.selection  = Util::Math::BitSet( atomCount, false );

		_systems.emplace_back( std::move( system ) );
	};

	void Scene::removeSystem( const size_t p_index ) { _systems.erase( _systems.begin() + p_index ); }

	Renderer::Cache::System Scene::_buildRendererSystem( const SystemEntry & p_system ) const
	{
		return Renderer::Cache::System {
			p_system.transform,	  *p_system.topology,		 p_system.positions,		p_system.atomUids,
			p_system.residueUids, p_system.colorSchemeAtoms, p_system.customColorAtoms, p_system.representationHandles,
			p_system.presetAtoms, p_system.visibility,		 p_system.selection
		};
	}

	void Scene::syncRenderer( Renderer::Renderer & p_renderer ) const
	{
		std::unordered_set<uint> currentSystemUids;
		currentSystemUids.reserve( _systems.size() );

		for ( const SystemEntry & system : _systems )
		{
			currentSystemUids.insert( system.uid );
			if ( _syncedSystems.contains( system.uid ) )
			{
				continue;
			}

			_syncedSystems.emplace( system.uid, p_renderer.addSystem( _buildRendererSystem( system ) ) );
		}

		for ( auto it = _syncedSystems.begin(); it != _syncedSystems.end(); )
		{
			if ( currentSystemUids.contains( it->first ) )
			{
				++it;
				continue;
			}

			p_renderer.removeSystem( it->second );
			it = _syncedSystems.erase( it );
		}
	}

} // namespace VTX::Bench
