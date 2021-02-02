#ifndef __VTX_MODEL_GENERATED_MOLECULE__
#define __VTX_MODEL_GENERATED_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "bond.hpp"
#include "molecule.hpp"

namespace VTX
{
	namespace Model
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
				const uint			_previousBondIndex;
				Model::Bond * const _bond;
				uint				_newFirstIndex	   = 0;
				uint				_newSecondIndex	   = 0;
				bool				_firstIndexLinked  = false;
				bool				_secondIndexLinked = false;
			};

		  public:
			void copyFromSelection( const Model::Selection & p_selection );
			void extractFromSelection( const Model::Selection & p_selection );

		  protected:
			GeneratedMolecule();

		  private:
			void _copyMoleculeData( const Model::Molecule & p_molecule, const std::string & p_namePrefix );
			void _copyChainData( Model::Chain & p_chain, const Model::Chain & p_chainSource );
			void _copyResidueData( Model::Residue &		  p_residue,
								   const Model::Residue & p_residueSource,
								   Model::Chain * const	  p_parent );
			void _copyAtomData( Model::Atom &		   p_atom,
								const Model::Atom &	   p_atomSource,
								Model::Residue * const p_parent );

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

			void _updateExternalBondsOfResidue( Model::Molecule &	   p_fromMolecule,
												const Model::Residue & p_fromResidue,
												const uint			   p_offsetAtomIndex );
			void _extractBond( const BondExtractData & p_bondData );

			std::vector<BondExtractData> _externalBondExtractData = std::vector<BondExtractData>();
		};
	} // namespace Model
} // namespace VTX

#endif
