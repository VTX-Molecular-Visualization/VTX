#include "menu_tool_structural_alignment_widget.hpp"
#include "action/action_manager.hpp"
#include "action/analysis.hpp"
#include "id.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
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
		_structuralAlignmentPymolButton->setEnabled( _checkStructuralAlignmentEnableSate() );
	}

	void MenuToolStructuralAlignmentWidget::_computeRMSDAction() const
	{
		const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();

		const Model::Molecule *		   target;
		std::vector<Model::Molecule *> comparers;
		_prepareTargetAndComparerForComparison( selection, target, comparers );

		std::vector<const Model::Molecule *> comparersConst = std::vector<const Model::Molecule *>();
		comparersConst.resize( comparers.size() );
		std::move( comparers.begin(), comparers.end(), comparersConst.begin() );

		VTX_ACTION( new Action::Analysis::ComputeRMSD( target, comparersConst ) );
	}

	void MenuToolStructuralAlignmentWidget::_computeStructuralAlignmentAction() const
	{
		const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();

		const Model::Molecule *		   staticMolecule;
		std::vector<Model::Molecule *> mobileMolecules;
		_prepareTargetAndComparerForComparison( selection, staticMolecule, mobileMolecules );

		VTX_ACTION( new Action::Analysis::ComputeStructuralAlignment( staticMolecule, mobileMolecules ) );
	}

	void MenuToolStructuralAlignmentWidget::_computeStructuralAlignmentPymolAction() const
	{
		const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();

		const Model::Molecule *		   staticMolecule;
		std::vector<Model::Molecule *> mobileMolecules;
		_prepareTargetAndComparerForComparison( selection, staticMolecule, mobileMolecules );

		VTX_ACTION( new Action::Analysis::ComputePymolStructuralAlignment( staticMolecule, *mobileMolecules.begin() ) );
	}

	void MenuToolStructuralAlignmentWidget::_prepareTargetAndComparerForComparison(
		const Model::Selection &		 p_selection,
		const Model::Molecule *&		 p_target,
		std::vector<Model::Molecule *> & p_comparers ) const
	{
		Model::Selection::MapMoleculeIds::const_iterator it = p_selection.getMoleculesMap().begin();

		std::list<Model::Molecule *> sortedMolecules = std::list<Model::Molecule *>();

		for ( const Model::Selection::PairMoleculeIds & pairMoleculeID : p_selection.getMoleculesMap() )
		{
			Model::Molecule * const currentMolecule
				= &MVC::MvcManager::get().getModel<Model::Molecule>( pairMoleculeID.first );

			const int currentMoleculeSceneIndex = VTXApp::get().getScene().getItemPosition( *currentMolecule );

			bool inserted = false;

			for ( std::list<Model::Molecule *>::const_iterator it = sortedMolecules.begin();
				  it != sortedMolecules.end();
				  it++ )
			{
				const int sortedMoleculeSceneIndex = VTXApp::get().getScene().getItemPosition( **it );

				if ( currentMoleculeSceneIndex < sortedMoleculeSceneIndex )
				{
					sortedMolecules.insert( it, currentMolecule );

					inserted = true;
					break;
				}
			}

			if ( !inserted )
			{
				sortedMolecules.push_back( currentMolecule );
			}
		}

		p_target	= *sortedMolecules.begin();
		p_comparers = std::vector<Model::Molecule *>();
		p_comparers.resize( sortedMolecules.size() - 1 );

		std::move( ++sortedMolecules.begin(), sortedMolecules.end(), p_comparers.begin() );
	}

} // namespace VTX::UI::Widget::MainMenu::Tool
