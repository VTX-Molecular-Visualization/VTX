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
		system.uid		 = _nextSystemUid++;

		const size_t residueCount = system.topology->getResidueCount();
		system.residueUids.resize( residueCount );
		std::iota( system.residueUids.begin(), system.residueUids.end(), _nextPickingUid );
		_nextPickingUid += PickingUID( residueCount );

		const size_t atomCount = system.topology->getAtomCount();
		system.atomUids.resize( atomCount );
		std::iota( system.atomUids.begin(), system.atomUids.end(), _nextPickingUid );
		_nextPickingUid += PickingUID( atomCount );

		_systems.emplace_back( std::move( system ) );
	};

	void Scene::removeSystem( const size_t p_index ) { _systems.erase( _systems.begin() + p_index ); }

	std::vector<Renderer::SystemData> Scene::_buildRendererSystems() const
	{
		std::vector<Renderer::SystemData> systems;
		systems.reserve( _systems.size() );

		for ( const SystemEntry & system : _systems )
		{
			systems.push_back(
				Renderer::SystemData { system.uid,
									   system.transform,
									   *system.topology,
									   system.positions,
									   system.atomUids,
									   system.residueUids }
			);
		}

		return systems;
	}

	std::vector<Renderer::ColorIndex> Scene::_buildAtomColors( const Core::Struct::Topology & p_topology ) const
	{
		const size_t					  atomCount = p_topology.getAtomCount();
		std::vector<Renderer::ColorIndex> colors( atomCount );

		for ( Index atomIndex = 0; atomIndex < atomCount; ++atomIndex )
		{
			colors[ atomIndex ] = Renderer::Color::getColorIndex( p_topology.getAtomSymbol( atomIndex ) );
		}

		return colors;
	}

	Renderer::MapRepresentationRanges Scene::_buildDefaultRepresentation(
		const Core::Struct::Topology & p_topology
	) const
	{
		Renderer::MapRepresentationRanges representations;
		representations.emplace(
			Renderer::RepresentationIndex( 0 ), Core::Struct::IndexRangeList( p_topology.getAtomRange() )
		);
		return representations;
	}

	void Scene::syncRenderer( Renderer::Renderer & p_renderer ) const
	{
		p_renderer.setSystems( _buildRendererSystems() );

		for ( const SystemEntry & system : _systems )
		{
			const size_t atomCount = system.topology->getAtomCount();
			p_renderer.setSystemColors( system.uid, _buildAtomColors( *system.topology ) );
			p_renderer.setSystemRepresentation( system.uid, _buildDefaultRepresentation( *system.topology ) );
			p_renderer.setSystemVisibility( system.uid, Util::Math::BitSet( atomCount, true ) );
			p_renderer.setSystemSelection( system.uid, Util::Math::BitSet( atomCount ) );
		}
	}

} // namespace VTX::Bench
