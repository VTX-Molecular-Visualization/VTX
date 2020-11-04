#ifndef __VTX_MODEL_SELECTION__
#define __VTX_MODEL_SELECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_model.hpp"
#include "define.hpp"
#include "model/atom.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include <map>

namespace VTX
{
	namespace Model
	{
		class Selection : public BaseModel
		{
			// Map molecule model id with with chains, residues and atoms.
			using MapIds = std::map<ID, std::map<ID, std::map<ID, std::vector<ID>>>>;

		  public:
			Selection() : BaseModel( ID::Model::MODEL_SELECTION ) {}
			~Selection() = default;

			void selectMolecule( const Molecule & );
			void unselectMolecule( const Molecule & );
			void selectChain( const Chain & );
			void unselectChain( const Chain & );
			void selectResidue( const Residue & );
			void unselectResidue( const Residue & );
			void selectAtom( const Atom & );
			void unselectAtom( const Atom & );

			void clear();

		  private:
			MapIds _items = MapIds();

			void _addMolecule( const Molecule & );
			void _addChain( const Chain & );
			void _addResidue( const Residue & );
			void _addAtom( const Atom & );

			void _addMoleculeContent( const Molecule & );
			void _addChainContent( const Chain & );
			void _addResidueContent( const Residue & );

			void _removeMolecule( const Molecule & );
			void _removeChain( const Chain & );
			void _removeResidue( const Residue & );
			void _removeAtom( const Atom & );
		};

	} // namespace Model
} // namespace VTX
#endif
