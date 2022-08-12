#include "menu_tool_structural_alignment_widget.hpp"
#include "action/action_manager.hpp"
#include "action/analysis.hpp"
#include "id.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "selection/selection_manager.hpp"
#include "ui/widget_factory.hpp"

namespace VTX::UI::Widget::MainMenu::Tool
{
	MenuToolStructuralAlignmentWidget::MenuToolStructuralAlignmentWidget( QWidget * p_parent ) :
		MenuToolBlockWidget( p_parent )
	{
		_registerEvent( Event::Global::SELECTION_CHANGE );
	}

	MenuToolStructuralAlignmentWidget::~MenuToolStructuralAlignmentWidget() {}

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
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "structuralAlignmentButton" );
		_structuralAlignmentButton->setData(
			"Structural\nAlignment", ":/sprite/measurement_distance_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_structuralAlignmentButton, 1 );

		_structuralAlignmentPymolButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "structuralAlignmentPymolButton" );
		_structuralAlignmentPymolButton->setData(
			"Pymol\nAlignment", ":/sprite/measurement_distance_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_structuralAlignmentPymolButton, 1 );

		validate();

		_refreshButtons();
	}
	void MenuToolStructuralAlignmentWidget::_setupSlots()
	{
		_rmsdButton->setTriggerAction( this, &MenuToolStructuralAlignmentWidget::_computeRMSDAction );

		_structuralAlignmentButton->setTriggerAction(
			this, &MenuToolStructuralAlignmentWidget::_computeStructuralAlignmentAction );

		_structuralAlignmentPymolButton->setTriggerAction(
			this, &MenuToolStructuralAlignmentWidget::_computeStructuralAlignmentPymolAction );
	}
	void MenuToolStructuralAlignmentWidget::localize() {}

	bool MenuToolStructuralAlignmentWidget::_checkRMSDEnableSate() const
	{
		const Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		return selectionModel.getMoleculeSelectedCount() == 2;
	}
	bool MenuToolStructuralAlignmentWidget::_checkStructuralAlignmentEnableSate() const
	{
		const Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		return selectionModel.getMoleculeSelectedCount() == 2;
	}

	void MenuToolStructuralAlignmentWidget::_refreshButtons() const
	{
		_rmsdButton->setEnabled( _checkRMSDEnableSate() );
		_structuralAlignmentButton->setEnabled( _checkStructuralAlignmentEnableSate() );
		_structuralAlignmentPymolButton->setEnabled( _checkStructuralAlignmentEnableSate() );
	}

	void MenuToolStructuralAlignmentWidget::_computeRMSDAction() const
	{
		const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();

		Model::Selection::MapMoleculeIds::const_iterator it = selection.getMoleculesMap().cbegin();

		const Model::ID & firstMoleculeID = it->first;
		it++;
		const Model::ID & secondMoleculeID = it->first;

		const Model::Molecule & firstMolecule  = MVC::MvcManager::get().getModel<Model::Molecule>( firstMoleculeID );
		const Model::Molecule & secondMolecule = MVC::MvcManager::get().getModel<Model::Molecule>( secondMoleculeID );

		VTX_ACTION( new Action::Analysis::ComputeRMSD( firstMolecule, secondMolecule ) );
	}
	void MenuToolStructuralAlignmentWidget::_computeStructuralAlignmentAction() const
	{
		const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();

		Model::Selection::MapMoleculeIds::const_iterator it = selection.getMoleculesMap().begin();

		const Model::ID & firstMoleculeID = it->first;
		it++;
		const Model::ID & secondMoleculeID = it->first;

		const Model::Molecule & firstMolecule  = MVC::MvcManager::get().getModel<Model::Molecule>( firstMoleculeID );
		Model::Molecule &		secondMolecule = MVC::MvcManager::get().getModel<Model::Molecule>( secondMoleculeID );

		VTX_ACTION( new Action::Analysis::ComputeStructuralAlignment( &firstMolecule, &secondMolecule ) );
	}

	void MenuToolStructuralAlignmentWidget::_computeStructuralAlignmentPymolAction() const
	{
		const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();

		Model::Selection::MapMoleculeIds::const_iterator it = selection.getMoleculesMap().begin();

		const Model::ID & firstMoleculeID = it->first;
		it++;
		const Model::ID & secondMoleculeID = it->first;

		const Model::Molecule & firstMolecule  = MVC::MvcManager::get().getModel<Model::Molecule>( firstMoleculeID );
		Model::Molecule &		secondMolecule = MVC::MvcManager::get().getModel<Model::Molecule>( secondMoleculeID );

		VTX_ACTION( new Action::Analysis::ComputePymolStructuralAlignment( &firstMolecule, &secondMolecule ) );
	}

} // namespace VTX::UI::Widget::MainMenu::Tool
