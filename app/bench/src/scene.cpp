#include "scene.hpp"
#include "util.hpp"
#include <core/chemdb/color.hpp>
#include <io/util/secondary_structure.hpp>
#include <numeric>
#include <renderer/renderer.hpp>
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

	Renderer::Proxy::Molecule & Scene::addMolecule( const std::string & p_name )
	{
		using namespace Util;

		if ( p_name.find( '.' ) != std::string::npos )
		{
			_molecules.emplace_back( std::make_unique<Core::Struct::System>( loadMolecule( p_name ) ) );
		}
		else
		{
			_molecules.emplace_back( std::make_unique<Core::Struct::System>( downloadMolecule( p_name ) ) );
		}

		//_molecules.back()->transform
		//	= Math::translate( _molecules.back()->transform, Math::randomVec3f() * 200.f - 100.f );
		IO::Util::SecondaryStructure::computeStride( *_molecules.back() );
		_proxyMolecules.emplace_back( _proxify( *_molecules.back() ) );
		_directions.emplace_back( Math::randomVec3f() * 2.f - 1.f );

		return *_proxyMolecules.back();
	};

	void Scene::removeMolecule( const size_t p_index )
	{
		_proxyMolecules[ p_index ]->onRemove();
		_molecules.erase( _molecules.begin() + p_index );
		_proxyMolecules.erase( _proxyMolecules.begin() + p_index );
		_directions.erase( _directions.begin() + p_index );
	}

	// TODO: remove renderer from here.
	void Scene::removeAllMolecules( Renderer::Renderer * const p_renderer )
	{
		std::vector<Renderer::Proxy::Molecule *> proxies;
		for ( auto & proxy : _proxyMolecules )
		{
			proxies.push_back( proxy.get() );
		}

		p_renderer->removeProxyMolecules( proxies );

		_molecules.clear();
		_proxyMolecules.clear();
		_directions.clear();
	}

	std::unique_ptr<Renderer::Proxy::Molecule> Scene::_proxify( const Core::Struct::System & p_molecule )
	{
		const size_t									sizeAtoms	= p_molecule.trajectory.frames.front().size();
		const std::vector<Core::ChemDB::Atom::SYMBOL> & symbols		= p_molecule.atomSymbols;
		const size_t									sizeResidue = p_molecule.residueOriginalIds.size();

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
			[ & ] { return Core::ChemDB::Color::getColorIndex( p_molecule.residueSecondaryStructureTypes[ i++ ] ); }
		);

		const Core::Struct::Category & categoryPolymer
			= p_molecule.getCategory( Core::ChemDB::Category::TYPE::POLYMER );
		const Core::Struct::Category & categoryCarbohydrate
			= p_molecule.getCategory( Core::ChemDB::Category::TYPE::CARBOHYDRATE );

		const std::vector<size_t> & polymerChainIds		 = categoryPolymer.getLinkedChains();
		const std::vector<size_t> & carbohydrateChainIds = categoryCarbohydrate.getLinkedChains();

		return std::make_unique<Renderer::Proxy::Molecule>( Renderer::Proxy::Molecule {
			&p_molecule.transform,
			&p_molecule.trajectory.frames.front(),
			&p_molecule.bondPairAtomIndexes,
			&p_molecule.atomNames,
			reinterpret_cast<const std::vector<uchar> *>( &p_molecule.residueSecondaryStructureTypes ),
			&p_molecule.residueFirstAtomIndexes,
			&p_molecule.residueAtomCounts,
			&p_molecule.chainFirstResidues,
			&p_molecule.chainResidueCounts,
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
