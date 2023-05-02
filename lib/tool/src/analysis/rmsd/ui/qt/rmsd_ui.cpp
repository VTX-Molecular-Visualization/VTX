#include "tool/analysis/rmsd/ui/qt/rmsd_ui.hpp"
#include "tool/analysis/rmsd/action.hpp"
// #include "ui/widget/main_menu/tool/menu_tool_structural_alignment_widget.hpp"

#include <app/application/selection/selection.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/core/event/vtx_event.hpp>
#include <app/event/global.hpp>
#include <ui/qt/application_qt.hpp>
#include <ui/qt/main_window.hpp>
#include <ui/qt/widget/main_menu/menu_tooltab_widget.hpp>
#include <ui/qt/widget_factory.hpp>

namespace VTX::Tool::Analysis::RMSD::UI::QT
{
	RMSDTool::RMSDTool() : VTX::UI::QT::BaseQtTool(), VTX::App::Core::Event::BaseEventReceiverVTX()
	{
		_registerEvent( VTX::App::Event::Global::SELECTION_CHANGE );
	}

	void RMSDTool::instantiateTool()
	{
		_addButtonsInMainMenu();
		_refreshButton();

		// VTXApp::get().getMainWindow().getContextualMenu().getMenu()
	}

	void RMSDTool::receiveEvent( const App::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Event::Global::SELECTION_CHANGE )
			_refreshButton();
	}

	void RMSDTool::_addButtonsInMainMenu()
	{
		const VTX::UI::Core::ToolLayoutData & layoutData = getLayoutData();

		VTX::UI::QT::Widget::MainMenu::MenuTooltabWidget & tooltab
			= dynamic_cast<VTX::UI::QT::Widget::MainMenu::MenuTooltabWidget &>(
				VTX::UI::QT::QT_APP()->getMainWindow().getMainMenu().getTab( layoutData.tabName ) );

		VTX::UI::QT::Widget::MainMenu::MenuToolBlockWidget & toolBlock
			= dynamic_cast<VTX::UI::QT::Widget::MainMenu::MenuToolBlockWidget &>(
				tooltab.getToolBlock( layoutData.blockName ) );

		_rmsdButton
			= VTX::UI::QT::WidgetFactory::get().instantiateWidget<VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget>(
				&tooltab, "rmsdButton" );
		_rmsdButton->setData( "RMSD", ":/sprite/analysis/compute_rmsd_icon.png", Qt::Orientation::Vertical );

		_rmsdButton->setTriggerAction( this, &RMSDTool::_computeRMSDAction );

		toolBlock.pushButton( *_rmsdButton );
	}

	void RMSDTool::_refreshButton() const { _rmsdButton->setEnabled( _checkRMSDEnableSate() ); }
	bool RMSDTool::_checkRMSDEnableSate() const
	{
		const App::Application::Selection::SelectionModel & selectionModel
			= VTX::App::Application::Selection::SelectionManager::get().getSelectionModel();
		return selectionModel.getMoleculeSelectedCount() >= 2;
	}

	void RMSDTool::_computeRMSDAction() const
	{
		const App::Application::Selection::SelectionModel & selection
			= VTX::App::Application::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION( new VTX::Tool::Analysis::RMSD::Action::ComputeRMSD( selection ) );
	}

} // namespace VTX::Tool::Analysis::RMSD::UI::QT
