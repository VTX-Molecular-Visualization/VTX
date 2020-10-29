#include "sequence_display_widget.hpp"
#include "model/chain.hpp"
#include "model/residue.hpp"
#include <QString>
#include <vector>

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

				void SequenceDisplayWidget::setupSequence( const Model::Molecule & p_molecule )
				{
					_model = &p_molecule;
					_chainsFirstIndex.clear();

					std::vector<Model::Chain *> chains		= p_molecule.getChains();
					QString						sequenceTxt = QString();

					for ( const Model::Chain * const chain : p_molecule.getChains() )
					{
						const QString chainName = QString::fromStdString( "/" + chain->getName() + "/" );
						sequenceTxt.append( chainName );

						_chainsFirstIndex.emplace_back( sequenceTxt.size() );

						for ( uint i = 0; i < chain->getResidueCount(); ++i )
						{
							const Model::Residue & residue = p_molecule.getResidue( chain->getIndexFirstResidue() + i );
							if ( residue.getSymbolShort() != "?" )
							{
								sequenceTxt.append( QString::fromStdString( residue.getSymbolShort() ) );
							}
							else
							{
								sequenceTxt.append( QString::fromStdString( residue.getSymbolStr() + " " ) );
							}
						}

						sequenceTxt.append( '\n' );
					}

					setText( sequenceTxt );
				}

				void SequenceDisplayWidget::_buildSelection()
				{
					_selection.clear();

					if ( !hasSelectedText() )
						return;

					const int start = selectionStart();

					const Model::Chain * chain			   = nullptr;
					uint				 indexFirstResidue = 0;
					for ( int i = (int)_chainsFirstIndex.size() - 1; i >= 0; i-- )
					{
						if ( start >= _chainsFirstIndex[ i ] )
						{
							chain			  = &_model->getChain( i );
							indexFirstResidue = start - _chainsFirstIndex[ i ];
							break;
						}
					}

					if ( chain == nullptr )
						return;

					const int selectionSize = selectedText().size();

					for ( int i = 0; i < selectionSize; i++ )
					{
						_selection.emplace_back( chain->getIndexFirstResidue() + indexFirstResidue + i );
					}
				}

			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI

} // namespace VTX
