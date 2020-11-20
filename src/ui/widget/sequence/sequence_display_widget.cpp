#include "sequence_display_widget.hpp"
#include "model/molecule.hpp"
#include "selection/selection_manager.hpp"
#include "style.hpp"
#include "util/ui.hpp"
#include "vtx_app.hpp"
#include <QPainter>
#include <QRectF>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				SequenceDisplayWidget::SequenceDisplayWidget( QWidget * p_parent ) : QLabel( p_parent )
				{
					setTextInteractionFlags( Qt::TextInteractionFlag::NoTextInteraction );
					setFont( Style::SEQUENCE_DISPLAY_FONT );

					_fontMetrics = new QFontMetricsF( font() );
				}
				SequenceDisplayWidget::~SequenceDisplayWidget()
				{
					delete _fontMetrics;
					delete _charIndexPaintCache;
					delete _symbolLengthPaintCache;
				}

				void SequenceDisplayWidget::setupSequence( const SequenceChainData * p_dataset )
				{
					_chainData						   = p_dataset;
					const QString sequenceString	   = _chainData->getSequenceString();
					const int	  sequenceStringLength = sequenceString.length();

					setText( sequenceString );
					const QString txt		 = text();
					const int	  textLength = txt.length();
				}

				void SequenceDisplayWidget::mouseDoubleClickEvent( QMouseEvent * p_event )
				{
					const Model::Residue * const residue = _getResidueFromLocaleXPos( p_event->localPos().x() );
					p_event->accept();

					VTX_INFO( "Double click on " + residue->getSymbolName() );
				}

				Model::Residue * const SequenceDisplayWidget::getResidueAtPos( const QPoint & p_pos ) { return _getResidueFromLocaleXPos( p_pos.x() ); }

				Model::Residue * const SequenceDisplayWidget::getClosestResidueFromPos( const QPoint & p_pos, const bool p_takeforward )
				{
					const float charSize  = _fontMetrics->averageCharWidth();
					const uint	charIndex = p_pos.x() / charSize;

					return _chainData->getClosestResidueFromCharIndex( charIndex, p_takeforward );
				}

				Model::Residue * const SequenceDisplayWidget::_getResidueFromLocaleXPos( const int p_localeXPos ) const
				{
					const float charSize  = _fontMetrics->averageCharWidth();
					const uint	charIndex = p_localeXPos / charSize;

					return _chainData->getResidueFromCharIndex( charIndex );
				}

				Model::Residue & SequenceDisplayWidget::_getResidue( const uint p_localResidueIndex ) const
				{
					const uint moleculeResidueIndex = _chainData->getIndexFirstResidue() + p_localResidueIndex;
					return _chainData->getMoleculePtr()->getResidue( moleculeResidueIndex );
				}
				uint SequenceDisplayWidget::_getCharIndex( const uint p_residueIndex ) const { return _chainData->getCharIndex( p_residueIndex ); }
				uint SequenceDisplayWidget::_getLocalResidueIndexFromResidue( const Model::Residue & p_residue ) const
				{
					return p_residue.getIndex() - _chainData->getIndexFirstResidue();
				}
				QPoint SequenceDisplayWidget::getResiduePos( const Model::Residue & p_residue, const QWidget * const p_widgetSpace ) const
				{
					const uint	localIndex = _getLocalResidueIndexFromResidue( p_residue );
					const uint	charIndex  = _getCharIndex( localIndex );
					const float charSize   = _fontMetrics->averageCharWidth();

					const int posX = (int)( charIndex * charSize + charSize * 0.5f );
					const int posY = height() / 2;

					const QPoint localPos		= QPoint( posX, posY );
					const QPoint globalPos		= mapToGlobal( localPos );
					const QPoint widgetSpacePos = p_widgetSpace->mapFromGlobal( globalPos );

					return widgetSpacePos;
				}

				void SequenceDisplayWidget::paintEvent( QPaintEvent * p_paintEvent )
				{
					QLabel::paintEvent( p_paintEvent );

					QPainter painter( this );
					painter.save();
					painter.setWorldMatrixEnabled( false );
					painter.setBrush( Qt::NoBrush );

					const int charSize = (int)_fontMetrics->averageCharWidth();

					const Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();

					const Model::ID & linkedMoleculeId = _chainData->getMoleculePtr()->getId();
					const uint		  linkedChainIndex = _chainData->getChainIndex();

					for ( const auto pairMoleculeChains : selectionModel.getItems() )
					{
						const Model::ID & moleculeId = pairMoleculeChains.first;

						if ( moleculeId == linkedMoleculeId )
						{
							for ( const auto pairChainResidues : pairMoleculeChains.second )
							{
								const uint chainId = pairChainResidues.first;

								if ( chainId == linkedChainIndex )
								{
									for ( const auto pairResiduesAtoms : pairChainResidues.second )
									{
										const Model::Residue & residue			= _chainData->getMoleculePtr()->getResidue( pairResiduesAtoms.first );
										const uint			   locaResidueIndex = _getLocalResidueIndexFromResidue( residue );

										const int charIndexPaint  = _chainData->getPaintCharIndex( locaResidueIndex );
										const int charLengthPaint = _chainData->getPaintLength( locaResidueIndex );

										const QRect selectionRect = QRect( charIndexPaint * charSize, 0, charLengthPaint * charSize, height() );
										painter.fillRect( selectionRect, Style::SEQUENCE_FOREGROUND_SELECTION_COLOR );
									}
									break;
								}
							}

							break;
						}
					}

					painter.setWorldMatrixEnabled( true );
					painter.restore();
				}
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI

} // namespace VTX
