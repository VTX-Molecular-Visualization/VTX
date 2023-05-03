#include "tool/old_tool/ui/widget/main_menu/tool/menu_tool_structural_alignment_widget.hpp"
#include "tool/old_tool/action/analysis.hpp"
#include "tool/old_tool/util/analysis.hpp"
#include <QAction>
#include <QMenu>
#include <app/application/scene.hpp>
#include <app/application/selection/selection.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/event/global.hpp>
#include <app/mvc.hpp>
#include <app/id.hpp>
#include <ui/old_ui/ui/main_window.hpp>
#include <ui/old_ui/ui/widget_factory.hpp>
#include <ui/old_ui/vtx_app.hpp>

namespace VTX::UI::Widget::MainMenu::Tool
{
	MenuToolStructuralAlignmentWidget::MenuToolStructuralAlignmentWidget( QWidget * p_parent ) :
		MenuToolBlockWidget( p_parent )
	{
		_registerEvent( VTX::App::Event::Global::SELECTION_CHANGE );
	}

	MenuToolStructuralAlignmentWidget::~MenuToolStructuralAlignmentWidget()
	{
		if ( _alignmentParameter != nullptr )
			delete _alignmentParameter;
	}

	void MenuToolStructuralAlignmentWidget::receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Event::Global::SELECTION_CHANGE )
			_refreshButtons();
	}

	void MenuToolStructuralAlignmentWidget::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		setTitle( "Structural Alignment" );

		_rmsdButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "rmsdButton" );
		_rmsdButton->setData( "RMSD", ":/sprite/analysis_compute_rmsd_icon.png", Qt::Orientation::Vertical );
		pushButton( *_rmsdButton, 0 );

		_structuralAlignmentButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonSubmenuWidget>( this, "structuralAlignmentButton" );
		_structuralAlignmentButton->setData(
			"Structural alignment", ":/sprite/structural_alignment_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_structuralAlignmentButton, 1 );

		_structuralAlignmentAdvancedButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "structuralAlignmentAdvancedButton" );
		_structuralAlignmentAdvancedButton->setData(
			"Alignment\nwindow", ":/sprite/structural_alignment_settings_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_structuralAlignmentAdvancedButton, 1 );

		validate();

		_refreshButtons();
	}
	void MenuToolStructuralAlignmentWidget::_setupSlots()
	{
		_rmsdButton->setTriggerAction( this, &MenuToolStructuralAlignmentWidget::_computeRMSDAction );

		_structuralAlignmentButton->addAction(
			"CE Align", this, &MenuToolStructuralAlignmentWidget::_computeStructuralAlignmentAction, true );

		_structuralAlignmentAdvancedButton->setTriggerAction(
			this, &MenuToolStructuralAlignmentWidget::_openStructuralAlignmentWindow );
	}
	void MenuToolStructuralAlignmentWidget::localize() {}

	bool MenuToolStructuralAlignmentWidget::_checkRMSDEnableSate() const
	{
		const App::Application::Selection::SelectionModel & selectionModel
			= VTX::App::Application::Selection::SelectionManager::get().getSelectionModel();
		return selectionModel.getMoleculeSelectedCount() >= 2;
	}
	bool MenuToolStructuralAlignmentWidget::_checkStructuralAlignmentEnableSate() const
	{
		const App::Application::Selection::SelectionModel & selectionModel
			= VTX::App::Application::Selection::SelectionManager::get().getSelectionModel();
		return selectionModel.getMoleculeSelectedCount() >= 2;
	}

	void MenuToolStructuralAlignmentWidget::_refreshButtons() const
	{
		_rmsdButton->setEnabled( _checkRMSDEnableSate() );
		_structuralAlignmentButton->setEnabled( _checkStructuralAlignmentEnableSate() );
	}

	void MenuToolStructuralAlignmentWidget::_computeRMSDAction() const
	{
		const App::Application::Selection::SelectionModel & selection
			= VTX::App::Application::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION( new Action::Analysis::ComputeRMSD( selection ) );
	}

	void MenuToolStructuralAlignmentWidget::_computeStructuralAlignmentAction()
	{
		if ( _alignmentParameter != nullptr )
			delete _alignmentParameter;

		_alignmentParameter = VTX::Analysis::StructuralAlignment::instantiateDefaultParameters(
			VTX::Analysis::StructuralAlignment::AlignmentMethodEnum::CEAlign );

		_launchStructuralAlignmentAction();
	}

	void MenuToolStructuralAlignmentWidget::_launchStructuralAlignmentAction() const
	{
		const App::Application::Selection::SelectionModel & selection
			= VTX::App::Application::Selection::SelectionManager::get().getSelectionModel();

		const App::Component::Chemistry::Molecule *		   staticMolecule;
		std::vector<App::Component::Chemistry::Molecule *> mobileMolecules;
		Util::Analysis::pickTargetAndComparersFromSelection( selection, staticMolecule, mobileMolecules );

		VTX_ACTION(
			new Action::Analysis::ComputeStructuralAlignment( staticMolecule, mobileMolecules, _alignmentParameter ) );
	}

	void MenuToolStructuralAlignmentWidget::_openStructuralAlignmentWindow() const
	{
		VTXApp::get().getMainWindow().showWidget( UI::ID::Window::STRUCTURAL_ALIGNMENT, true );
	}

} // namespace VTX::UI::Widget::MainMenu::Tool
