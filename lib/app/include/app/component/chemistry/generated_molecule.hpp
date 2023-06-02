#ifndef __VTX_APP_COMPONENT_CHEMISTRY_GENERATED_MOLECULE__
#define __VTX_APP_COMPONENT_CHEMISTRY_GENERATED_MOLECULE__

#include "_fwd.hpp"
#include "app/application/selection/_fwd.hpp"
#include "molecule.hpp"
#include <map>
#include <string>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Component::Chemistry
{
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
			BondExtractData( Chemistry::Bond * const p_bond, const uint p_previousBondIndex );

			Chemistry::Bond * const getBond() const { return _bond; };
			const uint				getPreviousBondIndex() const { return _previousBondIndex; };
			void					setFirstIndex( const uint p_index )
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

			bool						hasToBeExtracted() const { return _firstIndexLinked && _secondIndexLinked; };
			Chemistry::Molecule * const getSourceMolecule() const { return _sourceMolecule; };

		  private:
			Chemistry::Molecule * _sourceMolecule = nullptr;
			Chemistry::Bond *	  _bond			  = nullptr;
			uint				  _previousBondIndex;
			uint				  _sourceFirstIndex	 = -1;
			uint				  _sourceSecondIndex = -1;

			uint _newFirstIndex		= 0;
			uint _newSecondIndex	= 0;
			bool _firstIndexLinked	= false;
			bool _secondIndexLinked = false;
		};
		struct ExternalBondExtractData : BondExtractData
		{
		  public:
			ExternalBondExtractData( Chemistry::Bond * const p_bond, const uint p_previousBondIndex ) :
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
		void copyFromSelection( const App::Application::Selection::SelectionModel & p_selection,
								const App::Core::Model::ID &						p_moleculeID,
								const int											p_frame = ALL_FRAMES_INDEX );
		void copyFromMolecule( const Chemistry::Molecule & p_molecule, const int p_frame = ALL_FRAMES_INDEX );
		void copyFromChain( const Chemistry::Chain & p_chain, const int p_frame = ALL_FRAMES_INDEX );
		void copyFromResidue( const Chemistry::Residue & p_residue, const int p_frame = ALL_FRAMES_INDEX );
		void copyFromAtom( const Chemistry::Atom & p_atom, const int p_frame = ALL_FRAMES_INDEX );

		void extractFromSelection( const App::Application::Selection::SelectionModel & p_selection,
								   const App::Core::Model::ID &						   p_moleculeID );
		void extractChain( const Chemistry::Chain & p_chain );
		void extractResidue( const Chemistry::Residue & p_residue );
		void extractAtom( const Chemistry::Atom & p_atom );

		void copyFrame( const Chemistry::Molecule & p_molecule, const int p_frame );

	  protected:
		GeneratedMolecule();

	  private:
		void _copyMoleculeData( const Chemistry::Molecule & p_molecule,
								const int					p_frame		 = ALL_FRAMES_INDEX,
								const std::string &			p_namePrefix = "",
								const std::string &			p_nameSuffix = "" );
		void _copyChainData( Chemistry::Chain & p_chain, const Chemistry::Chain & p_chainSource );
		void _copyResidueData( Chemistry::Residue &		  p_residue,
							   const Chemistry::Residue & p_residueSource,
							   Chemistry::Chain * const	  p_parent );
		void _copyAtomData( Chemistry::Atom &		   p_atom,
							const Chemistry::Atom &	   p_atomSource,
							Chemistry::Residue * const p_parent );

		void _computeBonds( const Chemistry::Molecule & p_source );
		void _clearPendingExtractedBonds();
		void _validateBuffers();

		void				 _copyFullMolecule( const Chemistry::Molecule & p_moleculeSource,
												const int					p_frame	 = ALL_FRAMES_INDEX,
												const std::string &			p_prefix = "",
												const std::string &			p_suffix = "" );
		Chemistry::Chain &	 _copyFullChain( const Chemistry::Chain &	 p_source,
											 const Chemistry::Molecule & p_moleculeSource,
											 const int					 p_frame = ALL_FRAMES_INDEX );
		Chemistry::Residue & _copyFullResidue( const Chemistry::Residue &  p_source,
											   const Chemistry::Molecule & p_moleculeSource,
											   Chemistry::Chain * const	   p_parent,
											   const int				   p_frame = ALL_FRAMES_INDEX );
		Chemistry::Atom &	 _copyAtom( const Chemistry::Atom &		p_source,
										const Chemistry::Molecule & p_moleculeSource,
										Chemistry::Residue * const	p_parent,
										const int					p_frame = ALL_FRAMES_INDEX );

		Chemistry::Chain & _extractFullChain( Chemistry::Molecule & p_fromMolecule, const uint p_index );

		void _extractAllResidues( Chemistry::Molecule & p_fromMolecule, const uint p_startIndex, const uint p_count );
		Chemistry::Residue & _extractFullResidue( Chemistry::Molecule &	   p_fromMolecule,
												  const uint			   p_index,
												  Chemistry::Chain * const p_parent );

		void _extractAllAtoms( Chemistry::Molecule & p_fromMolecule, const uint p_startIndex, const uint p_count );
		void _extractAtomsFromResidue( Chemistry::Molecule &	  p_fromMolecule,
									   Chemistry::Residue * const p_parent,
									   const uint				  p_startIndex,
									   const uint				  p_count,
									   bool						  p_parentFromMolecule );
		void _extractAtomsFromResidue( Chemistry::Molecule &	  p_fromMolecule,
									   Chemistry::Residue * const p_parent,
									   const std::vector<uint> &  p_indexes,
									   bool						  p_parentFromMolecule );

		void _extractBond( const BondExtractData & p_bondData );
		void _updateBondInfos( const uint p_bondID, const uint p_firstAtomIndex, const uint p_secondAtomIndex );

		std::vector<ExternalBondExtractData>::iterator _findInPendingExternalBond( const Chemistry::Bond & p_bond );
		void _applyExternalBond( const ExternalBondExtractData & p_externalBondData );

		std::string _getFrameCopyPrefix( const int p_frame ) const;
		std::string _getFrameCopySuffix( const int p_frame ) const;

		std::map<const uint, const uint>	 _mapAtomIds		   = std::map<const uint, const uint>();
		std::vector<ExternalBondExtractData> _pendingExternalBonds = std::vector<ExternalBondExtractData>();
	};
} // namespace VTX::App::Component::Chemistry

#endif
