#include "molecule_sequence_widget.hpp"
#include "model/chain.hpp"
#include <QHBoxLayout>

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

					QHBoxLayout * const layout = new QHBoxLayout( this );
					layout->setSizeConstraint( QLayout::SetMinAndMaxSize );
					layout->setContentsMargins( 0, 0, 0, 0 );

					_sequenceLabel = new QLabel();

					layout->addWidget( _sequenceLabel );
				}

				void MoleculeSequenceWidget::_setupSlots() {}

				void MoleculeSequenceWidget::localize() {}
				void MoleculeSequenceWidget::refresh()
				{
					QString sequenceTxt = QString::fromStdString( _model->getName() ) + '\n';
					sequenceTxt.append( '/' );
					sequenceTxt.append( QString::fromStdString( _model->getPdbIdCode() ) );

					for ( const Model::Chain * const chain : _model->getChains() )
					{
						sequenceTxt.append( '\n' );
						sequenceTxt.append( QString::fromStdString( chain->getSequence() ) );
					}

					_sequenceLabel->setText( sequenceTxt );
				}
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI

} // namespace VTX
