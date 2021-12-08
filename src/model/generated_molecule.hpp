#ifndef __VTX_MODEL_GENERATED_MOLECULE__
#define __VTX_MODEL_GENERATED_MOLECULE__

#include "bond.hpp"
#include "molecule.hpp"
#include <vector>

namespace VTX::Model
{
	class Selection;
	class GeneratedMolecule : public Molecule
	{
		VTX_MODEL

	  private:
		struct BondExtractData
		{
		  public:
			BondExtractData( Model::Bond * const p_bond, const uint p_previousBondIndex ) :
				_bond( p_bond ), _previousBondIndex( p_previousBondIndex ) {};

			Model::Bond * const getBond() const { return _bond; };
			const uint			getPreviousBondIndex() const { return _previousBondIndex; };
			void				setFirstIndex( const uint p_index )
			{
				_newFirstIndex	  = p_index;
				_firstIndexLinked = true;
			}
			void setSecondIndex( const uint p_index )
			{
				_newSecondIndex	   = p_index;
				_secondIndexLinked = true;
			}

			uint getFirstIndex() const { return _newFirstIndex; };
			uint getSecondIndex() const { return _newSecondIndex; };
			bool isFirstIndexLinked() const { return _firstIndexLinked; };
			bool isSecondIndexLinked() const { return _secondIndexLinked; };
			bool hasToBeExtracted() const { return _firstIndexLinked && _secondIndexLinked; };

		  private:
			Model::Bond * _bond;
			uint		  _previousBondIndex;
			uint		  _newFirstIndex	 = 0;
			uint		  _newSecondIndex	 = 0;
			bool		  _firstIndexLinked	 = false;
			bool		  _secondIndexLinked = false;
		};
		struct ExternalBondExtractData : BondExtractData
		{
		  public:
			ExternalBondExtractData( Model::Bond * const p_bond, const uint p_previousBondIndex ) :
				BondExtractData( p_bond, p_previousBondIndex ) {};

			uint getIndexInExtractedResidue() const { return _indexInExtractedResidue; };
			void setIndexInExtractedResidue( const uint p_indexInExtractedResidue )
			{
				_indexInExtractedResidue = p_indexInExtractedResidue;
			};

		  private:
			uint _indexInExtractedResidue = -1;
		};

	  public:
		void copyFromSelection( const Model::Selection & p_selection, const Model::ID & p_moleculeID );
		void copyFromMolecule( const Model::Molecule & p_molecule );
		void copyFromChain( const Model::Chain & p_chain );
		void copyFromResidue( const Model::Residue & p_residue );
		void copyFromAtom( const Model::Atom & p_atom );

		void extractFromSelection( const Model::Selection & p_selection, const Model::ID & p_moleculeID );
		void extractChain( const Model::Chain & p_chain );
		void extractResidue( const Model::Residue & p_residue );
		void extractAtom( const Model::Atom & p_atom );

	  protected:
		GeneratedMolecule();

	  private:
		void _copyMoleculeData( const Model::Molecule & p_molecule, const std::string & p_namePrefix );
		void _copyChainData( Model::Chain & p_chain, const Model::Chain & p_chainSource );
		void _copyResidueData( Model::Residue &		  p_residue,
							   const Model::Residue & p_residueSource,
							   Model::Chain * const	  p_parent );
		void _copyAtomData( Model::Atom & p_atom, const Model::Atom & p_atomSource, Model::Residue * const p_parent );

		void _computeBonds( const Model::Molecule & p_source, std::map<const uint, const uint> & p_mapAtomIds );
		void _clearPendingExtractedBonds();
		void _validateBuffers();

		void			 _copyFullMolecule( const Model::Molecule &			   p_moleculeSource,
											std::map<const uint, const uint> & p_mapAtomIds );
		Model::Chain &	 _copyFullChain( const Model::Chain &				p_source,
										 const Model::Molecule &			p_moleculeSource,
										 std::map<const uint, const uint> & p_mapAtomIds );
		Model::Residue & _copyFullResidue( const Model::Residue &			  p_source,
										   const Model::Molecule &			  p_moleculeSource,
										   Model::Chain * const				  p_parent,
										   std::map<const uint, const uint> & p_mapAtomIds );
		Model::Atom &	 _copyAtom( const Model::Atom &		p_source,
									const Model::Molecule & p_moleculeSource,
									Model::Residue * const	p_parent );

		Model::Chain & _extractFullChain( Model::Molecule & p_fromMolecule, const uint p_index );

		void _extractAllResidues( Model::Molecule & p_fromMolecule, const uint p_startIndex, const uint p_count );
		Model::Residue & _extractFullResidue( Model::Molecule &	   p_fromMolecule,
											  const uint		   p_index,
											  Model::Chain * const p_parent );

		void _extractAllAtoms( Model::Molecule & p_fromMolecule, const uint p_startIndex, const uint p_count );
		void _extractAtomsFromResidue( Model::Molecule &	  p_fromMolecule,
									   Model::Residue * const p_parent,
									   const uint			  p_startIndex,
									   const uint			  p_count,
									   bool					  p_parentFromMolecule );
		void _extractAtomsFromResidue( Model::Molecule &		 p_fromMolecule,
									   Model::Residue * const	 p_parent,
									   const std::vector<uint> & p_indexes,
									   bool						 p_parentFromMolecule );

		void _extractBond( const BondExtractData & p_bondData );

		std::vector<ExternalBondExtractData>::iterator _findInPendingExternalBond( const Model::Bond & p_bond );
		void _applyExternalBond( const ExternalBondExtractData & p_externalBondData );

		std::vector<ExternalBondExtractData> _pendingExternalBonds = std::vector<ExternalBondExtractData>();
	};
} // namespace VTX::Model

#endif
