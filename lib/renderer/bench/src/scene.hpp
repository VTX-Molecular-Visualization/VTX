#ifndef __VTX_BENCH_SCENE__
#define __VTX_BENCH_SCENE__

#include "util.hpp"
#include <core/chemdb/color.hpp>
#include <core/struct/category.hpp>
#include <core/struct/color_layout.hpp>
#include <core/struct/molecule.hpp>
#include <renderer/proxy/molecule.hpp>
#include <renderer/renderer.hpp>
#include <util/chrono.hpp>
#include <util/math.hpp>

namespace VTX::Bench
{

	class Scene
	{
	  public:
		Scene()
		{
			_colorLayout			 = Core::ChemDB::Color::COLOR_LAYOUT_JMOL;
			_proxyLayoutColor.colors = &( _colorLayout.layout );
		}

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
			_proxyMolecules.emplace_back( _proxify( *_molecules.back() ) );
			_directions.emplace_back( Math::randomVec3f() * 2.f - 1.f );

			return *_proxyMolecules.back();
		};

		void removeMolecule( const size_t p_index )
		{
			_proxyMolecules[ p_index ]->onRemove();
			_molecules.erase( _molecules.begin() + p_index );
			_proxyMolecules.erase( _proxyMolecules.begin() + p_index );
			_directions.erase( _directions.begin() + p_index );
		}

		// TODO: remove renderer from here.
		void removeAllMolecules( Renderer::Renderer * const p_renderer )
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

		void update( const float p_deltaTime )
		{
			if ( !isUpdate )
			{
				return;
			}

			int			i			 = 0;
			static uint currentFrame = 0;

			for ( auto & molecule : _molecules )
			{
				molecule->transform = Util::Math::rotate( molecule->transform, p_deltaTime, _directions[ i ] );

				//_proxyMolecules[ i ]->atomPositions
				//	= &molecule->trajectory.frames[ currentFrame++ % molecule->trajectory.frames.size() ];
				//_proxyMolecules[ i ]->onAtomPositions();

				_proxyMolecules[ i++ ]->onTransform();
			}
		}

		inline const std::vector<std::unique_ptr<Core::Struct::Molecule>> & getMolecules() const { return _molecules; }
		inline const std::vector<std::unique_ptr<Renderer::Proxy::Molecule>> & getProxiesMolecules() const
		{
			return _proxyMolecules;
		}
		inline const Core::Struct::ColorLayout & getColorLayout() const { return _colorLayout; }
		inline void								 setColorLayout( const Core::Struct::ColorLayout & p_colorLayout )
		{
			_colorLayout = p_colorLayout;
			_proxyLayoutColor.onChange();
		}
		inline Renderer::Proxy::ColorLayout & getProxyColorLayout() { return _proxyLayoutColor; }

		bool isUpdate = false;

	  private:
		std::vector<std::unique_ptr<Core::Struct::Molecule>>	_molecules;
		std::vector<std::unique_ptr<Renderer::Proxy::Molecule>> _proxyMolecules;
		std::vector<Vec3f>										_directions;

		Core::Struct::ColorLayout	 _colorLayout;
		Renderer::Proxy::ColorLayout _proxyLayoutColor;

		std::unique_ptr<Renderer::Proxy::Molecule> _proxify( const Core::Struct::Molecule & p_molecule )
		{
			const size_t									sizeAtoms	= p_molecule.trajectory.frames.front().size();
			const std::vector<Core::ChemDB::Atom::SYMBOL> & symbols		= p_molecule.atomSymbols;
			const size_t									sizeResidue = p_molecule.residueOriginalIds.size();

			std::vector<uchar> atomColors( sizeAtoms );
			size_t			   i = 0;
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
	};

} // namespace VTX::Bench

#endif
