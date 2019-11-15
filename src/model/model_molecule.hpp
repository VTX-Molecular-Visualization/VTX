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
			const std::string & getName() const { return _name; };
			void				setName( const std::string & p_name ) { _name = p_name; };

			ModelChain & addChain() { return _chains.emplace_back( ModelChain() ); }
			ModelChain & getChain( uint p_idx ) { return _chains[ p_idx ]; }

			ModelResidue & addResidue() { return _residues.emplace_back( ModelResidue() ); }
			ModelResidue & getResidue( uint p_idx ) { return _residues[ p_idx ]; }

			ModelAtom & addAtom() { return _atoms.emplace_back( ModelAtom() ); }
			ModelAtom & getAtom( uint p_idx ) { return _atoms[ p_idx ]; }

			void addAtomPosition( const Vec3f p_position ) { _atomPositions.emplace_back( p_position ); }
			void addAtomRadius( const float p_radius ) { _atomRadius.emplace_back( p_radius ); }
			void addAtomColor( const Vec3f p_color ) { _atomColors.emplace_back( p_color ); }

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
		};
	} // namespace Model
} // namespace VTX
#endif
