#include "ui/old_ui/ui/widget/main_menu/molecule/selection_action_block.hpp"
#include "ui/old_ui/ui/dialog.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include <app/action/selection.hpp>
#include <app/action/visible.hpp>
#include <app/mvc.hpp>
#include <app/event/global.hpp>
#include <app/model/selection.hpp>
#include <app/old_app/selection/selection_manager.hpp>

namespace VTX::UI::Widget::MainMenu::Molecule
{
	SelectionActionBlock::SelectionActionBlock( QWidget * p_parent ) : MenuToolBlockWidget( p_parent )
	{
		_registerEvent( VTX::App::Event::Global::SELECTION_CHANGE );
	};

	void SelectionActionBlock::receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == App::Event::Global::SELECTION_CHANGE )
		{
			const VTX::App::Core::Event::VTXEventArg<const Model::Selection *> & castedEvent
				= dynamic_cast<const VTX::App::Core::Event::VTXEventArg<const Model::Selection *> &>( p_event );

			const bool enableSelection = castedEvent.get()->getMoleculeSelectedCount() > 0;
			_enableButtons( enableSelection );

			_copyFrameSubmenu->updateFrames( *castedEvent.get() );
		}
	}

	void SelectionActionBlock::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		_copyFrameSubmenu
			= WidgetFactory::get().instantiateWidget<CustomWidget::TrajectoryFramesMenu>( this, "copyFrameSubmenu" );
		_copyFrameSubmenu->setFrameDisplayString( "Frame #" );
		_copyFrameSubmenu->setDisplayAllFramesOption( true );
		_copyFrameSubmenu->setDisplayAllOptionName( "All frames" );

		// Selection actions 1
		_copy = WidgetFactory::get().instantiateWidget<MenuToolButtonSubmenuWidget>( this, "copySelectionButton" );
		_copy->setSubmenu( _copyFrameSubmenu );
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
	void SelectionActionBlock::_setupSlots()
	{
		_copy->setTriggerAction( this, &SelectionActionBlock::_copySelection );
		_extract->setTriggerAction( this, &SelectionActionBlock::_extractSelection );
		_delete->setTriggerAction( this, &SelectionActionBlock::_deleteSelection );

		_show->setTriggerAction( this, &SelectionActionBlock::_showSelection );
		_hide->setTriggerAction( this, &SelectionActionBlock::_hideSelection );
		_solo->setTriggerAction( this, &SelectionActionBlock::_soloSelection );

		_exportSelectionButton->setTriggerAction( this, &SelectionActionBlock::_exportSelection );

		connect( _copyFrameSubmenu,
				 &CustomWidget::TrajectoryFramesMenu::onFrameSelected,
				 this,
				 &SelectionActionBlock::_copyFrameSelection );
	}
	void SelectionActionBlock::localize() { setTitle( "Molecule Action" ); }

	void SelectionActionBlock::_copySelection() const
	{
		const Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION( new App::Action::Selection::Copy( selectionModel ) );
	}
	void SelectionActionBlock::_copyFrameSelection( const int p_frame ) const
	{
		const Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION( new App::Action::Selection::Copy( selectionModel, p_frame ) );
	}

	void SelectionActionBlock::_extractSelection() const
	{
		Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION( new App::Action::Selection::Extract( selectionModel ) );
	}
	void SelectionActionBlock::_deleteSelection() const
	{
		Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION( new App::Action::Selection::Delete( selectionModel ) );
	}

	void SelectionActionBlock::_showSelection() const
	{
		const Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION(
			new App::Action::Selection::ChangeVisibility( selectionModel, App::Action::VISIBILITY_MODE::SHOW ) );
	}
	void SelectionActionBlock::_hideSelection() const
	{
		const Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION(
			new App::Action::Selection::ChangeVisibility( selectionModel, App::Action::VISIBILITY_MODE::HIDE ) );
	}
	void SelectionActionBlock::_soloSelection() const
	{
		const Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION(
			new App::Action::Selection::ChangeVisibility( selectionModel, App::Action::VISIBILITY_MODE::SOLO ) );
	}

	void SelectionActionBlock::_exportSelection() const { UI::Dialog::openExportMoleculeDialog(); }

	void SelectionActionBlock::_enableButtons( const bool p_enable )
	{
		_copy->setEnabled( p_enable );
		_extract->setEnabled( p_enable );
		_delete->setEnabled( p_enable );
		_show->setEnabled( p_enable );
		_hide->setEnabled( p_enable );
		_solo->setEnabled( p_enable );
		_exportSelectionButton->setEnabled( p_enable );
	}

} // namespace VTX::UI::Widget::MainMenu::Molecule
