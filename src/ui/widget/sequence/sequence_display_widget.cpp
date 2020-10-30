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
					_charIndexResidueIndexMap.clear();

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
							symbol = QString::fromStdString( " " + residue.getSymbolStr() + " " );

						_charIndexResidueIndexMap.emplace( sequenceTxt.size() - richTextTagSize, residueIndex );
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

					const int start = selectionStart();
					const int end	= start + selectedText().size();

					for ( int i = start; i < end; i++ )
					{
						auto it = _charIndexResidueIndexMap.find( i );
						if ( it != _charIndexResidueIndexMap.end() )
							_selection.emplace_back( _charIndexResidueIndexMap[ i ] );
					}
				}

			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI

} // namespace VTX
