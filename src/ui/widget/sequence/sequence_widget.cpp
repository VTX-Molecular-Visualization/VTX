#include "sequence_widget.hpp"
#include "model/molecule.hpp"
#include "mvc/mvc_manager.hpp"
#include "view/base_view.hpp"
#include "view/ui/widget/molecule_sequence_view.hpp"
#include "view/ui/widget/selection_sequence_view.hpp"
#include "vtx_app.hpp"
#include <QLayout>
#include <QScrollBar>
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				SequenceWidget::SequenceWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
				{
					_registerEvent( Event::Global::MOLECULE_ADDED );
					_registerEvent( Event::Global::MOLECULE_REMOVED );
					_registerEvent( Event::Global::SELECTION_ADDED );
					_registerEvent( Event::Global::SELECTION_CHANGE );
					_registerEvent( Event::Global::SELECTION_REMOVED );
				}

				void SequenceWidget::receiveEvent( const Event::VTXEvent & p_event )
				{
					if ( p_event.name == Event::Global::MOLECULE_ADDED )
					{
						const Event::VTXEventPtr<Model::Molecule> &	   castedEvent			= dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
						View::UI::Widget::MoleculeSequenceView * const moleculeSequenceView = new View::UI::Widget::MoleculeSequenceView( castedEvent.ptr, this );

						MVC::MvcManager::get().addViewOnModel( castedEvent.ptr, ID::View::UI_MOLECULE_SEQUENCE, moleculeSequenceView );
						MoleculeSequenceWidget * widget = moleculeSequenceView->getWidget();

						_moleculeWidgets.emplace( widget );

						_layout->insertWidget( _layout->count() - 1, widget );
					}
					else if ( p_event.name == Event::Global::MOLECULE_REMOVED )
					{
						const Event::VTXEventPtr<Model::Molecule> &	   castedEvent = dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
						View::UI::Widget::MoleculeSequenceView * const moleculeSequenceView
							= MVC::MvcManager::get().removeViewOnModel<Model::Molecule, View::UI::Widget::MoleculeSequenceView>( castedEvent.ptr, ID::View::UI_MOLECULE_SEQUENCE );

						_moleculeWidgets.erase( moleculeSequenceView->getWidget() );
						delete moleculeSequenceView;
					}
					else if ( p_event.name == Event::Global::SELECTION_ADDED )
					{
						const Event::VTXEventPtr<Model::Selection> &	castedEvent			  = dynamic_cast<const Event::VTXEventPtr<Model::Selection> &>( p_event );
						View::UI::Widget::SelectionSequenceView * const selectionSequenceView = new View::UI::Widget::SelectionSequenceView( castedEvent.ptr, this );

						MVC::MvcManager::get().addViewOnModel( castedEvent.ptr, ID::View::UI_MOLECULE_SEQUENCE, selectionSequenceView );

						refreshSelection();
					}
					else if ( p_event.name == Event::Global::SELECTION_REMOVED )
					{
						const Event::VTXEventPtr<Model::Selection> & castedEvent = dynamic_cast<const Event::VTXEventPtr<Model::Selection> &>( p_event );
						View::UI::Widget::SelectionSequenceView *	 selectionSequenceView
							= MVC::MvcManager::get().removeViewOnModel<Model::Selection, View::UI::Widget::SelectionSequenceView>( castedEvent.ptr,
																																   ID::View::UI_SELECTION_SEQUENCE );

						delete selectionSequenceView;

						refreshSelection();
					}
					else if ( p_event.name == Event::Global::SELECTION_CHANGE )
					{
						refreshSelection();
					}
				}

				void SequenceWidget::_setupUi( const QString & p_name )
				{
					BaseManualWidget::_setupUi( p_name );

					QScrollArea * const scrollArea = new QScrollArea( this );
					scrollArea->setContentsMargins( 0, 0, 0, 0 );

					QWidget * const scrollWidget = new QWidget( scrollArea );
					scrollWidget->setContentsMargins( 0, 0, 0, 0 );

					_layout = new QVBoxLayout( scrollWidget );
					_layout->setContentsMargins( 0, 0, 0, 0 );
					_layout->setSizeConstraint( QLayout::SizeConstraint::SetMinAndMaxSize );
					_layout->addStretch( 1000 );

					scrollArea->setWidget( scrollWidget );
					scrollArea->setWidgetResizable( true );
					this->setWidget( scrollArea );
				}

				void SequenceWidget::_setupSlots() {}

				void SequenceWidget::refreshSelection() const
				{
					for ( const auto it : _moleculeWidgets )
					{
						it->repaintSelection();
					}
				}

				void SequenceWidget::localize()
				{
					this->setWindowTitle( "Sequence" );
					// this->setWindowTitle( QCoreApplication::translate( "SceneWidget", "Scene", nullptr ) );
				}
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI

} // namespace VTX
