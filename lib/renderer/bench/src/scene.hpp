#ifndef __VTX_BENCH_SCENE__
#define __VTX_BENCH_SCENE__

#include "util.hpp"
#include <core/struct/molecule.hpp>
#include <renderer/proxy/molecule.hpp>
#include <util/chrono.hpp>
#include <util/math.hpp>

namespace VTX::Bench
{

	class Scene
	{
	  public:
		Renderer::Proxy::Molecule & addMolecule( const std::string & p_name )
		{
			using namespace Util;

			if ( p_name.find( '.' ) != std::string::npos )
			{
				_molecules.emplace_back( std::make_unique<Core::Struct::Molecule>( loadMolecule( p_name ) ) );
			}
			else
			{
				_molecules.emplace_back( std::make_unique<Core::Struct::Molecule>( downloadMolecule( p_name ) ) );
			}

			_molecules.back()->transform
				= Math::translate( _molecules.back()->transform, Math::randomVec3f() * 200.f - 100.f );
			IO::Util::SecondaryStructure::computeStride( *_molecules.back() );
			_proxies.emplace_back( _proxify( *_molecules.back() ) );
			_directions.emplace_back( Math::randomVec3f() * 2.f - 1.f );

			return *_proxies.back();
		};

		void removeMolecule( const size_t p_index )
		{
			_proxies[ p_index ]->onRemove();
			_molecules.erase( _molecules.begin() + p_index );
			_proxies.erase( _proxies.begin() + p_index );
			_directions.erase( _directions.begin() + p_index );
		}

		void update( const float p_deltaTime )
		{
			if ( !isUpdate )
			{
				return;
			}

			int i = 0;
			for ( auto & molecule : _molecules )
			{
				molecule->transform = Util::Math::rotate( molecule->transform, p_deltaTime, _directions[ i ] );
				_proxies[ i++ ]->onTransform();
			}
		}

		inline const std::vector<std::unique_ptr<Core::Struct::Molecule>> & getMolecules() const { return _molecules; }
		inline const std::vector<std::unique_ptr<Renderer::Proxy::Molecule>> & getProxiesMolecules() const
		{
			return _proxies;
		}

		bool isUpdate = false;

	  private:
		std::vector<std::unique_ptr<Core::Struct::Molecule>>	_molecules;
		std::vector<std::unique_ptr<Renderer::Proxy::Molecule>> _proxies;
		std::vector<Vec3f>										_directions;

		std::unique_ptr<Renderer::Proxy::Molecule> _proxify( const Core::Struct::Molecule & p_molecule )
		{
			const size_t									sizeAtoms	= p_molecule.trajectory.frames.front().size();
			const std::vector<Core::ChemDB::Atom::SYMBOL> & symbols		= p_molecule.atomSymbols;
			const size_t									sizeResidue = p_molecule.residueOriginalIds.size();

			auto   atomColors = std::vector<uchar>( sizeAtoms );
			size_t i		  = 0;
			std::generate(
				atomColors.begin(),
				atomColors.end(),
				[ & ] { return Core::ChemDB::Color::getColorIndex( symbols[ i++ ] ); }
			);

			auto atomRadii = std::vector<float>( sizeAtoms );
			i			   = 0;
			std::generate(
				atomRadii.begin(),
				atomRadii.end(),
				[ & ] { return Core::ChemDB::Atom::SYMBOL_VDW_RADIUS[ int( symbols[ i++ ] ) ]; }
			);

			auto atomIds = std::vector<uint>( sizeAtoms );
			std::iota( atomIds.begin(), atomIds.end(), 0 );

			auto residueIds = std::vector<uint>( sizeResidue );
			std::iota( residueIds.begin(), residueIds.end(), 0 );

			auto residueColors = std::vector<uchar>( sizeResidue );
			i				   = 0;
			std::generate(
				residueColors.begin(),
				residueColors.end(),
				[ & ] { return Core::ChemDB::Color::getColorIndex( p_molecule.residueSecondaryStructureTypes[ i++ ] ); }
			);

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
				uint( rand() % 3 ) } );
		}
	};

} // namespace VTX::Bench

#endif
