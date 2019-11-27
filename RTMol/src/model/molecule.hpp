#ifndef __MOLECULE_HPP__
#define __MOLECULE_HPP__

#include "../defines.hpp"
#include "atom.hpp"
#include "chain.hpp"
#include "residue.hpp"
#include <iostream>
#include <set>
#include <vector>

namespace Model
{
	class Molecule
	{
	  public:
		inline const std::string & getName() const { return _name; };
		inline void				   setName( const std::string & p_name ) { _name = p_name; };

		inline Chain &				  addChain() { return _chains.emplace_back( Chain() ); }
		inline Chain &				  getChain( uint p_idx ) { return _chains[ p_idx ]; }
		inline std::vector<Chain> &	  getChains() { return _chains; }
		inline Residue &			  addResidue() { return _residues.emplace_back( Residue() ); }
		inline Residue &			  getResidue( uint p_idx ) { return _residues[ p_idx ]; }
		inline std::vector<Residue> & getResidues() { return _residues; }
		inline Atom &				  addAtom() { return _atoms.emplace_back( Atom() ); }
		inline Atom &				  getAtom( uint p_idx ) { return _atoms[ p_idx ]; }
		inline std::vector<Atom> &	  getAtoms() { return _atoms; }

		inline std::set<std::string> & getUnknownResidueSymbols() { return _unknownResidueSymbol; }
		inline std::set<std::string> & getUnknownAtomSymbols() { return _unknownAtomSymbol; }

		inline void addUnknownResidueSymbol( const std::string & p_symbol )
		{
			_unknownResidueSymbol.emplace( p_symbol );
		}
		inline void addUnknownAtomSymbol( const std::string & p_symbol ) { _unknownAtomSymbol.emplace( p_symbol ); }

		inline Vec3f & addAtomPosition( const Vec3f p_position ) { return _atomPositions.emplace_back( p_position ); }
		inline void	   addAtomRadius( const float p_radius ) { _atomRadius.emplace_back( p_radius ); }
		inline float   getAtomRadius( const uint p_idx ) { return _atomRadius[ p_idx ]; }
		inline void	   addAtomColor( const Vec3f p_color ) { _atomColors.emplace_back( p_color ); }
		inline Vec3f & getAtomColor( const uint p_idx ) { return _atomColors[ p_idx ]; }
		inline void	   addBond( const uint p_bond ) { _bonds.emplace_back( p_bond ); }
		inline void	   addBonds( const std::vector<int> & p_bonds )
		{
			_bonds.insert( _bonds.end(), p_bonds.begin(), p_bonds.end() );
		}

		inline uint getChainCount() const { return (uint)_chains.size(); }
		inline uint getResidueCount() const { return (uint)_residues.size(); }
		inline uint getAtomCount() const { return (uint)_atoms.size(); }
		inline uint getBondCount() const { return (uint)_bonds.size(); }

		friend std::ostream & operator<<( std::ostream & out, Molecule & p_molecule )
		{
			return out << "-------------------------------------------" << std::endl
					   << "Chains: " + std::to_string( p_molecule.getChainCount() ) << std::endl
					   << "Residues: " + std::to_string( p_molecule.getResidueCount() ) << std::endl
					   << "Atoms: " + std::to_string( p_molecule.getAtomCount() ) << std::endl
					   << "Bonds: " + std::to_string( p_molecule.getBondCount() ) << std::endl
					   << "-------------------------------------------" << std::endl;
		}

	  private:
		// Models.
		std::string			 _name	   = "";
		std::vector<Chain>	 _chains   = std::vector<Chain>();
		std::vector<Residue> _residues = std::vector<Residue>();
		std::vector<Atom>	 _atoms	   = std::vector<Atom>();

		std::set<std::string> _unknownResidueSymbol = std::set<std::string>();
		std::set<std::string> _unknownAtomSymbol	= std::set<std::string>();

		// Buffers.
		std::vector<Vec3f> _atomPositions = std::vector<Vec3f>();
		std::vector<float> _atomRadius	  = std::vector<float>();
		std::vector<Vec3f> _atomColors	  = std::vector<Vec3f>();
		std::vector<uint>  _bonds		  = std::vector<uint>();

#ifdef _DEBUG
	  public:
		// Validation.
		uint chainCount	  = 0;
		uint residueCount = 0;
		uint atomCount	  = 0;
		uint bondCount	  = 0;
#endif
	};
} // namespace Model
#endif
