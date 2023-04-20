#include "ui/old_ui/ui/widget/main_menu/home/menu_home_molecule_widget.hpp"
#include "ui/old_ui/ui/dialog.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include <app/core/action/action_manager.hpp>
#include <app/action/selection.hpp>
#include <app/model/selection.hpp>
#include <app/core/mvc/mvc_manager.hpp>
#include <app/old_app/selection/selection_manager.hpp>

namespace VTX::UI::Widget::MainMenu::Home
{
	MenuHomeMoleculeWidget::MenuHomeMoleculeWidget( QWidget * p_parent ) : MenuToolBlockWidget( p_parent )
	{
		_registerEvent( VTX::Event::Global::SELECTION_CHANGE );
	};

	void MenuHomeMoleculeWidget::receiveEvent( const VTX::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::Event::SELECTION_CHANGE )
		{
			const VTX::Event::VTXEventPtr<Model::Selection> & castedEvent
				= dynamic_cast<const VTX::Event::VTXEventPtr<Model::Selection> &>( p_event );

			const bool enableSelection = castedEvent.ptr->getMoleculeSelectedCount() > 0;
			_enableButtons( enableSelection );
		}
	}

	void MenuHomeMoleculeWidget::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		_openTrajectoryButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "openTrajectoryOnMolecule" );
		_openTrajectoryButton->setData(
			"Open\nTrajectory", ":/sprite/load_trajectory_icon.png", Qt::Orientation::Vertical );
		pushButton( *_openTrajectoryButton, 0 );

		_exportSelectionButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "exportSelectionButton" );
		_exportSelectionButton->setData( "Export", ":/sprite/save_molecule_icon.png", Qt::Orientation::Vertical );
		pushButton( *_exportSelectionButton, 1 );

		_enableButtons( false );

		validate();
	}
	void MenuHomeMoleculeWidget::_setupSlots()
	{
		_openTrajectoryButton->setTriggerAction( this, &MenuHomeMoleculeWidget::_openTrajectory );
		_exportSelectionButton->setTriggerAction( this, &MenuHomeMoleculeWidget::_exportSelection );
	}
	void MenuHomeMoleculeWidget::localize() { setTitle( "Molecule" ); }

	void MenuHomeMoleculeWidget::_exportSelection() const { UI::Dialog::openExportMoleculeDialog(); }
	void MenuHomeMoleculeWidget::_openTrajectory() const { UI::Dialog::openLoadTrajectoryDialog(); }

	void MenuHomeMoleculeWidget::_enableButtons( const bool p_enable )
	{
		_exportSelectionButton->setEnabled( p_enable );
	}

} // namespace VTX::UI::Widget::MainMenu::Home
