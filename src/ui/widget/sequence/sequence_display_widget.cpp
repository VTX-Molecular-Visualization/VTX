#include "sequence_display_widget.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				void SequenceDisplayWidget::mousePressEvent( QMouseEvent * ev )
				{
					_updateSelectionData();
					QLabel::mousePressEvent( ev );
				}
				void SequenceDisplayWidget::mouseReleaseEvent( QMouseEvent * ev )
				{
					QLabel::mouseReleaseEvent( ev );

					if ( _lastHasSelection != hasSelectedText() || _lastFirstIndexSelection != selectionStart() || _lastSelection != selectedText() )
					{
						_buildSelection();
						emit selectionChanged();
					}
				};

				void SequenceDisplayWidget::setupSequence( const Model::Chain & p_chain )
				{
					_chain = &p_chain;

					QString sequenceTxt = QString();

					const uint residueCount = _chain->getResidueCount();

					int					  richTextTagSize = 0;
					const VTX::Color::Rgb currentColor	  = _chain->getColor();
					const QString		  colorString	  = QString::fromStdString( currentColor.toHexaString() );
					const QString		  htmlColorTag	  = "<font color=" + colorString + ">";
					richTextTagSize += htmlColorTag.size();
					sequenceTxt.append( htmlColorTag );

					for ( uint i = 0; i < residueCount; ++i )
					{
						const uint			   residueIndex = _chain->getIndexFirstResidue() + i;
						const Model::Residue & residue		= _chain->getMoleculePtr()->getResidue( residueIndex );

						/*VTX::Color::Rgb residueColor = residue.getColor();

						if ( currentColor != residueColor )
						{
							htmlColorTag = "</font>";
							richTextTagSize += htmlColorTag.size();
							sequenceTxt.append( htmlColorTag );
							colorString	 = QString::fromStdString( residueColor.toHexaString() );
							htmlColorTag = "<font color=" + colorString + ">";
							richTextTagSize += htmlColorTag.size();
							sequenceTxt.append( htmlColorTag );

							currentColor = residueColor;
						}*/

						QString symbol;
						if ( residue.getSymbolShort() != "?" )
							symbol = QString::fromStdString( residue.getSymbolShort() );
						else
						{
							const uint posInString = i + (uint)_positionUnknownResidues.size() * Model::Residue::SYMBOL_STR_SIZE;
							_positionUnknownResidues.emplace_back( posInString );

							symbol = QString::fromStdString( " " + residue.getSymbolStr() + " " );
						}

						sequenceTxt.append( symbol );
					}

					sequenceTxt.append( "</font>" );
					setText( sequenceTxt );

					_selection.reserve( residueCount );
				}

				void SequenceDisplayWidget::_buildSelection()
				{
					_selection.clear();

					if ( !hasSelectedText() )
						return;

					const uint strStartIndex = selectionStart();
					uint	   residueIndex	 = _chain->getIndexFirstResidue() + strStartIndex;
					const uint strEndIndex	 = strStartIndex + selectedText().size();

					int unknownResidueCounter = 0;

					for ( int i = 0; i < _positionUnknownResidues.size(); i++ )
					{
						if ( _positionUnknownResidues[ i ] < strStartIndex )
						{
							residueIndex -= Model::Residue::SYMBOL_STR_SIZE;
							unknownResidueCounter++;
						}
						else
						{
							break;
						}
					}

					for ( uint strIndex = strStartIndex; strIndex < strEndIndex; strIndex++ )
					{
						_selection.emplace_back( residueIndex );
						residueIndex++;

						if ( _positionUnknownResidues.size() > unknownResidueCounter && strIndex == _positionUnknownResidues[ unknownResidueCounter ] )
						{
							unknownResidueCounter++;
							strIndex += Model::Residue::SYMBOL_STR_SIZE;
						}
					}
				}

			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI

} // namespace VTX
