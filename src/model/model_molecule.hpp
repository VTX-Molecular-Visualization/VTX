#ifndef __VTX_MODEL_MOLECULE__
#define __VTX_MODEL_MOLECULE__

#include "../defines.hpp"
#include "atom.hpp"
#include "base_model.hpp"
#include "chain.hpp"
#include "residue.hpp"
#include <iostream>
#include <vector>

namespace VTX
{
	namespace Model
	{
		class ModelMolecule : BaseModel
		{
		  public:
			std::string getName() const { return _name; };
			void setName( const std::string & p_name ) { _name = p_name; };

			Chain &	  addChain() { return _chains.emplace_back( Chain() ); }
			Residue & addResidue()
			{
				return _residues.emplace_back( Residue() );
			}
			Atom & addAtom() { return _atoms.emplace_back( Atom() ); }

			Chain &	  getChain( uint p_idx ) { return _chains[ p_idx ]; }
			Residue & getResidue( uint p_idx ) { return _residues[ p_idx ]; }
			Atom &	  getAtom( uint p_idx ) { return _atoms[ p_idx ]; }

		  private:
			std::string			 _name	   = "";
			std::vector<Chain>	 _chains   = std::vector<Chain>();
			std::vector<Residue> _residues = std::vector<Residue>();
			std::vector<Atom>	 _atoms	   = std::vector<Atom>();
		};
	} // namespace Model
} // namespace VTX
#endif
