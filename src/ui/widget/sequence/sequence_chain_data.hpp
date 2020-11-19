#ifndef __VTX_UI_WIDGET_SEQUENCE_CHAIN_DATA__
#define __VTX_UI_WIDGET_SEQUENCE_CHAIN_DATA__

#ifdef _MSC_VER
#pragma once
#endif

#include "dataset/sequence_dataset.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include <QString>
#include <list>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				class SequenceChainData
				{
				  public:
					SequenceChainData( const Model::Chain & p_chain ) : _chain( p_chain ), _molecule( *( p_chain.getMoleculePtr() ) )
					{
						_generateDataSet();
						_generateString();
					};
					~SequenceChainData()
					{
						for ( auto it : _dataset )
							delete it;

						_dataset.clear();
					}

					const QString & getSequenceString() const { return _strSequence; };
					const QString & getScale() const { return _strScale; }

					uint					getResidueCount() const { return _chain.getResidueCount(); };
					uint					getIndexFirstResidue() const { return _chain.getIndexFirstResidue(); };
					Model::Molecule * const getMoleculePtr() const { return _chain.getMoleculePtr(); };
					uint					getChainIndex() const { return _chain.getIndex(); };

					Model::Residue * const getResidueFromCharIndex( const uint p_charIndex ) const;
					Model::Residue * const getClosestResidueFromCharIndex( const uint p_charIndex, const bool takeForward ) const;
					uint				   getCharIndex( const uint p_residueIndex ) const;
					uint				   getPaintCharIndex( const uint p_residueIndex ) const;
					uint				   getPaintLength( const uint p_localResidueIndex ) const;
					uint				   getCharCount() const { return _dataset.back()->getLastCharIndex(); };

				  private:
					const Model::Molecule &						 _molecule;
					const Model::Chain &						 _chain;
					std::list<Dataset::SequenceDisplayDataset *> _dataset = std::list<Dataset::SequenceDisplayDataset *>();

					Model::Residue & _getResidue( const uint p_localResidueIndex ) const
					{
						const uint moleculeResidueIndex = _chain.getIndexFirstResidue() + p_localResidueIndex;
						return _chain.getMoleculePtr()->getResidue( moleculeResidueIndex );
					}

					void _generateDataSet();
					void _generateString();

				  private:
					QString _strSequence;
					QString _strScale;
				};

			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
