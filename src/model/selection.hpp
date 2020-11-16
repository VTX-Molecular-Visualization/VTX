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
		  public:
			// Map molecule model id with with chains, residues and atoms index.
			using VecAtomIds	 = std::vector<uint>;
			using MapResidueIds	 = std::map<uint, VecAtomIds>;
			using MapChainIds	 = std::map<uint, MapResidueIds>;
			using MapMoleculeIds = std::map<ID, MapChainIds>;

			Selection() : BaseModel( ID::Model::MODEL_SELECTION ) {}
			~Selection() = default;

			inline const MapMoleculeIds & getItems() const { return _items; }

			void update( const std::vector<ID> & );
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
			MapMoleculeIds _items = MapMoleculeIds();

			void _selectMolecule( const Molecule & );
			void _unselectMolecule( const Molecule & );
			void _selectChain( const Chain & );
			void _unselectChain( const Chain & );
			void _selectResidue( const Residue & );
			void _unselectResidue( const Residue & );
			void _selectAtom( const Atom & );
			void _unselectAtom( const Atom & );

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
