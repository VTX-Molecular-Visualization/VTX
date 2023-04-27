#ifndef __VTX_UI_WIDGET_SEQUENCE_CHAIN_DATA__
#define __VTX_UI_WIDGET_SEQUENCE_CHAIN_DATA__

#include "chain_sequence_builder.hpp"
#include "dataset/sequence_dataset.hpp"
#include <QString>
#include <app/model/chain.hpp>
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
		inline const Model::Chain &	   getChain() const { return _chain; };

		Model::Residue * const getResidueFromCharIndex( const uint p_charIndex ) const;
		Model::Residue * const getClosestResidueFromCharIndex( const uint p_charIndex, const bool takeForward ) const;
		uint				   getCharIndex( const uint p_residueIndex ) const;
		uint				   getPaintCharIndex( const uint p_residueIndex ) const;
		uint				   getPaintLength( const uint p_localResidueIndex ) const;
		inline uint			   getCharCount() const { return _chainSequenceBuilder.getCharCount(); };

	  private:
		const Model::Molecule & _molecule;
		const Model::Chain &	_chain;
		const uint				_fromResidue;
		const uint				_toResidue;

		Model::Residue * const _getResidue( const uint p_localResidueIndex ) const;

		Dataset::SequenceDisplayDataset * const getDataset( const uint p_residueIndex ) const;
		Dataset::SequenceDisplayDataset * const getDataset_recursive(
			const std::vector<Dataset::SequenceDisplayDataset *> p_vec,
			const uint											 p_residueIndex,
			const uint											 p_indexMin,
			const uint											 p_indexMax,
			const bool											 p_minHasChanged ) const;

		void _generateString();

	  private:
		ChainSequenceBuilder _chainSequenceBuilder;
		QString				 _strSequence;
		QString				 _strScale;
	};

} // namespace VTX::UI::Widget::Sequence
#endif
