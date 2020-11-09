#include "inspector_widget.hpp"
#include "model/molecule.hpp"
#include "selection/selection_manager.hpp"
#include "view/ui/widget/molecule_inspector_view.hpp"

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
					_registerEvent( Event::Global::MOLECULE_ADDED );
				}

				InspectorWidget::~InspectorWidget() {}

				void InspectorWidget::receiveEvent( const Event::VTXEvent & p_event )
				{
					if ( p_event.name == Event::Global::SELECTION_CHANGE )
						refresh();

					if ( p_event.name == Event::Global::MOLECULE_ADDED )
					{
						const Event::VTXEventPtr<Model::Molecule> &		castedEvent			  = dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
						View::UI::Widget::MoleculeInspectorView * const moleculeInspectorView = new View::UI::Widget::MoleculeInspectorView( castedEvent.ptr, nullptr );
						MVC::MvcManager::get().addViewOnModel( castedEvent.ptr, ID::View::UI_INSPECTOR_MOLECULE_STRUCTURE, moleculeInspectorView );

						QWidget * const widget = moleculeInspectorView->getLinkedWidget();

						_verticalLayout->insertWidget( 0, widget );
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
						_verticalLayout->removeItem( dynamic_cast<QWidgetItem *>( *it ) );
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
