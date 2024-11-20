#include "scene.hpp"
#include "util.hpp"
#include <core/chemdb/color.hpp>
#include <io/util/secondary_structure.hpp>
#include <numeric>
#include <renderer/facade.hpp>
#include <util/math.hpp>

namespace VTX::Bench
{

	Scene::Scene( const size_t p_width, const size_t p_height ) :
		_camera( p_width, p_height ), _colorLayout( Core::ChemDB::Color::COLOR_LAYOUT_JMOL ),
		_proxyCamera( { _camera.getMatrixViewPtr(),
						_camera.getMatrixProjectionPtr(),
						_camera.getPosition(),
						VEC2I_ZERO,
						_camera.getNear(),
						_camera.getFar(),
						_camera.isPerspective() } )
	{
		_camera.callbackMatrixView += [ & ]( const Mat4f & p_matrix ) { _proxyCamera.onMatrixView(); };
		_camera.callbackMatrixProjection += [ & ]( const Mat4f & p_matrix ) { _proxyCamera.onMatrixProjection(); };
		_camera.callbackTranslation +=
			[ & ]( const Vec3f & p_position ) { _proxyCamera.onCameraPosition( p_position ); };
		_camera.callbackClipInfos +=
			[ & ]( const float p_near, const float p_far ) { _proxyCamera.onCameraNearFar( p_near, p_far ); };
		_camera.callbackPerspective +=
			[ & ]( const bool p_isPerspective ) { _proxyCamera.onPerspective( p_isPerspective ); };

		_proxyLayoutColor.colors = &( _colorLayout.layout );
	}

	Renderer::Proxy::System & Scene::addSystem( const std::string & p_name )
	{
		using namespace Util;

		if ( p_name.find( '.' ) != std::string::npos )
		{
			_systems.emplace_back( std::make_unique<Core::Struct::System>( loadSystem( p_name ) ) );
		}
		else
		{
			_systems.emplace_back( std::make_unique<Core::Struct::System>( downloadSystem( p_name ) ) );
		}

		//_systems.back()->transform
		//	= Math::translate( _systems.back()->transform, Math::randomVec3f() * 200.f - 100.f );
		IO::Util::SecondaryStructure::computeStride( *_systems.back() );
		_proxySystems.emplace_back( _proxify( *_systems.back() ) );
		_directions.emplace_back( Math::randomVec3f() * 2.f - 1.f );

		return *_proxySystems.back();
	};

	void Scene::removeSystem( const size_t p_index )
	{
		_proxySystems[ p_index ]->onRemove();
		_systems.erase( _systems.begin() + p_index );
		_proxySystems.erase( _proxySystems.begin() + p_index );
		_directions.erase( _directions.begin() + p_index );
	}

	// TODO: remove renderer from here.
	void Scene::removeAllSystems( Renderer::Facade * const p_renderer )
	{
		std::vector<Renderer::Proxy::System *> proxies;
		for ( auto & proxy : _proxySystems )
		{
			proxies.push_back( proxy.get() );
		}

		p_renderer->removeProxySystems( proxies );

		_systems.clear();
		_proxySystems.clear();
		_directions.clear();
	}

	std::unique_ptr<Renderer::Proxy::System> Scene::_proxify( const Core::Struct::System & p_system )
	{
		// devjla
		// const size_t									sizeAtoms	= p_system.trajectory.frames.front().size();
		const size_t									sizeAtoms	= p_system.trajectory.GetCurrentFrame().size();
		const std::vector<Core::ChemDB::Atom::SYMBOL> & symbols		= p_system.atomSymbols;
		const size_t									sizeResidue = p_system.residueOriginalIds.size();

		std::vector<uchar> atomColors( sizeAtoms );
		size_t			   i = 0;
		std::generate(
			atomColors.begin(), atomColors.end(), [ & ] { return Core::ChemDB::Color::getColorIndex( symbols[ i++ ] ); }
		);

		auto atomRadii = std::vector<float>( sizeAtoms );
		i			   = 0;
		std::generate(
			atomRadii.begin(),
			atomRadii.end(),
			[ & ] { return Core::ChemDB::Atom::SYMBOL_VDW_RADIUS[ int( symbols[ i++ ] ) ]; }
		);

		std::vector<uint> atomIds( sizeAtoms );
		std::iota( atomIds.begin(), atomIds.end(), 0 );

		std::vector<uint> residueIds( sizeResidue );
		std::iota( residueIds.begin(), residueIds.end(), 0 );

		std::vector<uchar> residueColors( sizeResidue );
		i = 0;
		std::generate(
			residueColors.begin(),
			residueColors.end(),
			[ & ] { return Core::ChemDB::Color::getColorIndex( p_system.residueSecondaryStructureTypes[ i++ ] ); }
		);

		const Core::Struct::Category & categoryPolymer = p_system.getCategory( Core::ChemDB::Category::TYPE::POLYMER );
		const Core::Struct::Category & categoryCarbohydrate
			= p_system.getCategory( Core::ChemDB::Category::TYPE::CARBOHYDRATE );

		const std::vector<size_t> & polymerChainIds		 = categoryPolymer.getLinkedChains();
		const std::vector<size_t> & carbohydrateChainIds = categoryCarbohydrate.getLinkedChains();

		const std::vector<Vec3f> *atomsPositions = &p_system.trajectory.GetCurrentFrame();

		return std::make_unique<Renderer::Proxy::System>( Renderer::Proxy::System {
			&p_system.transform,
			// devjla
			// &p_system.trajectory.frames.front(),
			atomsPositions, // FIXME modelframe?
			&p_system.bondPairAtomIndexes,
			&p_system.atomNames,
			reinterpret_cast<const std::vector<uchar> *>( &p_system.residueSecondaryStructureTypes ),
			&p_system.residueFirstAtomIndexes,
			&p_system.residueAtomCounts,
			&p_system.chainFirstResidues,
			&p_system.chainResidueCounts,
			atomColors,
			atomRadii,
			atomIds,
			residueColors,
			residueIds,
			polymerChainIds,
			carbohydrateChainIds,
			uint( rand() % 3 ) } );
	}

} // namespace VTX::Bench
