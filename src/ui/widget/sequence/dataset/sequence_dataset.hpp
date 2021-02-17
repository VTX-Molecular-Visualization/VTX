#ifndef __VTX_UI_WIDGET_SEQUENCE_DATASET__
#define __VTX_UI_WIDGET_SEQUENCE_DATASET__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/residue.hpp"
#include <QPoint>
#include <QString>

namespace VTX::UI::Widget::Sequence::Dataset
{
	class SequenceDisplayDataset
	{
	  public:
		SequenceDisplayDataset( const uint p_startIndexChar, const uint p_charCount ) :
			_startIndexChar( p_startIndexChar ), _charCount( p_charCount ) {};

		virtual void appendToSequence( QString & p_sequenceString ) const {};

		virtual const QString & getString() const { return _str; };
		virtual int				getStringSize() const { return _charCount; };

		virtual void appendToScale( QString & p_scale, uint & p_lastIndexCharWritten, const bool p_startBloc ) const {};
		virtual const bool isFinishingBlock( bool p_startBlock ) const { return false; };

		virtual bool isCharIndexInScope( const uint p_charIndex )
		{
			return _startIndexChar <= p_charIndex && p_charIndex < ( _startIndexChar + _charCount );
		}
		virtual Model::Residue * const getResidueAtCharIndex( const uint p_charIndex ) { return nullptr; };

		virtual bool				   isResidueInScope( const uint p_residueIndex ) const { return false; };
		virtual uint				   getCharIndexOfResidue( const uint p_residueIndex ) const { return 0; };
		virtual Model::Residue * const getFirstResidue() const { return nullptr; };
		virtual Model::Residue * const getLastResidue() const { return nullptr; };

		virtual uint getPaintCharIndex( const uint p_charIndex ) const { return getCharIndexOfResidue( p_charIndex ); };
		virtual uint getPaintLength( const uint p_charIndex ) const { return 0; };
		uint		 getLastCharIndex() const { return _startIndexChar + _charCount - 1; };

	  protected:
		uint	_startIndexChar;
		uint	_charCount;
		QString _str;

		inline uint _drawInScale( QString &			  p_scale,
								  const std::string & p_label,
								  const uint		  p_charIndex,
								  const bool		  p_center ) const
		{
			const uint indexTxtSize = (uint)p_label.size();
			const uint indexOffset	= p_center ? ( indexTxtSize - 1 ) / 2 : 0;

			for ( uint j = 0; j < indexTxtSize; j++ )
			{
				const uint index = p_charIndex + j - indexOffset;
				p_scale[ index ] = p_label[ j ];
			}

			return p_charIndex - indexOffset + indexTxtSize - 1;
		}
	};

} // namespace VTX::UI::Widget::Sequence::Dataset
#endif
