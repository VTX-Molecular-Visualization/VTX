#include "inspector_widget.hpp"
#include "model/molecule.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
#include "model/selection.hpp"
#include "representation/representation_manager.hpp"
#include "selection/selection_manager.hpp"
#include "view/ui/widget/molecule_inspector_view.hpp"
#include "view/ui/widget/representation_inspector_view.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Inspector
			{
				InspectorWidget::InspectorWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
				{
					_registerEvent( Event::Global::SELECTION_CHANGE );
					_registerEvent( Event::Global::REPRESENTATION_ADDED );
					_registerEvent( Event::Global::REPRESENTATION_REMOVED );
				}

				InspectorWidget::~InspectorWidget() {}

				void InspectorWidget::receiveEvent( const Event::VTXEvent & p_event )
				{
					if ( p_event.name == Event::Global::SELECTION_CHANGE )
					{
						refresh();
					}
					else if ( p_event.name == Event::Global::REPRESENTATION_ADDED )
					{
						const Event::VTXEventPtr<Model::Representation::BaseRepresentation> & castedEvent
							= dynamic_cast<const Event::VTXEventPtr<Model::Representation::BaseRepresentation> &>( p_event );

						Model::Representation::BaseRepresentation * const	  representation = castedEvent.ptr;
						View::UI::Widget::RepresentationInspectorView * const representationView
							= MVC::MvcManager::get().instanciateViewWidget<View::UI::Widget::RepresentationInspectorView>(
								representation, ID::View::UI_INSPECTOR_REPRESENTATION, this );

						QWidget * const widget = representationView->getWidget();

						_verticalLayout->insertWidget( _verticalLayout->count() - 1, widget );
					}
					else if ( p_event.name == Event::Global::REPRESENTATION_REMOVED )
					{
						const Event::VTXEventPtr<Model::Representation::BaseRepresentation> & castedEvent
							= dynamic_cast<const Event::VTXEventPtr<Model::Representation::BaseRepresentation> &>( p_event );

						const Model::Representation::BaseRepresentation * const representation = castedEvent.ptr;
						View::UI::Widget::RepresentationInspectorView * const	representationView
							= MVC::MvcManager::get().getView<View::UI::Widget::RepresentationInspectorView>( representation, ID::View::UI_INSPECTOR_REPRESENTATION );

						_verticalLayout->removeWidget( representationView->getWidget() );

						MVC::MvcManager::get().deleteView<View::UI::Widget::MoleculeInspectorView>( representation, ID::View::UI_INSPECTOR_MOLECULE_STRUCTURE );
					}
				}

				void InspectorWidget::refresh()
				{
					clear();

					// TODO
				}

				void InspectorWidget::clear()
				{
					QObjectList widgets = _verticalLayout->children();

					for ( auto it = widgets.begin(); it != widgets.end(); it++ )
					{
						_verticalLayout->removeItem( dynamic_cast<QWidgetItem *>( *it ) );
						delete *it;
					}
				}

				void InspectorWidget::_setupUi( const QString & p_name )
				{
					BaseManualWidget::_setupUi( p_name );

					QWidget * layoutWidget = new QWidget( this );
					layoutWidget->setContentsMargins( 0, 0, 0, 0 );

					_verticalLayout = new QVBoxLayout( layoutWidget );
					_verticalLayout->setSpacing( 4 );
					_verticalLayout->setContentsMargins( 0, 0, 0, 0 );
					_verticalLayout->addStretch( 1000 );

					this->setWidget( layoutWidget );
				}

				void InspectorWidget::_setupSlots() {}
				void InspectorWidget::localize()
				{
					this->setWindowTitle( "Inspector" );
					// this->setWindowTitle( QCoreApplication::translate( "InspectorWidget", "Inspector", nullptr ) );
				}
			} // namespace Inspector
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
