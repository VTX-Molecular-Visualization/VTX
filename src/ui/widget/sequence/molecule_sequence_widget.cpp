#include "molecule_sequence_widget.hpp"
#include "model/residue.hpp"
#include "vtx_app.hpp"
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

					_sequenceDisplayWidget = new SequenceDisplayWidget();

					layout->addWidget( _sequenceDisplayWidget );
				}

				void MoleculeSequenceWidget::_setupSlots()
				{
					connect( _sequenceDisplayWidget, &SequenceDisplayWidget::selectionChanged, this, &MoleculeSequenceWidget::_onSequenceSelectionChanged );
				}

				void MoleculeSequenceWidget::_onSequenceSelectionChanged()
				{
					for ( uint residueIndex : _sequenceDisplayWidget->getSelection() )
					{
						const Model::Residue residue = _model->getResidue( residueIndex );
						VTX_INFO( residue.getSymbolName() + " selected." );
					}
				}

				void MoleculeSequenceWidget::localize() {}
				void MoleculeSequenceWidget::refresh() { _sequenceDisplayWidget->setupSequence( *_model ); }
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI

} // namespace VTX
