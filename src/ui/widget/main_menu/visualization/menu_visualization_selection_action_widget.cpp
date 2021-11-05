#include "menu_visualization_selection_action_widget.hpp"
#include "action/action_manager.hpp"
#include "action/selection.hpp"
#include "action/visible.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "selection/selection_manager.hpp"
#include "ui/dialog.hpp"
#include "ui/widget_factory.hpp"

namespace VTX::UI::Widget::MainMenu::Visualization
{
	MenuVisualizationSelectionActionWidget::MenuVisualizationSelectionActionWidget( QWidget * p_parent ) :
		MenuToolBlockWidget( p_parent )
	{
		_registerEvent( Event::Global::SELECTION_CHANGE );
	};

	void MenuVisualizationSelectionActionWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::SELECTION_CHANGE )
		{
			const Event::VTXEventPtr<Model::Selection> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Selection> &>( p_event );

			const bool enableSelection = castedEvent.ptr->getMoleculeSelectedCount() > 0;
			_enableButtons( enableSelection );
		}
	}

	void MenuVisualizationSelectionActionWidget::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		// Selection actions 1
		_copy = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "copySelectionButton" );
		_copy->setData( "Duplicate", ":/sprite/copy_selection_icon.png", Qt::Orientation::Horizontal );
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

		_solo = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "soloSelectionButton" );
		_solo->setData( "Solo", ":/sprite/solo_selection_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_solo, 1 );

		_exportSelectionButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "saveMoleculeButton" );
		_exportSelectionButton->setData( "Export", ":/sprite/save_molecule_icon.png", Qt::Orientation::Vertical );
		pushButton( *_exportSelectionButton, 2 );

		_enableButtons( false );

		validate();
	}
	void MenuVisualizationSelectionActionWidget::_setupSlots()
	{
		_copy->setTriggerAction( this, &MenuVisualizationSelectionActionWidget::_copySelection );
		_extract->setTriggerAction( this, &MenuVisualizationSelectionActionWidget::_extractSelection );
		_delete->setTriggerAction( this, &MenuVisualizationSelectionActionWidget::_deleteSelection );

		_show->setTriggerAction( this, &MenuVisualizationSelectionActionWidget::_showSelection );
		_hide->setTriggerAction( this, &MenuVisualizationSelectionActionWidget::_hideSelection );
		_solo->setTriggerAction( this, &MenuVisualizationSelectionActionWidget::_soloSelection );

		_exportSelectionButton->setTriggerAction( this, &MenuVisualizationSelectionActionWidget::_exportSelection );
	}
	void MenuVisualizationSelectionActionWidget::localize() { setTitle( "Molecule Action" ); }

	void MenuVisualizationSelectionActionWidget::_copySelection() const
	{
		const Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION( new Action::Selection::Copy( selectionModel ) );
	}
	void MenuVisualizationSelectionActionWidget::_extractSelection() const
	{
		const Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION( new Action::Selection::Extract( selectionModel ) );
	}
	void MenuVisualizationSelectionActionWidget::_deleteSelection() const
	{
		Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION( new Action::Selection::Delete( selectionModel ) );
	}

	void MenuVisualizationSelectionActionWidget::_showSelection() const
	{
		const Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION( new Action::Selection::ChangeVisibility(
			selectionModel, Action::Visible::ChangeVisibility::VISIBILITY_MODE::SHOW ) );
	}
	void MenuVisualizationSelectionActionWidget::_hideSelection() const
	{
		const Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION( new Action::Selection::ChangeVisibility(
			selectionModel, Action::Visible::ChangeVisibility::VISIBILITY_MODE::HIDE ) );
	}
	void MenuVisualizationSelectionActionWidget::_soloSelection() const
	{
		const Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION( new Action::Selection::ChangeVisibility(
			selectionModel, Action::Visible::ChangeVisibility::VISIBILITY_MODE::SOLO ) );
	}

	void MenuVisualizationSelectionActionWidget::_exportSelection() const { UI::Dialog::openExportMoleculeDialog(); }

	void MenuVisualizationSelectionActionWidget::_enableButtons( const bool p_enable )
	{
		_copy->setEnabled( p_enable );
		_extract->setEnabled( p_enable );
		_delete->setEnabled( p_enable );
		_show->setEnabled( p_enable );
		_hide->setEnabled( p_enable );
		_solo->setEnabled( p_enable );
		_exportSelectionButton->setEnabled( p_enable );
	}

} // namespace VTX::UI::Widget::MainMenu::Visualization
