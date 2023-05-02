#ifndef __VTX_UI_WIDGET_SEQUENCE_CHAIN_DATA__
#define __VTX_UI_WIDGET_SEQUENCE_CHAIN_DATA__

#include "chain_sequence_builder.hpp"
#include "dataset/sequence_dataset.hpp"
#include <QString>
#include <app/component/chemistry/_fwd.hpp>
#include <vector>

namespace VTX::UI::Widget::Sequence
{
	class SequenceChainData
	{
	  public:
		SequenceChainData( const App::Component::Chemistry::Chain & p_chain );
		~SequenceChainData();

		const QString & getSequenceString() const { return _strSequence; };
		const QString & getScale() const { return _strScale; }

		inline uint getResidueCount() const { return _chain.getResidueCount(); };
		inline uint getIndexFirstResidue() const { return _chain.getIndexFirstResidue(); };
		inline App::Component::Chemistry::Molecule * const getMoleculePtr() const { return _chain.getMoleculePtr(); };
		inline uint										   getChainIndex() const { return _chain.getIndex(); };
		inline const App::Component::Chemistry::Chain &	   getChain() const { return _chain; };

		App::Component::Chemistry::Residue * const getResidueFromCharIndex( const uint p_charIndex ) const;
		App::Component::Chemistry::Residue * const getClosestResidueFromCharIndex( const uint p_charIndex,
																				   const bool takeForward ) const;
		uint									   getCharIndex( const uint p_residueIndex ) const;
		uint									   getPaintCharIndex( const uint p_residueIndex ) const;
		uint									   getPaintLength( const uint p_localResidueIndex ) const;
		inline uint getCharCount() const { return _chainSequenceBuilder.getCharCount(); };

	  private:
		const App::Component::Chemistry::Molecule & _molecule;
		const App::Component::Chemistry::Chain &	_chain;
		const uint									_fromResidue;
		const uint									_toResidue;

		App::Component::Chemistry::Residue * const _getResidue( const uint p_localResidueIndex ) const;

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
