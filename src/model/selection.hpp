#ifndef __VTX_MODEL_SELECTION__
#define __VTX_MODEL_SELECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_model.hpp"
#include "define.hpp"
#include "event/base_event_receiver_vtx.hpp"
#include "event/event.hpp"
#include "model/atom.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include <map>
#include <vector>

namespace VTX
{
	namespace Model
	{
		class Selection : public BaseModel, public VTX::Event::BaseEventReceiverVTX
		{
		  public:
			// Map molecule model id with with chains, residues and atoms index.
			using VecAtomIds	 = std::vector<uint>;
			using MapResidueIds	 = std::map<uint, VecAtomIds>;
			using MapChainIds	 = std::map<uint, MapResidueIds>;
			using MapMoleculeIds = std::map<ID, MapChainIds>;

			Selection() : BaseModel( ID::Model::MODEL_SELECTION ) { _registerEvent( Event::MOLECULE_REMOVED ); }
			~Selection() = default;

			inline const MapMoleculeIds & getItems() const { return _items; }
			inline MapMoleculeIds &		  getItems() { return _items; }

			void selectMolecule( Molecule & );
			void selectMolecules( std::vector<Molecule *> & );
			void unselectMolecule( Molecule & );
			void unselectMolecules( std::vector<Molecule *> & );
			void selectChain( Chain & );
			void selectChains( std::vector<Chain *> & );
			void unselectChain( Chain & );
			void unselectChains( std::vector<Chain *> & );
			void selectResidue( Residue & );
			void selectResidues( std::vector<Residue *> & );
			void unselectResidue( Residue & );
			void unselectResidues( std::vector<Residue *> & );
			void selectAtom( Atom & );
			void selectAtoms( std::vector<Atom *> & );
			void unselectAtom( Atom & );
			void unselectAtoms( std::vector<Atom *> & );

			void clear();

			void receiveEvent( const Event::VTXEvent & p_event ) override;

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
