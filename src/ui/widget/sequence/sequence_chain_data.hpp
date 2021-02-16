#ifndef __VTX_UI_WIDGET_SEQUENCE_CHAIN_DATA__
#define __VTX_UI_WIDGET_SEQUENCE_CHAIN_DATA__

#ifdef _MSC_VER
#pragma once
#endif

#include "dataset/sequence_dataset.hpp"
#include "model/chain.hpp"
#include <QString>
#include <vector>

namespace VTX::Model
{
	class Molecule;
	class Residue;
} // namespace VTX::Model

namespace VTX::UI::Widget::Sequence
{
	class SequenceChainData
	{
	  public:
		SequenceChainData( const Model::Chain & p_chain );
		~SequenceChainData();

		const QString & getSequenceString() const { return _strSequence; };
		const QString & getScale() const { return _strScale; }

		inline uint					   getResidueCount() const { return _chain.getResidueCount(); };
		inline uint					   getIndexFirstResidue() const { return _chain.getIndexFirstResidue(); };
		inline Model::Molecule * const getMoleculePtr() const { return _chain.getMoleculePtr(); };
		inline uint					   getChainIndex() const { return _chain.getIndex(); };

		Model::Residue * const getResidueFromCharIndex( const uint p_charIndex ) const;
		Model::Residue * const getClosestResidueFromCharIndex( const uint p_charIndex, const bool takeForward ) const;
		uint				   getCharIndex( const uint p_residueIndex ) const;
		uint				   getPaintCharIndex( const uint p_residueIndex ) const;
		uint				   getPaintLength( const uint p_localResidueIndex ) const;
		inline uint			   getCharCount() const { return _dataset.back()->getLastCharIndex(); };

	  private:
		const Model::Molecule &						   _molecule;
		const Model::Chain &						   _chain;
		std::vector<Dataset::SequenceDisplayDataset *> _dataset = std::vector<Dataset::SequenceDisplayDataset *>();

		uint _emplaceResidueDataSet( const uint p_localCharIndex,
									 const uint p_startResidueIndex,
									 const uint p_endResidueIndex );

		Model::Residue * const _getResidue( const uint p_localResidueIndex ) const;

		Dataset::SequenceDisplayDataset * const getDataset( const uint p_residueIndex ) const;
		Dataset::SequenceDisplayDataset * const getDataset_recursive(
			const std::vector<Dataset::SequenceDisplayDataset *> p_vec,
			const uint											 p_residueIndex,
			const uint											 p_indexMin,
			const uint											 p_indexMax,
			const bool											 p_minHasChanged ) const;

		void _generateDataSet();
		void _generateString();

	  private:
		QString _strSequence;
		QString _strScale;
	};

} // namespace VTX::UI::Widget::Sequence
#endif
