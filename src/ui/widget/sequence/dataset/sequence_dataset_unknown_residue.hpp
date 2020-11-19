#ifndef __VTX_UI_WIDGET_SEQUENCE_UNKNOWN_DATASET__
#define __VTX_UI_WIDGET_SEQUENCE_UNKNOWN_DATASET__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/chain.hpp"
#include "sequence_dataset.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				namespace Dataset
				{
					class SequenceDisplayDataset_UnknownResidue : public SequenceDisplayDataset
					{
					  public:
						SequenceDisplayDataset_UnknownResidue( Model::Residue & p_residue,
															   const bool		p_spaceBefore,
															   const bool		p_spaceAfter,
															   const uint		p_startIndexChar,
															   const uint		p_residueIndex );

						const void			   appendToScale( QString & p_scale, bool p_startBloc ) const override;
						Model::Residue * const getResidueAtCharIndex( const uint p_charIndex ) override;

						virtual bool isResidueInScope( const uint p_residueIndex ) const
						{
							return p_residueIndex == ( _residue->getIndex() - _residue->getChainPtr()->getIndexFirstResidue() );
						};
						virtual uint getCharIndexOfResidue( const uint p_residueIndex ) const { return _startIndexChar; };

						Model::Residue * const getFirstResidue( const uint p_charIndex ) const override { return _residue; };
						Model::Residue * const getLastResidue( const uint p_charIndex ) const override { return _residue; };

						uint getPaintLength( const uint p_charIndex ) const override { return 3; };
						uint getPaintCharIndex( const uint p_charIndex ) const override { return _startIndexChar + _scaleIndexPosition - 1; };

					  private:
						Model::Residue * const _residue;
						uint				   _residueIndex;
						uint				   _scaleIndexPosition;
					};
				} // namespace Dataset
			}	  // namespace Sequence
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
#endif
