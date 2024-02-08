#include "ui/old_ui/ui/widget/sequence/sequence_display_widget.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/util/ui.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include "ui/qt/action/residue.hpp"
#include <QPainter>
#include <QRectF>

#include <app/old/action/residue.hpp>
#include <app/old/component/chemistry/chain.hpp>
#include <app/old/component/chemistry/molecule.hpp>
#include <app/old/application/selection/selection.hpp>
#include <app/old/application/selection/selection_manager.hpp>

namespace VTX::UI::Widget::Sequence
{
	SequenceDisplayWidget::SequenceDisplayWidget( QWidget * p_parent ) : QLabel( p_parent )
	{
		setTextInteractionFlags( Qt::TextInteractionFlag::NoTextInteraction );
		setFont( Style::SEQUENCE_DISPLAY_FONT() );

		_fontMetrics		 = new QFontMetricsF( font() );
		_charSizeFromMetrics = double( _fontMetrics->horizontalAdvance( 'A' ) );
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
		const App::Old::Component::Chemistry::Residue * const residue = _getResidueFromLocaleXPos( p_event->localPos().x() );
		if ( residue != nullptr )
		{
			VTX_ACTION( new QT::Action::Residue::Orient( *residue ) );
			p_event->accept();
		}
	}

	App::Old::Component::Chemistry::Residue * const SequenceDisplayWidget::getResidueAtPos( const QPoint & p_pos )
	{
		return _getResidueFromLocaleXPos( p_pos.x() );
	}

	App::Old::Component::Chemistry::Residue * const SequenceDisplayWidget::getClosestResidueFromPos( const QPoint & p_pos,
																			const bool	   p_takeforward )
	{
		const uint charIndex = p_pos.x() / _charSizeFromMetrics;
		return _chainData->getClosestResidueFromCharIndex( charIndex, p_takeforward );
	}

	App::Old::Component::Chemistry::Residue * const SequenceDisplayWidget::_getResidueFromLocaleXPos( const int p_localeXPos ) const
	{
		const uint charIndex = p_localeXPos / _charSizeFromMetrics;
		return _chainData->getResidueFromCharIndex( charIndex );
	}

	App::Old::Component::Chemistry::Residue & SequenceDisplayWidget::_getResidue( const uint p_localResidueIndex ) const
	{
		const uint moleculeResidueIndex = _chainData->getIndexFirstResidue() + p_localResidueIndex;
		return *_chainData->getMoleculePtr()->getResidue( moleculeResidueIndex );
	}
	uint SequenceDisplayWidget::_getCharIndex( const uint p_residueIndex ) const
	{
		return _chainData->getCharIndex( p_residueIndex );
	}
	uint SequenceDisplayWidget::_getLocalResidueIndexFromResidue( const App::Old::Component::Chemistry::Residue & p_residue ) const
	{
		return p_residue.getIndex() - _chainData->getIndexFirstResidue();
	}
	QPoint SequenceDisplayWidget::getResiduePos( const App::Old::Component::Chemistry::Residue & p_residue,
												 const QWidget * const	p_widgetSpace ) const
	{
		const uint localIndex = _getLocalResidueIndexFromResidue( p_residue );
		const uint charIndex  = _getCharIndex( localIndex );

		const int posX = (int)( charIndex * _charSizeFromMetrics + _charSizeFromMetrics * 0.5f );
		const int posY = height() / 2;

		const QPoint localPos		= QPoint( posX, posY );
		const QPoint globalPos		= mapToGlobal( localPos );
		const QPoint widgetSpacePos = p_widgetSpace->mapFromGlobal( globalPos );

		return widgetSpacePos;
	}

	void SequenceDisplayWidget::paintEvent( QPaintEvent * p_paintEvent )
	{
		QLabel::paintEvent( p_paintEvent );

		App::Old::Application::Selection::SelectionModel & selectionModel = VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel();
		if ( selectionModel.isChainSelected( _chainData->getChain() ) )
		{
			// linked chain is in selection => draw selection feedback on selected residues
			QPainter painter( this );
			painter.save();
			painter.setWorldMatrixEnabled( false );
			painter.setBrush( Qt::NoBrush );

			// Use _fontMetrics->horizontalAdvance( 'A' ) instead of _fontMetrics->averageCharWidth() because even on a
			// Monospace font, some characters may change the averageCharWidth value, even if all displayed chars have
			// the same width
			int lastPixelDrawn	  = -1;
			int currentFirstPixel = -1;
			int currentRectWidth  = 0;

			const App::Old::Component::Chemistry::Molecule & molecule = *_chainData->getMoleculePtr();

			const App::Old::Application::Selection::SelectionModel::MapResidueIds & mapSelectedResidueID
				= selectionModel.getMoleculesMap()[ molecule.getId() ][ _chainData->getChainIndex() ];

			for ( const App::Old::Application::Selection::SelectionModel::PairResidueIds & pairResiduesAtoms : mapSelectedResidueID )
			{
				const App::Old::Component::Chemistry::Residue * const residue		  = molecule.getResidue( pairResiduesAtoms.first );
				const uint					 locaResidueIndex = _getLocalResidueIndexFromResidue( *residue );

				const int charIndexPaint  = _chainData->getPaintCharIndex( locaResidueIndex );
				const int charLengthPaint = _chainData->getPaintLength( locaResidueIndex );

				// Trick to prevent double painting on same pixel when charsize is not int
				int firstPixel = int( floor( charIndexPaint * _charSizeFromMetrics ) );
				int rectWidth  = int( ceil( charLengthPaint * _charSizeFromMetrics ) );

				if ( lastPixelDrawn > firstPixel )
				{
					firstPixel += 1;
					rectWidth -= 1;
				}

				if ( currentFirstPixel >= 0 )
				{
					if ( firstPixel == lastPixelDrawn )
					{
						currentRectWidth += rectWidth;
					}
					else
					{
						const QRect selectionRect = QRect( currentFirstPixel, 0, currentRectWidth, height() );
						painter.fillRect( selectionRect, Style::SEQUENCE_FOREGROUND_SELECTION_COLOR );

						currentFirstPixel = firstPixel;
						currentRectWidth  = rectWidth;
					}
				}
				else
				{
					currentFirstPixel = firstPixel;
					currentRectWidth  = rectWidth;
				}

				lastPixelDrawn = firstPixel + rectWidth;
			}

			if ( currentRectWidth > 0 )
			{
				const QRect selectionRect = QRect( currentFirstPixel, 0, currentRectWidth, height() );
				painter.fillRect( selectionRect, Style::SEQUENCE_FOREGROUND_SELECTION_COLOR );
			}

			painter.setWorldMatrixEnabled( true );
			painter.restore();
		}
	}
} // namespace VTX::UI::Widget::Sequence