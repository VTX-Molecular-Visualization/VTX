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

		CustomWidget::DockWindowMainWidget<QWidget> * const mainWidget
			= new CustomWidget::DockWindowMainWidget<QWidget>(
				Style::SETTINGS_PREFERRED_SIZE, Style::SETTINGS_MINIMUM_SIZE, this );

		QVBoxLayout * const mainLayout	   = new QVBoxLayout( mainWidget );
		QWidget * const		scrollAreaBody = new QWidget( this );

		QScrollArea * const scrollArea = new QScrollArea( mainWidget );
		QSizePolicy			sizePolicy = QSizePolicy( QSizePolicy::Policy::MinimumExpanding,
											  QSizePolicy::Policy::MinimumExpanding,
											  QSizePolicy::ControlType::Frame );
		sizePolicy.setHorizontalStretch( 10 );
		sizePolicy.setVerticalStretch( 10 );
		scrollArea->setSizePolicy( sizePolicy );

		scrollArea->setFrameShape( QFrame::Shape::NoFrame );
		scrollArea->setWidget( scrollAreaBody );
		scrollArea->setWidgetResizable( true );
		scrollArea->setSizeAdjustPolicy( QAbstractScrollArea::SizeAdjustPolicy::AdjustIgnored );

		QVBoxLayout * const attributeVLayout = new QVBoxLayout( scrollAreaBody );
		attributeVLayout->setContentsMargins( 0, 0, 0, 0 );

		QWidget * const						attributeWidget	 = new QWidget( scrollAreaBody );
		Layout::AttributeListLayout * const attributesLayout = new Layout::AttributeListLayout( attributeWidget );

		_moleculesField = WidgetFactory::get().instantiateWidget<CustomWidget::ModelFieldListWidget>( attributeWidget,
																									  "moleculeField" );
		_moleculesField->setTitle( "Molecules" );
		_alignmentParametersWidget = WidgetFactory::get().instantiateWidget<AlignParametersWidget>(
			attributeWidget, "alignmentParametersWidget" );
		_moleculesField->setFoldState( true );

		CustomWidget::FoldingButton * const parametersFoldingButton
			= WidgetFactory::get().instantiateWidget<CustomWidget::FoldingButton>(
				attributeWidget, _alignmentParametersWidget, "foldButton" );
		parametersFoldingButton->setTitle( "Parameters" );

		const VTX::Analysis::StructuralAlignment::AlignmentParameters * const defaultParameters
			= VTX::Analysis::StructuralAlignment::instantiateDefaultParameters(
				VTX::Analysis::StructuralAlignment::AlignmentMethodEnum::CEAlign );

		_alignmentParametersWidget->applyParameter( *defaultParameters );

		delete defaultParameters;

		_alignButton = new QPushButton( this );
		_alignButton->setText( "Align" );

		attributesLayout->addAttribute( _moleculesField );
		attributesLayout->addAttribute( parametersFoldingButton );

		attributeVLayout->addWidget( attributeWidget );
		attributeVLayout->addStretch( 1000 );

		mainLayout->addWidget( scrollArea, 1000 );
		mainLayout->addWidget( _alignButton );

		setWidget( mainWidget );
		refresh();
	}
	void StructuralAlignmentWidget::_setupSlots()
	{
		connect( _alignButton, &QPushButton::clicked, this, &StructuralAlignmentWidget::_computeAlign );
		connect( _moleculesField,
				 &CustomWidget::ModelFieldListWidget::onModelListChange,
				 this,
				 &StructuralAlignmentWidget::_onModelListChange );
	}

	void StructuralAlignmentWidget::_onModelListChange() { refresh(); }

	void StructuralAlignmentWidget::localize() { setWindowTitle( "Structural Alignment" ); }

	void StructuralAlignmentWidget::showEvent( QShowEvent * p_event )
	{
		const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();
		_updateTargetedMoleculesWithSelection( selection );

		refresh();
	}

	void StructuralAlignmentWidget::refresh()
	{
		const bool alignButtonEnabled = _moleculesField->getModelCount() >= 2;
		_alignButton->setEnabled( alignButtonEnabled );
	}

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

		for ( int i = 0; i < selectedMolecules.size(); i++ )
			_moleculesField->addModel( selectedMolecules[ i ] );
	}

	void StructuralAlignmentWidget::_computeAlign() const
	{
		const Model::Molecule * const  staticMolecule	= _moleculesField->getTickedModel<Model::Molecule>();
		std::vector<Model::Molecule *> mobilesMolecules = _moleculesField->getNotTickedModels<Model::Molecule>();

		// TODO : Display error message here (red text under empty field ?)
		if ( staticMolecule == nullptr || mobilesMolecules.size() == 0 )
			return;

		const VTX::Analysis::StructuralAlignment::AlignmentParameters * const parameters
			= _alignmentParametersWidget->generateParameters();

		VTX_ACTION( new Action::Analysis::ComputeStructuralAlignment( staticMolecule, mobilesMolecules, parameters ) );

		delete parameters;
	}

} // namespace VTX::UI::Widget::Analysis::StructuralAlignment
