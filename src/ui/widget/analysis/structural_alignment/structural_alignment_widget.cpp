#include "structural_alignment_widget.hpp"
#include "action/action_manager.hpp"
#include "action/analysis.hpp"
#include "model/molecule.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "selection/selection_manager.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::Analysis::StructuralAlignment
{
	StructuralAlignmentWidget::StructuralAlignmentWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}
	void StructuralAlignmentWidget::receiveEvent( const Event::VTXEvent & p_event ) {}

	void StructuralAlignmentWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_mainWidget = new CustomWidget::DockWindowMainWidget<QWidget>(
			Style::SETTINGS_PREFERRED_SIZE, Style::SETTINGS_MINIMUM_SIZE, this );

		QVBoxLayout * const mainLayout		= new QVBoxLayout( _mainWidget );
		QWidget * const		attributeWidget = new QWidget( this );

		QScrollArea * const scrollArea = new QScrollArea( _mainWidget );
		QSizePolicy			sizePolicy = QSizePolicy( QSizePolicy::Policy::MinimumExpanding,
											  QSizePolicy::Policy::MinimumExpanding,
											  QSizePolicy::ControlType::Frame );
		sizePolicy.setHorizontalStretch( 10 );
		sizePolicy.setVerticalStretch( 10 );
		scrollArea->setSizePolicy( sizePolicy );

		scrollArea->setFrameShape( QFrame::Shape::NoFrame );
		scrollArea->setWidget( attributeWidget );
		scrollArea->setWidgetResizable( true );
		scrollArea->setSizeAdjustPolicy( QAbstractScrollArea::SizeAdjustPolicy::AdjustIgnored );

		QVBoxLayout * const attributeVLayout = new QVBoxLayout( attributeWidget );
		attributeVLayout->setContentsMargins( 0, 0, 0, 0 );

		Layout::AttributeListLayout * const attributesLayout = new Layout::AttributeListLayout();

		_staticMoleculeField = WidgetFactory::get().instantiateWidget<CustomWidget::ModelFieldWidget>(
			attributeWidget, "staticMoleculeField" );
		_mobileMoleculeField = WidgetFactory::get().instantiateWidget<CustomWidget::ModelFieldWidget>(
			attributeWidget, "mobileMoleculeField" );
		_alignmentParametersWidget
			= WidgetFactory::get().instantiateWidget<AlignParametersWidget>( attributeWidget, "mobileMoleculeField" );
		_alignmentParametersWidget->displayAsFoldable( true );

		const VTX::Analysis::StructuralAlignment::AlignmentParameters * const defaultParameters
			= VTX::Analysis::StructuralAlignment::instantiateDefaultParameters(
				VTX::Analysis::StructuralAlignment::AlignmentMethodEnum::CEAlign );

		_alignmentParametersWidget->applyParameter( *defaultParameters );

		delete defaultParameters;

		_alignButton = new QPushButton( this );
		_alignButton->setText( "Align" );

		attributesLayout->addAttribute( _staticMoleculeField, "Static molecule" );
		attributesLayout->addAttribute( _mobileMoleculeField, "Mobile molecule" );
		attributesLayout->addAttribute( _alignmentParametersWidget );

		attributeVLayout->addLayout( attributesLayout );
		attributeVLayout->addStretch( 1000 );

		mainLayout->addWidget( scrollArea, 1000 );
		mainLayout->addWidget( _alignButton );

		setWidget( _mainWidget );
		refresh();
	}
	void StructuralAlignmentWidget::_setupSlots()
	{
		connect( _alignButton, &QPushButton::clicked, this, &StructuralAlignmentWidget::_computeAlign );
	}

	void StructuralAlignmentWidget::localize() { setWindowTitle( "Structural Alignment" ); }

	void StructuralAlignmentWidget::showEvent( QShowEvent * p_event )
	{
		const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();
		_updateTargetedMoleculesWithSelection( selection );

		refresh();
	}

	void StructuralAlignmentWidget::refresh() {}

	void StructuralAlignmentWidget::_updateTargetedMoleculesWithSelection( const Model::Selection & p_selection )
	{
		std::vector<Model::Molecule *> selectedMolecules = std::vector<Model::Molecule *>();
		selectedMolecules.reserve( p_selection.getMoleculesMap().size() );

		for ( const Model::Selection::PairMoleculeIds & pairMolIDs : p_selection.getMoleculesMap() )
		{
			Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( pairMolIDs.first );
			selectedMolecules.emplace_back( &molecule );
		}

		VTXApp::get().getScene().sortMoleculesBySceneIndex( selectedMolecules );

		Model::Molecule * const staticMolecule = selectedMolecules.size() > 0 ? selectedMolecules[ 0 ] : nullptr;
		Model::Molecule * const mobileMolecule = selectedMolecules.size() >= 2 ? selectedMolecules[ 1 ] : nullptr;

		_staticMoleculeField->setModel( staticMolecule );
		_mobileMoleculeField->setModel( mobileMolecule );
	}
	void StructuralAlignmentWidget::_computeAlign() const
	{
		const Model::Molecule * const staticMolecule = _staticMoleculeField->getModel<Model::Molecule>();
		Model::Molecule * const		  mobileMolecule = _mobileMoleculeField->getModel<Model::Molecule>();

		// TODO : Display error message here (red text under empty field ?)
		if ( staticMolecule == nullptr || mobileMolecule == nullptr )
			return;

		const VTX::Analysis::StructuralAlignment::AlignmentParameters * const parameters
			= _alignmentParametersWidget->generateParameters();

		VTX_ACTION(
			new Action::Analysis::ComputeStructuralAlignment( staticMolecule, { mobileMolecule }, parameters ) );

		delete parameters;
	}

} // namespace VTX::UI::Widget::Analysis::StructuralAlignment
