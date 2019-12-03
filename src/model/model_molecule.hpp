#ifndef __VTX_MODEL_MOLECULE__
#define __VTX_MODEL_MOLECULE__

#include "../defines.hpp"
#include "../math/aabb.hpp"
#include "../util/logger.hpp"
#include "base_model.hpp"
#include "model_atom.hpp"
#include "model_chain.hpp"
#include "model_residue.hpp"
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <set>
#include <vector>

namespace VTX
{
	namespace Model
	{
		class ModelMolecule : public BaseModel
		{
		  public:
			inline const std::string & getName() const { return _name; };
			inline void				   setName( const std::string & p_name ) { _name = p_name; };

			inline ModelChain &				   addChain() { return _chains.emplace_back( ModelChain() ); }
			inline ModelChain &				   getChain( uint p_idx ) { return _chains[ p_idx ]; }
			inline std::vector<ModelChain> &   getChains() { return _chains; }
			inline ModelResidue &			   addResidue() { return _residues.emplace_back( ModelResidue() ); }
			inline ModelResidue &			   getResidue( uint p_idx ) { return _residues[ p_idx ]; }
			inline std::vector<ModelResidue> & getResidues() { return _residues; }
			inline ModelAtom &				   addAtom() { return _atoms.emplace_back( ModelAtom() ); }
			inline ModelAtom &				   getAtom( uint p_idx ) { return _atoms[ p_idx ]; }
			inline std::vector<ModelAtom> &	   getAtoms() { return _atoms; }

			inline std::set<std::string> & getUnknownResidueSymbols() { return _unknownResidueSymbol; }
			inline std::set<std::string> & getUnknownAtomSymbols() { return _unknownAtomSymbol; }

			inline void addUnknownResidueSymbol( const std::string & p_symbol )
			{
				_unknownResidueSymbol.emplace( p_symbol );
			}
			inline void addUnknownAtomSymbol( const std::string & p_symbol ) { _unknownAtomSymbol.emplace( p_symbol ); }

			inline Vec3f & addAtomPosition( const Vec3f p_position )
			{
				return _atomPositions.emplace_back( p_position );
			}
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

			inline Math::AABB & AABB() { return _aabb; }

			void printInfos() const;

			ModelChain * const	 getSelectedChain() const { return _selectedChain; }
			ModelResidue * const getSelectedResidue() const { return _selectedResidue; }
			ModelAtom * const	 getSelectedAtom() const { return _selectedAtom; }
			void				 setSelectedChain( const uint );
			void				 setSelectedResidue( const uint );
			void				 setSelectedAtom( const uint );
			void				 resetSelectedChain();
			void				 resetSelectedResidue();
			void				 resetSelectedAtom();

		  protected:
			virtual void _addViews() override final;

		  private:
			// Models.
			std::string				  _name		= "";
			std::vector<ModelChain>	  _chains	= std::vector<ModelChain>();
			std::vector<ModelResidue> _residues = std::vector<ModelResidue>();
			std::vector<ModelAtom>	  _atoms	= std::vector<ModelAtom>();

			std::set<std::string> _unknownResidueSymbol = std::set<std::string>();
			std::set<std::string> _unknownAtomSymbol	= std::set<std::string>();

			// Buffers.
			std::vector<Vec3f> _atomPositions = std::vector<Vec3f>();
			std::vector<float> _atomRadius	  = std::vector<float>();
			std::vector<Vec3f> _atomColors	  = std::vector<Vec3f>();
			std::vector<uint>  _bonds		  = std::vector<uint>();

			Math::AABB _aabb;

			// Selection.
			ModelChain *   _selectedChain	= nullptr;
			ModelResidue * _selectedResidue = nullptr;
			ModelAtom *	   _selectedAtom	= nullptr;

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
