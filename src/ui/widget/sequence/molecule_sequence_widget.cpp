#include "molecule_sequence_widget.hpp"
#include "model/residue.hpp"
#include "sequence_display_widget.hpp"
#include "style.hpp"
#include "tool/logger.hpp"
#include "ui/widget_factory.hpp"
#include <QScrollBar>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				MoleculeSequenceWidget::MoleculeSequenceWidget( QWidget * p_parent ) : ViewItemWidget( p_parent ) { _registerEvent( Event::Global::SELECTION_CHANGE ); }

				void MoleculeSequenceWidget::receiveEvent( const Event::VTXEvent & p_event )
				{
					if ( p_event.name == Event::Global::SELECTION_CHANGE ) {}
				}

				void MoleculeSequenceWidget::_setupUi( const QString & p_name )
				{
					BaseManualWidget::_setupUi( p_name );
					setContentsMargins( 0, 0, 0, 0 );

					_sequenceLabel = new QLabel( this );
					_sequenceLabel->setAlignment( Qt::AlignmentFlag::AlignTop );
					_sequenceLabel->setFont( Style::SEQUENCE_DISPLAY_FONT );

					QWidget * const scrollAreaContent = new QWidget();
					scrollAreaContent->setContentsMargins( 0, 0, 0, 0 );
					_sequenceLayout = new QHBoxLayout( scrollAreaContent );
					_sequenceLayout->setSizeConstraint( QLayout::SetMinAndMaxSize );
					_sequenceLayout->setContentsMargins( 0, 0, 0, 0 );
					_sequenceLayout->setSpacing( 0 );
					_sequenceLayout->addStretch( 1000 );

					_scrollArea = new QScrollArea( this );
					_scrollArea->setContentsMargins( 0, 0, 0, 0 );
					_scrollArea->setWidget( scrollAreaContent );
					_scrollArea->setFrameShape( QFrame::Shape::NoFrame );

					QHBoxLayout * const layout = new QHBoxLayout( this );
					layout->setContentsMargins( 0, 0, 0, 0 );

					layout->addWidget( _sequenceLabel );
					layout->addWidget( _scrollArea );
				}

				void MoleculeSequenceWidget::_setupSlots()
				{
					const QScrollBar * const scrollBar = _scrollArea->horizontalScrollBar();
					connect( scrollBar, &QScrollBar::valueChanged, this, &MoleculeSequenceWidget::_onScrollBarValueChanged );
				}

				void MoleculeSequenceWidget::_onScrollBarValueChanged()
				{
					const QScrollBar * const scrollBar = _scrollArea->horizontalScrollBar();
					const int				 value	   = scrollBar->value();

					// Check first visible
					for ( auto it = _chainDisplayWidgets.rbegin(); it != _chainDisplayWidgets.rend(); it++ )
					{
						const QPoint pos = ( *it )->pos();

						if ( pos.x() < value )
						{
							_updateLabelName( *( ( *it )->getModel() ) );
							break;
						}
					}
				}

				void MoleculeSequenceWidget::_updateLabelName( const Model::Chain & p_currentChainDisplayed )
				{
					_sequenceLabel->setText( QString::fromStdString( _model->getPdbIdCode() + Style::SEQUENCE_CHAIN_NAME_SEPARATOR + p_currentChainDisplayed.getName()
																	 + Style::SEQUENCE_CHAIN_NAME_SEPARATOR ) );
				}

				void MoleculeSequenceWidget::localize() {}
				void MoleculeSequenceWidget::refresh()
				{
					const int chainCount = _model->getChainCount();
					if ( chainCount <= 0 )
						_sequenceLabel->setText( QString::fromStdString( _model->getPdbIdCode() ) );
					else
						_updateLabelName( _model->getChain( 0 ) );

					_chainDisplayWidgets = std::vector<ChainSequenceWidget *>();
					_chainDisplayWidgets.reserve( chainCount );

					for ( Model::Chain * const chain : _model->getChains() )
					{
						QLabel * const chainNameWidget = new QLabel( this );
						chainNameWidget->setAlignment( Qt::AlignmentFlag::AlignTop );
						chainNameWidget->setFont( Style::SEQUENCE_DISPLAY_FONT );
						chainNameWidget->setText( QString::fromStdString( Style::SEQUENCE_CHAIN_NAME_SEPARATOR + chain->getName() + Style::SEQUENCE_CHAIN_NAME_SEPARATOR ) );

						ChainSequenceWidget * const chainSequenceDisplay = WidgetFactory::get().GetWidget<ChainSequenceWidget>( this, "chainSequenceWidget" );
						_chainDisplayWidgets.emplace_back( chainSequenceDisplay );

						chainSequenceDisplay->setModel( chain );

						_sequenceLayout->insertWidget( _sequenceLayout->count() - 1, chainNameWidget );
						_sequenceLayout->insertWidget( _sequenceLayout->count() - 1, chainSequenceDisplay );
					}
				}
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI

} // namespace VTX
