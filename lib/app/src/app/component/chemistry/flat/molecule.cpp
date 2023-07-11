#include "app/component/chemistry/flat/molecule.hpp"
#include "app/component/chemistry/flat/atom.hpp"
#include "app/component/chemistry/flat/bond.hpp"
#include "app/component/chemistry/flat/chain.hpp"
#include "app/component/chemistry/flat/residue.hpp"
#include <algorithm>
#include <util/constants.hpp>

namespace VTX::App::Component::Chemistry::Flat
{
	namespace ChemDB = VTX::Core::ChemDB;

	Molecule::Molecule() {}
	Molecule::Molecule( VTX::Core::FlatStruct::Molecule & p_moleculeStruct ) { setMoleculeStruct( p_moleculeStruct ); }
	Molecule::~Molecule() = default;

	void Molecule::setMoleculeStruct( VTX::Core::FlatStruct::Molecule & p_moleculeStruct )
	{
		_moleculeStruct = std::move( p_moleculeStruct );

		initChains( _moleculeStruct.getChainCount() );
		initResidues( _moleculeStruct.getResidueCount() );
		initAtoms( _moleculeStruct.getAtomCount() );
		initBonds( _moleculeStruct.getBondCount() );
	}

	void Molecule::initChains( const size_t p_chainCount )
	{
		_chains.resize( p_chainCount, nullptr );
		std::generate( _chains.begin(), _chains.end(), [ this, n = 0 ]() mutable { return new Chain( this, n++ ); } );
	}

	void Molecule::initResidues( const size_t p_residueCount )
	{
		_residues.resize( p_residueCount, nullptr );
		std::generate(
			_residues.begin(), _residues.end(), [ this, n = 0 ]() mutable { return new Residue( this, n++ ); } );
	}

	void Molecule::initAtoms( const size_t p_atomCount )
	{
		_atoms.resize( p_atomCount, nullptr );
		std::generate( _atoms.begin(), _atoms.end(), [ this, n = 0 ]() mutable { return new Atom( this, n++ ); } );

		_atomVisibilities.resize( p_atomCount, uint( true ) );

		_atomColors.resize( p_atomCount, COLOR_WHITE );
		std::generate( _atomColors.begin(),
					   _atomColors.end(),
					   [ this, i = 0 ]() mutable
					   { return ChemDB::Atom::SYMBOL_COLOR[ int( _moleculeStruct.atomSymbols[ i ] ) ]; } );

		_atomRadii.resize( p_atomCount, 0.f );
		std::generate( _atomRadii.begin(),
					   _atomRadii.end(),
					   [ this, i = 0 ]() mutable
					   { return ChemDB::Atom::SYMBOL_VDW_RADIUS[ int( _moleculeStruct.atomSymbols[ i ] ) ]; } );

		_atomIds.resize( p_atomCount, uint( INVALID_INDEX ) );
		std::generate( _atomIds.begin(), _atomIds.end(), [ this, i = 0 ]() mutable { return i++; } );

		_atomSelections.resize( p_atomCount, uint( false ) );
	}

	void Molecule::initBonds( const size_t p_bondCount )
	{
		_bonds.resize( p_bondCount, nullptr );
		std::generate( _bonds.begin(), _bonds.end(), [ this, n = 0 ]() mutable { return new Bond( this, n++ ); } );
	}
} // namespace VTX::App::Component::Chemistry::Flat
