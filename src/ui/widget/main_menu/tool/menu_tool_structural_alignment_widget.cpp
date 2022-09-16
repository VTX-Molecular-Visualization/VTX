#include "menu_tool_structural_alignment_widget.hpp"
#include "action/action_manager.hpp"
#include "action/analysis.hpp"
#include "id.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "selection/selection_manager.hpp"
#include "ui/main_window.hpp"
#include "ui/widget_factory.hpp"
#include "util/analysis.hpp"
#include "vtx_app.hpp"
#include <QAction>
#include <QMenu>

namespace VTX::UI::Widget::MainMenu::Tool
{
	MenuToolStructuralAlignmentWidget::MenuToolStructuralAlignmentWidget( QWidget * p_parent ) :
		MenuToolBlockWidget( p_parent )
	{
		_registerEvent( Event::Global::SELECTION_CHANGE );
	}

	MenuToolStructuralAlignmentWidget::~MenuToolStructuralAlignmentWidget()
	{
		if ( _alignmentParameter != nullptr )
			delete _alignmentParameter;
	}

	void MenuToolStructuralAlignmentWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::SELECTION_CHANGE )
			_refreshButtons();
	}

	void MenuToolStructuralAlignmentWidget::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		setTitle( "Measurement" );

		_rmsdButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "rmsdButton" );
		_rmsdButton->setData( "RMSD", ":/sprite/measurement_distance_icon.png", Qt::Orientation::Vertical );
		pushButton( *_rmsdButton, 0 );

		_structuralAlignmentButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonSubmenuWidget>( this, "structuralAlignmentButton" );
		_structuralAlignmentButton->setData(
			"Structural alignment", ":/sprite/measurement_distance_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_structuralAlignmentButton, 1 );

		_structuralAlignmentAdvancedButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "structuralAlignmentAdvancedButton" );
		_structuralAlignmentAdvancedButton->setData(
			"Alignment...", ":/sprite/settings_icon.png", Qt::Orientation::Horizontal );
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
		const Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		return selectionModel.getMoleculeSelectedCount() >= 2;
	}
	bool MenuToolStructuralAlignmentWidget::_checkStructuralAlignmentEnableSate() const
	{
		const Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		return selectionModel.getMoleculeSelectedCount() >= 2;
	}

	void MenuToolStructuralAlignmentWidget::_refreshButtons() const
	{
		_rmsdButton->setEnabled( _checkRMSDEnableSate() );
		_structuralAlignmentButton->setEnabled( _checkStructuralAlignmentEnableSate() );
	}

	void MenuToolStructuralAlignmentWidget::_computeRMSDAction() const
	{
		const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();

		const Model::Molecule *				 target;
		std::vector<const Model::Molecule *> comparers;
		Util::Analysis::pickTargetAndComparersFromSelection( selection, target, comparers );

		std::vector<const Model::Molecule *> comparersConst = std::vector<const Model::Molecule *>();
		comparersConst.resize( comparers.size() );
		std::move( comparers.begin(), comparers.end(), comparersConst.begin() );

		VTX_ACTION( new Action::Analysis::ComputeRMSD( target, comparersConst ) );
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
		const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();

		const Model::Molecule *		   staticMolecule;
		std::vector<Model::Molecule *> mobileMolecules;
		Util::Analysis::pickTargetAndComparersFromSelection( selection, staticMolecule, mobileMolecules );

		VTX_ACTION(
			new Action::Analysis::ComputeStructuralAlignment( staticMolecule, mobileMolecules, _alignmentParameter ) );
	}

	void MenuToolStructuralAlignmentWidget::_openStructuralAlignmentWindow() const
	{
		VTXApp::get().getMainWindow().showWidget( ID::UI::Window::STRUCTURAL_ALIGNMENT, true );
	}

} // namespace VTX::UI::Widget::MainMenu::Tool
