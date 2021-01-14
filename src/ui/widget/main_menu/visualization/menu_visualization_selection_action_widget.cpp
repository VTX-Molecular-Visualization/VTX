#include "menu_visualization_selection_action_widget.hpp"
#include "action/action_manager.hpp"
#include "action/molecule.hpp"
#include "mvc/mvc_manager.hpp"
#include "selection/selection_manager.hpp"
#include "ui/widget_factory.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				namespace Visualization
				{
					MenuVisualizationSelectionActionWidget::~MenuVisualizationSelectionActionWidget() {}

					void MenuVisualizationSelectionActionWidget::_setupUi( const QString & p_name )
					{
						MenuToolBlockWidget::_setupUi( p_name );

						// Selection actions 1
						_copy = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "copySelectionButton" );
						_copy->setData( "Copy", ":/sprite/copy_selection_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_copy, 0 );

						_extract = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "extractSelectionButton" );
						_extract->setData( "Extract", ":/sprite/extract_selection_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_extract, 0 );

						_delete = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "deleteSelectionButton" );
						_delete->setData( "Delete", ":/sprite/delete_selection_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_delete, 0 );

						// Visibility actions
						_show = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "showSelectionButton" );
						_show->setData( "Show", ":/sprite/show_selection_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_show, 1 );

						_hide = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "hideSelectionButton" );
						_hide->setData( "Hide", ":/sprite/hide_selection_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_hide, 1 );

						validate();
					}
					void MenuVisualizationSelectionActionWidget::_setupSlots()
					{
						_copy->setTriggerAction( this, &MenuVisualizationSelectionActionWidget::_copySelection );
						_extract->setTriggerAction( this, &MenuVisualizationSelectionActionWidget::_extractSelection );
						_delete->setTriggerAction( this, &MenuVisualizationSelectionActionWidget::_deleteSelection );

						_show->setTriggerAction( this, &MenuVisualizationSelectionActionWidget::_showSelection );
						_hide->setTriggerAction( this, &MenuVisualizationSelectionActionWidget::_hideSelection );
					}
					void MenuVisualizationSelectionActionWidget::localize() { setTitle( "Selection Action" ); }

					void MenuVisualizationSelectionActionWidget::_copySelection() const
					{
						const Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
						VTX_ACTION( new Action::Molecule::Copy( selectionModel ) );
					}
					void MenuVisualizationSelectionActionWidget::_extractSelection() const
					{
						const Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
						VTX_ACTION( new Action::Molecule::Extract( selectionModel ) );
					}
					void MenuVisualizationSelectionActionWidget::_deleteSelection() const
					{
						const Model::Selection &			   selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
						const Model::Selection::MapMoleculeIds moleculeIDs	  = selectionModel.getItems();

						for ( const auto pairMolecule : moleculeIDs )
						{
							Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( pairMolecule.first );
							VTX_ACTION( new Action::Molecule::Delete( molecule ) );
						}
					}

					void MenuVisualizationSelectionActionWidget::_showSelection() const {}
					void MenuVisualizationSelectionActionWidget::_hideSelection() const {}
				} // namespace Visualization
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
