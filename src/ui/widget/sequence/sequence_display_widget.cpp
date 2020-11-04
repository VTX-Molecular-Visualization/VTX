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
					setTextInteractionFlags( Qt::TextInteractionFlag::TextSelectableByMouse );
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

				void SequenceDisplayWidget::mousePressEvent( QMouseEvent * ev ) { _startPressPosition = ev->localPos(); }
				void SequenceDisplayWidget::mouseReleaseEvent( QMouseEvent * ev )
				{
					bool selectionHasChange = false;

					const Qt::KeyboardModifiers modifiers = ev->modifiers();

					if ( !( modifiers & ( Qt::KeyboardModifier::ShiftModifier | Qt::KeyboardModifier::ControlModifier ) ) )
					{
						if ( !_selection.empty() )
						{
							selectionHasChange = true;
							_selection.clear();
						}
					}

					const uint startResidue = _getResidueIndexFromMousePos( _startPressPosition );
					const uint endResidue	= _getResidueIndexFromMousePos( ev->localPos() );

					const uint minResidueIndex = startResidue < endResidue ? startResidue : endResidue;
					const uint maxResidueIndex = startResidue > endResidue ? startResidue : endResidue;

					for ( uint localResidueIndex = minResidueIndex; localResidueIndex <= maxResidueIndex; localResidueIndex++ )
					{
						const Model::Residue & residueSelected = _getResidue( localResidueIndex );
						VTX::Selection::SelectionManager::get().getSelectionModel().selectResidue( residueSelected );

						_selection.emplace( localResidueIndex );
						selectionHasChange = true;
					}

					if ( selectionHasChange )
					{
						emit selectionChanged();
						repaint();
					}
				};

				void SequenceDisplayWidget::mouseDoubleClickEvent( QMouseEvent * ev )
				{
					const uint			   residueIndex = _getResidueIndexFromMousePos( ev->localPos() );
					const Model::Residue & residue		= _getResidue( residueIndex );

					VTX_INFO( "Double click on " + residue.getSymbolName() );
				}

				uint SequenceDisplayWidget::_getResidueIndexFromMousePos( const QPointF & p_position ) const
				{
					const float charSize  = _fontMetrics->averageCharWidth();
					const uint	charIndex = p_position.x() / charSize;

					return _getLocalResidueIndex( charIndex );
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

				bool SequenceDisplayWidget::_checkUnknownResidue( const uint p_localResidueIndex, const UnknownResidueData *& p_unknownResidueData ) const
				{
					bool unknownResidueDataFound = false;

					for ( int i = 0; i < _positionUnknownResidues.size(); i++ )
					{
						UnknownResidueData data = _positionUnknownResidues[ i ];
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

					const int charSize = (int)_fontMetrics->averageCharWidth();

					for ( auto it : _selection )
					{
						_getUnderlineData( it, _charIndexPaintCache, _symbolLengthPaintCache );

						QRect selectionRect = QRect( *_charIndexPaintCache * charSize, 0, *_symbolLengthPaintCache * charSize, height() );
						painter.fillRect( selectionRect, Style::SEQUENCE_FOREGROUND_SELECTION_COLOR );
					}

					painter.setWorldMatrixEnabled( true );
					painter.restore();
				}

				void SequenceDisplayWidget::_getUnderlineData( uint p_localResidueIndex, int * const p_charIndex, int * const p_length ) const
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
