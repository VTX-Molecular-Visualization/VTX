#include "inspector_widget.hpp"
#include "selection/selection_manager.hpp"
#include "ui/widget/inspector_molecule_widget.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			InspectorWidget::InspectorWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
			{
				_registerEvent( Event::Global::SELECTION_CHANGE );
				_registerEvent( Event::Global::MOLECULE_ADDED );
			}

			void InspectorWidget::receiveEvent( const Event::VTXEvent & p_event )
			{
				if ( p_event.name == Event::Global::SELECTION_CHANGE )
					refresh();

				if ( p_event.name == Event::Global::MOLECULE_ADDED )
				{
					UI::Widget::InspectorMoleculeWidget * moleculeInspector = UI::WidgetFactory::get().GetWidget<UI::Widget::InspectorMoleculeWidget>( this, "MoleculeInspector" );
					const Event::VTXEventPtr<Model::Molecule> & castedEvent = dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
					moleculeInspector->setModel( castedEvent.ptr );

					_verticalLayout->addWidget( moleculeInspector );
				}
			}

			void InspectorWidget::refresh()
			{
				clear();

				Selection::SelectionManager					  selectionManager = VTXApp::get().getSelectionManager();
				Selection::SelectionManager::SetSelectablePtr selection		   = selectionManager.getSelection();

				if ( selection.size() > 0 )
				{
					Selection::BaseSelectable * firstItem = *selection.begin();
					Model::BaseModel *			model	  = static_cast<Model::BaseModel *>( firstItem );

					// View::BaseView * inspectorItemView = model->instantiateInspectorView();
					// MVC::MvcManager::get().addViewOnModel( model, ID::UI::INSPECTOR_ITEM, inspectorItemView );

					// BaseManualWidget * inspectorItemWidget = dynamic_cast<BaseManualWidget *>( inspectorItemView );
					//_verticalLayout->insertWidget( 0, inspectorItemWidget );
				}
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

				this->setWidget( layoutWidget );
			}

			void InspectorWidget::_setupSlots() {}
			void InspectorWidget::localize()
			{
				this->setWindowTitle( "Inspector" );
				// this->setWindowTitle( QCoreApplication::translate( "InspectorWidget", "Inspector", nullptr ) );
			}
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
