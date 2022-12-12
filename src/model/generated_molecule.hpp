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

	  public:
		inline static const int ALL_FRAMES_INDEX		   = -1;
		inline static const int ALL_FRAMES_SEPARATED_INDEX = -2;

	  private:
		inline static const std::string COPY_PREFIX	   = "Copy of ";
		inline static const std::string COPY_SUFFIX	   = "";
		inline static const std::string EXTRACT_PREFIX = "Extract of ";
		inline static const std::string EXTRACT_SUFFIX = "";

		struct BondExtractData
		{
		  public:
			BondExtractData( Model::Bond * const p_bond, const uint p_previousBondIndex ) :
				_bond( p_bond ), _previousBondIndex( p_previousBondIndex ), _sourceMolecule( p_bond->getMoleculePtr() ),
				_sourceFirstIndex( p_bond->getIndexFirstAtom() ), _sourceSecondIndex( p_bond->getIndexSecondAtom() ) {};

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

			uint getSourceFirstIndex() const { return _sourceFirstIndex; };
			uint getSourceSecondIndex() const { return _sourceSecondIndex; };

			bool					hasToBeExtracted() const { return _firstIndexLinked && _secondIndexLinked; };
			Model::Molecule * const getSourceMolecule() const { return _sourceMolecule; };

		  private:
			Model::Molecule * _sourceMolecule = nullptr;
			Model::Bond *	  _bond			  = nullptr;
			uint			  _previousBondIndex;
			uint			  _sourceFirstIndex	 = -1;
			uint			  _sourceSecondIndex = -1;

			uint _newFirstIndex		= 0;
			uint _newSecondIndex	= 0;
			bool _firstIndexLinked	= false;
			bool _secondIndexLinked = false;
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
		void copyFromSelection( const Model::Selection & p_selection,
								const Model::ID &		 p_moleculeID,
								const int				 p_frame = ALL_FRAMES_INDEX );
		void copyFromMolecule( const Model::Molecule & p_molecule, const int p_frame = ALL_FRAMES_INDEX );
		void copyFromChain( const Model::Chain & p_chain, const int p_frame = ALL_FRAMES_INDEX );
		void copyFromResidue( const Model::Residue & p_residue, const int p_frame = ALL_FRAMES_INDEX );
		void copyFromAtom( const Model::Atom & p_atom, const int p_frame = ALL_FRAMES_INDEX );

		void extractFromSelection( const Model::Selection & p_selection, const Model::ID & p_moleculeID );
		void extractChain( const Model::Chain & p_chain );
		void extractResidue( const Model::Residue & p_residue );
		void extractAtom( const Model::Atom & p_atom );

		void copyFrame( const Model::Molecule & p_molecule, const int p_frame );

	  protected:
		GeneratedMolecule();

	  private:
		void _copyMoleculeData( const Model::Molecule & p_molecule,
								const int				p_frame		 = ALL_FRAMES_INDEX,
								const std::string &		p_namePrefix = "",
								const std::string &		p_nameSuffix = "" );
		void _copyChainData( Model::Chain & p_chain, const Model::Chain & p_chainSource );
		void _copyResidueData( Model::Residue &		  p_residue,
							   const Model::Residue & p_residueSource,
							   Model::Chain * const	  p_parent );
		void _copyAtomData( Model::Atom & p_atom, const Model::Atom & p_atomSource, Model::Residue * const p_parent );

		void _computeBonds( const Model::Molecule & p_source );
		void _clearPendingExtractedBonds();
		void _validateBuffers();

		void			 _copyFullMolecule( const Model::Molecule & p_moleculeSource,
											const int				p_frame	 = ALL_FRAMES_INDEX,
											const std::string &		p_prefix = "",
											const std::string &		p_suffix = "" );
		Model::Chain &	 _copyFullChain( const Model::Chain &	 p_source,
										 const Model::Molecule & p_moleculeSource,
										 const int				 p_frame = ALL_FRAMES_INDEX );
		Model::Residue & _copyFullResidue( const Model::Residue &  p_source,
										   const Model::Molecule & p_moleculeSource,
										   Model::Chain * const	   p_parent,
										   const int			   p_frame = ALL_FRAMES_INDEX );
		Model::Atom &	 _copyAtom( const Model::Atom &		p_source,
									const Model::Molecule & p_moleculeSource,
									Model::Residue * const	p_parent,
									const int				p_frame = ALL_FRAMES_INDEX );

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
		void _updateBondInfos( const uint p_bondID, const uint p_firstAtomIndex, const uint p_secondAtomIndex );

		std::vector<ExternalBondExtractData>::iterator _findInPendingExternalBond( const Model::Bond & p_bond );
		void _applyExternalBond( const ExternalBondExtractData & p_externalBondData );

		std::string _getFrameCopyPrefix( const int p_frame ) const;
		std::string _getFrameCopySuffix( const int p_frame ) const;

		std::map<const uint, const uint>	 _mapAtomIds		   = std::map<const uint, const uint>();
		std::vector<ExternalBondExtractData> _pendingExternalBonds = std::vector<ExternalBondExtractData>();
	};
} // namespace VTX::Model

#endif
