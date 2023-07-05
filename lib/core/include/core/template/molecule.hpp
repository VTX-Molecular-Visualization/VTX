#ifndef __VTX_CORE_TEMPLATE_MOLECULE__
#define __VTX_CORE_TEMPLATE_MOLECULE__

#include "_fwd.hpp"
#include "concept.hpp"
#include "core/chemdb/category.hpp"
#include "core/gpu/molecule.hpp"
#include "core/struct/category.hpp"
#include "core/struct/trajectory.hpp"
#include <array>
#include <util/color/rgba.hpp>
#include <vector>

namespace VTX::Core::Template
{
	template<ConceptChain C, ConceptResidue R, ConceptAtom A, ConceptBond B>
	class Molecule
	{
	  private:
		static const int CATEGORY_COUNT = int( ChemDB::Category::TYPE::COUNT );

	  public:
		Molecule()
		{
			for ( size_t i = 0; i < CATEGORY_COUNT; i++ )
			{
				_categories[ i ] = std::make_unique<Struct::Category>( ChemDB::Category::TYPE( i ) );
			}
		}

		~Molecule() {}

		void initChains( const size_t p_chainCount )
		{
			_chains.resize( p_chainCount, nullptr );
			std::generate( _chains.begin(), _chains.end(), [ this, n = 0 ]() mutable { return new C( this, n++ ); } );
		}
		C * const				 getChain( const size_t p_chainIndex ) { return _chains[ p_chainIndex ]; }
		const C * const			 getChain( const size_t p_chainIndex ) const { return _chains[ p_chainIndex ]; }
		std::vector<C *> &		 getChains() { return _chains; }
		const std::vector<C *> & getChains() const { return _chains; }
		size_t					 getChainCount() const { return _chains.size(); }
		C * const				 appendNewChain()
		{
			C * const res = new C( this, _chains.size() );
			_chains.emplace_back( res );

			return res;
		}

		void initResidues( const size_t p_residueCount )
		{
			_residues.resize( p_residueCount, nullptr );
			std::generate( _residues.begin(), _residues.end(), [ n = 0 ]() mutable { return new R( n++ ); } );
		}
		R * const				 getResidue( const size_t p_chainIndex ) { return _residues[ p_chainIndex ]; }
		const R * const			 getResidue( const size_t p_chainIndex ) const { return _residues[ p_chainIndex ]; }
		std::vector<R *> &		 getResidues() { return _residues; }
		const std::vector<R *> & getResidues() const { return _residues; }
		size_t					 getResidueCount() const { return _residues.size(); }

		void initAtoms( const size_t p_atomCount )
		{
			_atoms.resize( p_atomCount, nullptr );
			std::generate( _atoms.begin(), _atoms.end(), [ this, n = 0 ]() mutable { return new A( this, n++ ); } );
		}
		A * const				 getAtom( const size_t p_chainIndex ) { return _atoms[ p_chainIndex ]; }
		const A * const			 getAtom( const size_t p_chainIndex ) const { return _atoms[ p_chainIndex ]; }
		std::vector<A *> &		 getAtoms() { return _atoms; }
		const std::vector<A *> & getAtoms() const { return _atoms; }
		size_t					 getAtomCount() const { return _atoms.size(); }

		void initBonds( const size_t p_bondCount )
		{
			_bonds.resize( p_bondCount, nullptr );
			std::generate( _bonds.begin(), _bonds.end(), [ this, n = 0 ]() mutable { return new B( this, n++ ); } );
		}
		std::vector<B *> &		 getBonds() { return _bonds; }
		const std::vector<B *> & getBonds() const { return _bonds; }
		size_t					 getBondCount() const { return _bonds.size(); }

		Struct::Category & getCategory( const ChemDB::Category::TYPE p_categoryEnum )
		{
			return *( _categories[ int( p_categoryEnum ) ] );
		}
		const Struct::Category & getCategory( const ChemDB::Category::TYPE p_categoryEnum ) const
		{
			return *( _categories[ int( p_categoryEnum ) ] );
		}

		Struct::Trajectory & getTrajectory() { return _trajectory; }

	  protected:
		// Logic
		std::string _name = "unknown";

		std::array<std::unique_ptr<Struct::Category>, CATEGORY_COUNT> _categories
			= std::array<std::unique_ptr<Struct::Category>, CATEGORY_COUNT>();

		std::vector<C *> _chains   = std::vector<C *>();
		std::vector<R *> _residues = std::vector<R *>();
		std::vector<A *> _atoms	   = std::vector<A *>();
		std::vector<B *> _bonds	   = std::vector<B *>();

		Struct::Trajectory _trajectory = Struct::Trajectory();

		// Always usefull ?
		// size_t								   indexFirstBondExtraResidue = 0;

		// Unused in Core (Move to App::Old ?)
		// uint _realChainCount = 0;
	};

} // namespace VTX::Core::Template

#endif
