#ifndef __VTX_MODEL_MOLECULE__
#define __VTX_MODEL_MOLECULE__

#include "../defines.hpp"
#include "../util/logger.hpp"
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
			void		setName( const std::string & p_name ) { _name = p_name; };

			Chain & addChain() { return _chains.emplace_back( Chain() ); }
			Chain & getChain( uint p_idx ) { return _chains[ p_idx ]; }

			Residue & addResidue() { return _residues.emplace_back( Residue() ); }
			Residue & getResidue( uint p_idx ) { return _residues[ p_idx ]; }

			Atom & addAtom() { return _atoms.emplace_back( Atom() ); }
			Atom & getAtom( uint p_idx ) { return _atoms[ p_idx ]; }
			void   addAtomPosition( const Vec3f p_position ) { _atomPositions.emplace_back( p_position ); }
			void   addAtomRadius( const float p_radius ) { _atomRadius.emplace_back( p_radius ); }
			void   addAtomColor( const Vec3f p_color ) { _atomColors.emplace_back( p_color ); }

			void printInfos()
			{
				VTX_INFO( "Chains: " + std::to_string( _chains.size() ) );
				VTX_INFO( "Residues: " + std::to_string( _residues.size() ) );
				VTX_INFO( "Atoms: " + std::to_string( _atoms.size() ) );
			}

		  private:
			// Models.
			std::string			 _name	   = "";
			std::vector<Chain>	 _chains   = std::vector<Chain>();
			std::vector<Residue> _residues = std::vector<Residue>();
			std::vector<Atom>	 _atoms	   = std::vector<Atom>();

			// Buffers.
			std::vector<Vec3f> _atomPositions = std::vector<Vec3f>();
			std::vector<float> _atomRadius	  = std::vector<float>();
			std::vector<Vec3f> _atomColors	  = std::vector<Vec3f>();
		};
	} // namespace Model
} // namespace VTX
#endif
