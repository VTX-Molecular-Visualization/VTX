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
#include "model/representation/representation.hpp"
#include "model/residue.hpp"
#include <map>
#include <set>
#include <vector>

namespace VTX
{
	namespace Model
	{
		class Selection : public BaseModel, public VTX::Event::BaseEventReceiverVTX
		{
			VTX_MODEL

		  public:
			// Map molecule model id with with chains, residues and atoms index.
			using VecAtomIds	 = std::vector<uint>;
			using MapResidueIds	 = std::map<uint, VecAtomIds>;
			using MapChainIds	 = std::map<uint, MapResidueIds>;
			using MapMoleculeIds = std::map<ID, MapChainIds>;

			inline const MapMoleculeIds & getItems() const { return _items; }
			inline MapMoleculeIds &		  getItems() { return _items; }

			inline const std::set<Representation::BaseRepresentation *> & getRepresentations() const { return _representations; }
			inline std::set<Representation::BaseRepresentation *> &		  getRepresentations() { return _representations; }

			void selectMolecule( Molecule &, const bool p_appendToSelection = false );
			void selectMolecules( const std::vector<Molecule *> &, const bool p_appendToSelection = false );
			void unselectMolecule( Molecule & );
			void unselectMolecules( const std::vector<Molecule *> & );
			void unselectMoleculesWithCheck( const std::vector<Molecule *> & );
			bool isMoleculeSelected( const Molecule & ) const;
			uint getMoleculeSelectedCount() const;

			void selectChain( Chain &, const bool p_appendToSelection = false );
			void selectChains( const std::vector<Chain *> &, const bool p_appendToSelection = false );
			void unselectChain( Chain & );
			void unselectChains( const std::vector<Chain *> & );
			void unselectChainsWithCheck( const std::vector<Chain *> & );
			bool isChainSelected( const Chain & ) const;
			uint getChainSelectedCount() const;

			void selectResidue( Residue &, const bool p_appendToSelection = false );
			void selectResidues( const std::vector<Residue *> &, const bool p_appendToSelection = false );
			void unselectResidue( Residue & );
			void unselectResidues( const std::vector<Residue *> & );
			void unselectResiduesWithCheck( const std::vector<Residue *> & );
			bool isResidueSelected( const Residue & ) const;
			uint getResidueSelectedCount() const;

			void selectAtom( Atom &, const bool p_appendToSelection = false );
			void selectAtoms( const std::vector<Atom *> &, const bool p_appendToSelection = false );
			void unselectAtom( Atom & );
			void unselectAtoms( const std::vector<Atom *> & );
			void unselectAtomsWithCheck( const std::vector<Atom *> & );
			bool isAtomSelected( const Atom & ) const;
			uint getAtomSelectedCount() const;

			void selectRepresentation( Representation::BaseRepresentation &, const bool p_appendToSelection = false );
			void selectRepresentations( const std::vector<Representation::BaseRepresentation *> &, const bool p_appendToSelection = false );
			void unselectRepresentation( Representation::BaseRepresentation & );
			void unselectRepresentations( const std::vector<Representation::BaseRepresentation *> & );
			void unselectRepresentationsWithCheck( const std::vector<Representation::BaseRepresentation *> & );
			bool isRepresentationSelected( Representation::BaseRepresentation & ) const;
			uint getRepresentationSelectedCount() const;

			void clear();

			void receiveEvent( const Event::VTXEvent & p_event ) override;

		  private:
			MapMoleculeIds								   _items			= MapMoleculeIds();
			std::set<Representation::BaseRepresentation *> _representations = std::set<Representation::BaseRepresentation *>();

			Selection() : BaseModel( ID::Model::MODEL_SELECTION ) { _registerEvent( Event::MOLECULE_REMOVED ); }
			~Selection() = default;

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

			void _selectRepresentation( Representation::BaseRepresentation & );
			void _unselectRepresentation( Representation::BaseRepresentation & );

			void _clearWithoutNotify();

			void _refreshMoleculeSelection( Molecule * const );
			void _notifyDataChanged();
		};

	} // namespace Model
} // namespace VTX
#endif
