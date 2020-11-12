#include "sequence_display_widget.hpp"
#include "model/molecule.hpp"
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

				void SequenceDisplayWidget::setupSequence( const Model::Chain & p_chain )
				{
					_chain = &p_chain;

					QString sequenceTxt	   = QString();
					uint	plainTextIndex = 0;

					const uint residueCount = _chain->getResidueCount();

					Util::UI::appendColorHtmlTag( sequenceTxt, _chain->getColor() );

					bool previousResidueWasUnknown = false;

					for ( uint localResidueIndex = 0; localResidueIndex < residueCount; ++localResidueIndex )
					{
						const Model::Residue & residue = _getResidue( localResidueIndex );

						QString	   symbol;
						const bool unknownResidue = residue.getSymbolShort() == "?";

						if ( unknownResidue )
						{
							symbol = QString();

							if ( localResidueIndex > 0 && !previousResidueWasUnknown )
								symbol.append( ' ' );

							symbol.append( QString::fromStdString( residue.getSymbolStr() ) );

							if ( localResidueIndex < residueCount - 1 )
								symbol.append( ' ' );

							_positionUnknownResidues.emplace_back( UnknownResidueData( localResidueIndex, plainTextIndex, symbol ) );
						}
						else
						{
							symbol = QString::fromStdString( residue.getSymbolShort() );
						}

						sequenceTxt.append( symbol );

						plainTextIndex += symbol.length();
						previousResidueWasUnknown = unknownResidue;
					}
					Util::UI::appendEndColorHtmlTag( sequenceTxt );

					setText( sequenceTxt );
				}

				void SequenceDisplayWidget::mouseDoubleClickEvent( QMouseEvent * p_event )
				{
					const uint			   residueIndex = _getResidueIndexFromLocaleXPos( p_event->localPos().x() );
					const Model::Residue & residue		= _getResidue( residueIndex );

					p_event->accept();

					VTX_INFO( "Double click on " + residue.getSymbolName() );
				}

				Model::Residue & SequenceDisplayWidget::getResidueAtPos( const QPoint & p_pos )
				{
					const uint		 localResidueIndex = _getResidueIndexFromLocaleXPos( p_pos.x() );
					Model::Residue & residue		   = _getResidue( localResidueIndex );

					return residue;
				}

				uint SequenceDisplayWidget::_getResidueIndexFromLocaleXPos( const int p_localeXPos ) const
				{
					const float charSize  = _fontMetrics->averageCharWidth();
					const uint	charIndex = p_localeXPos / charSize;

					const uint residueIndex = _getLocalResidueIndex( charIndex );

					return residueIndex >= _chain->getResidueCount() ? _chain->getResidueCount() - 1 : residueIndex;
				}
				uint SequenceDisplayWidget::_getLocalResidueIndex( const uint p_charIndex ) const
				{
					uint localResidueIndex = p_charIndex;

					for ( int i = 0; i < _positionUnknownResidues.size(); i++ )
					{
						const UnknownResidueData & unknownResidueData = _positionUnknownResidues[ i ];
						if ( unknownResidueData.charIndex < p_charIndex )
						{
							if ( p_charIndex >= unknownResidueData.charIndex + unknownResidueData.strSize )
								localResidueIndex -= unknownResidueData.strSize - 1;
							else
								localResidueIndex = unknownResidueData.residueIndex;
						}
						else
						{
							break;
						}
					}

					return localResidueIndex;
				}
				Model::Residue & SequenceDisplayWidget::_getResidue( const uint p_localResidueIndex ) const
				{
					const uint moleculeResidueIndex = _chain->getIndexFirstResidue() + p_localResidueIndex;
					return _chain->getMoleculePtr()->getResidue( moleculeResidueIndex );
				}
				uint SequenceDisplayWidget::_getCharIndex( const uint p_residueIndex ) const
				{
					uint res = p_residueIndex;

					for ( int i = 0; i < _positionUnknownResidues.size(); i++ )
					{
						if ( _positionUnknownResidues[ i ].residueIndex < p_residueIndex )
							res += _positionUnknownResidues[ i ].strSize - 1;
						else
							break;
					}

					return res;
				}
				uint SequenceDisplayWidget::_getLocalResidueIndexFromResidue( const Model::Residue & p_residue ) const
				{
					return p_residue.getIndex() - _chain->getIndexFirstResidue();
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

				bool SequenceDisplayWidget::_checkUnknownResidue( const uint p_localResidueIndex, const UnknownResidueData *& p_unknownResidueData ) const
				{
					bool unknownResidueDataFound = false;

					for ( int i = 0; i < _positionUnknownResidues.size(); i++ )
					{
						const UnknownResidueData data = _positionUnknownResidues[ i ];
						if ( data.residueIndex == p_localResidueIndex )
						{
							p_unknownResidueData	= &data;
							unknownResidueDataFound = true;
							break;
						}
						else if ( data.residueIndex > p_localResidueIndex )
						{
							break;
						}
					}

					return unknownResidueDataFound;
				}

				void SequenceDisplayWidget::paintEvent( QPaintEvent * p_paintEvent )
				{
					QLabel::paintEvent( p_paintEvent );

					QPainter painter( this );
					painter.save();
					painter.setWorldMatrixEnabled( false );
					painter.setBrush( Qt::NoBrush );

					if ( _moleculeSelection != nullptr )
					{
						const int charSize = (int)_fontMetrics->averageCharWidth();

						for ( auto it : *_moleculeSelection )
						{
							if ( it == nullptr || it->getChainPtr()->getId() != _chain->getId() )
								continue;

							uint locaResidueIndex = _getLocalResidueIndexFromResidue( *it );
							_getResidueHighlightData( locaResidueIndex, _charIndexPaintCache, _symbolLengthPaintCache );

							QRect selectionRect = QRect( *_charIndexPaintCache * charSize, 0, *_symbolLengthPaintCache * charSize, height() );
							painter.fillRect( selectionRect, Style::SEQUENCE_FOREGROUND_SELECTION_COLOR );
						}
					}
					painter.setWorldMatrixEnabled( true );
					painter.restore();
				}
				void SequenceDisplayWidget::_getResidueHighlightData( uint p_localResidueIndex, int * const p_charIndex, int * const p_length ) const
				{
					const UnknownResidueData * unkownResidueData = nullptr;

					if ( _checkUnknownResidue( p_localResidueIndex, unkownResidueData ) )
					{
						unkownResidueData->getSelectionData( p_charIndex, p_length );
					}
					else
					{
						*p_charIndex = _getCharIndex( p_localResidueIndex );
						*p_length	 = 1;
					}
				}
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI

} // namespace VTX
