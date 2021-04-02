#ifndef __VTX_MODEL_SELECTION__
#define __VTX_MODEL_SELECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_model.hpp"
#include "define.hpp"
#include "event/base_event_receiver_vtx.hpp"
#include "event/event.hpp"
#include "math/aabb.hpp"
#include <map>
#include <set>
#include <vector>

namespace VTX::Model
{
	class Molecule;
	class Chain;
	class Residue;
	class Atom;

	class Selection : public BaseModel, public VTX::Event::BaseEventReceiverVTX
	{
		VTX_MODEL

	  public:
		class VecAtomIds : public std::vector<unsigned int>
		{
			friend Selection;

		  public:
			uint getFullySelectedChildCount() const { return _childrenFullySelectedCount; }

		  private:
			void _setFullChildrenCount( const uint p_count ) { _childrenFullySelectedCount = p_count; };
			void _addFullChild() { _childrenFullySelectedCount++; };
			void _removeFullChild() { _childrenFullySelectedCount--; };

			uint _childrenFullySelectedCount = 0;
		};
		class MapResidueIds : public std::map<Model::ID, VecAtomIds>
		{
			friend Selection;

		  public:
			uint getFullySelectedChildCount() const { return _childrenFullySelectedCount; }

		  private:
			void _setFullChildrenCount( const uint p_count ) { _childrenFullySelectedCount = p_count; };
			void _addFullChild() { _childrenFullySelectedCount++; };
			void _removeFullChild() { _childrenFullySelectedCount--; };

		  private:
			uint _childrenFullySelectedCount = 0;
		};
		class MapChainIds : public std::map<Model::ID, MapResidueIds>
		{
			friend Selection;

		  public:
			uint getFullySelectedChildCount() const { return _childrenFullySelectedCount; }

		  private:
			void _setFullChildrenCount( const uint p_count ) { _childrenFullySelectedCount = p_count; };
			void _addFullChild() { _childrenFullySelectedCount++; };
			void _removeFullChild() { _childrenFullySelectedCount--; };

			uint _childrenFullySelectedCount = 0;
		};
		using MapMoleculeIds = std::map<Model::ID, MapChainIds>;

		inline const MapMoleculeIds & getItems() const { return _items; }
		inline MapMoleculeIds &		  getItems() { return _items; }

		void selectMolecule( Molecule &, const bool p_appendToSelection = false );
		void selectMolecules( const std::vector<Molecule *> &, const bool p_appendToSelection = false );
		void unselectMolecule( Molecule & );
		void unselectMolecules( const std::vector<Molecule *> & );
		void unselectMoleculesWithCheck( const std::vector<Molecule *> & );
		bool isMoleculeSelected( const Molecule & ) const;
		bool isMoleculeFullySelected( const Molecule & ) const;
		uint getMoleculeSelectedCount() const;

		void selectChain( Chain &, const bool p_appendToSelection = false );
		void selectChains( const std::vector<Chain *> &, const bool p_appendToSelection = false );
		void unselectChain( Chain & );
		void unselectChains( const std::vector<Chain *> & );
		void unselectChainsWithCheck( const std::vector<Chain *> & );
		bool isChainSelected( const Chain & ) const;
		bool isChainFullySelected( const Chain & ) const;
		uint getChainSelectedCount() const;

		void selectResidue( Residue &, const bool p_appendToSelection = false );
		void selectResidues( const std::vector<Residue *> &, const bool p_appendToSelection = false );
		void unselectResidue( Residue & );
		void unselectResidues( const std::vector<Residue *> & );
		void unselectResiduesWithCheck( const std::vector<Residue *> & );
		bool isResidueSelected( const Residue & ) const;
		bool isResidueFullySelected( const Residue & ) const;
		uint getResidueSelectedCount() const;

		void selectAtom( Atom &, const bool p_appendToSelection = false );
		void selectAtoms( const std::vector<Atom *> &, const bool p_appendToSelection = false );
		void unselectAtom( Atom & );
		void unselectAtoms( const std::vector<Atom *> & );
		void unselectAtomsWithCheck( const std::vector<Atom *> & );
		bool isAtomSelected( const Atom & ) const;
		uint getAtomSelectedCount() const;

		void selectModels( const std::vector<Model::Molecule *> & p_molecules,
						   const std::vector<Model::Chain *> &	  p_chains,
						   const std::vector<Model::Residue *> &  p_residus,
						   const std::vector<Model::Atom *> &	  p_atoms,
						   const bool							  p_appendToSelection = false );
		void unselectModels( const std::vector<Model::Molecule *> & p_molecules,
							 const std::vector<Model::Chain *> &	p_chains,
							 const std::vector<Model::Residue *> &	p_residus,
							 const std::vector<Model::Atom *> &		p_atoms );

		bool isEmpty() const;
		void clear();

		void receiveEvent( const Event::VTXEvent & p_event ) override;

		void	   getItemTypes( std::set<ID::VTX_ID> & p_types ) const;
		Math::AABB getAABB() const;

	  protected:
		Selection() : BaseModel( ID::Model::MODEL_SELECTION )
		{
			_registerEvent( Event::MOLECULE_REMOVED );
			_registerEvent( Event::MOLECULE_STRUCTURE_CHANGE );
		}
		~Selection() = default;

		void _notifyDataChanged();

	  private:
		MapMoleculeIds					_items			  = MapMoleculeIds();
		std::map<Model::ID, Math::AABB> _mapSelectionAABB = std::map<Model::ID, Math::AABB>();

		void _selectMolecule( const Molecule & );
		void _unselectMolecule( const Molecule & );
		void _selectChain( const Chain & );
		void _unselectChain( const Chain & );
		void _selectResidue( const Residue & );
		void _unselectResidue( const Residue & );
		void _selectAtom( const Atom & );
		void _unselectAtom( const Atom & );

		bool _addMolecule( const Molecule & );
		bool _addChain( const Chain & );
		bool _addResidue( const Residue & );
		bool _addAtom( const Atom & );

		void _addMoleculeContent( const Molecule & );
		void _addChainContent( const Chain & );
		void _addResidueContent( const Residue & );

		void _removeMolecule( const Molecule & );
		void _removeChain( const Chain & );
		void _removeResidue( const Residue & );
		void _removeAtom( const Atom & );

		void _clearWithoutNotify();

		void _refreshMoleculeSelection( Molecule * const );
		void _recomputeAABB();

		void _referenceAtom( const Atom & p_atom );
		void _referenceFullResidue( const Residue & p_residue );
		void _referenceFullChain( const Chain & p_chain );

		void _unreferenceAtom( const Atom & p_atom );
		void _unreferenceFullResidue( const Residue & p_residue );
		void _unreferenceFullChain( const Chain & p_chain );
	};

} // namespace VTX::Model
#endif
