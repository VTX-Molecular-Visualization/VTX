#ifndef __VTX_MODEL_MOLECULE__
#define __VTX_MODEL_MOLECULE__

#include "../defines.hpp"
#include "../util/logger.hpp"
#include "base_model.hpp"
#include "model_atom.hpp"
#include "model_chain.hpp"
#include "model_residue.hpp"
#include <iostream>
#include <vector>

namespace VTX
{
	namespace Model
	{
		class ModelMolecule : BaseModel
		{
		  public:
			inline const std::string & getName() const { return _name; };
			inline void				   setName( const std::string & p_name ) { _name = p_name; };

			inline ModelChain &	  addChain() { return _chains.emplace_back( ModelChain() ); }
			inline ModelChain &	  getChain( uint p_idx ) { return _chains[ p_idx ]; }
			inline ModelResidue & addResidue() { return _residues.emplace_back( ModelResidue() ); }
			inline ModelResidue & getResidue( uint p_idx ) { return _residues[ p_idx ]; }
			inline ModelAtom &	  addAtom() { return _atoms.emplace_back( ModelAtom() ); }
			inline ModelAtom &	  getAtom( uint p_idx ) { return _atoms[ p_idx ]; }

			inline void addAtomPosition( const Vec3f p_position ) { _atomPositions.emplace_back( p_position ); }
			inline void addAtomRadius( const float p_radius ) { _atomRadius.emplace_back( p_radius ); }
			inline void addAtomColor( const Vec3f p_color ) { _atomColors.emplace_back( p_color ); }
			inline void addBond( const uint p_bond ) { _bonds.emplace_back( p_bond ); }
			inline void addBonds( const std::vector<int> & p_bonds )
			{
				_bonds.insert( _bonds.end(), p_bonds.begin(), p_bonds.end() );
			}

			inline uint getChainCount() const { return _chains.size(); }
			inline uint getResidueCount() const { return _residues.size(); }
			inline uint getAtomCount() const { return _atoms.size(); }
			inline uint getBondCount() const { return _bonds.size(); }

			void printInfos()
			{
				VTX_INFO( "Chains: " + std::to_string( _chains.size() ) );
				VTX_INFO( "Residues: " + std::to_string( _residues.size() ) );
				VTX_INFO( "Atoms: " + std::to_string( _atoms.size() ) );
			}

		  private:
			// Models.
			std::string				  _name		= "";
			std::vector<ModelChain>	  _chains	= std::vector<ModelChain>();
			std::vector<ModelResidue> _residues = std::vector<ModelResidue>();
			std::vector<ModelAtom>	  _atoms	= std::vector<ModelAtom>();

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
} // namespace VTX
#endif
