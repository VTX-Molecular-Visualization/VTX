#ifndef __VTX_UI_WIDGET_SEQUENCE_DATASET__
#define __VTX_UI_WIDGET_SEQUENCE_DATASET__

#include "ui/old_ui/style.hpp"
#include <QPoint>
#include <QString>
#include <app/model/residue.hpp>
#include <string>

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
		virtual uint getLastScaleCharIndex() const { return _charCount; };

	  protected:
		uint	_startIndexChar;
		uint	_charCount;
		QString _str;

		uint _drawInScale( QString &		   p_scale,
						   const std::string & p_label,
						   const uint		   p_charIndex,
						   const bool		   p_center ) const;

		uint _getStepToNextValidIndex( const int p_originalIndex ) const;
	};

} // namespace VTX::UI::Widget::Sequence::Dataset
#endif
