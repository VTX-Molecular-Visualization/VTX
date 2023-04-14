#include "structural_alignment_widget.hpp"
#include "old_tool/action/analysis.hpp"
#include "old_tool/analysis/rmsd.hpp"
#include "structural_alignment_model_list_widget.hpp"
#include <app/old_app/action/action_manager.hpp>
#include <app/old_app/model/molecule.hpp>
#include <app/old_app/model/selection.hpp>
#include <app/old_app/mvc/mvc_manager.hpp>
#include <app/old_app/object3d/scene.hpp>
#include <app/old_app/selection/selection_manager.hpp>
#include <app/old_app/vtx_app.hpp>
#include <ui/old_ui/style.hpp>
#include <ui/old_ui/ui/widget_factory.hpp>

namespace VTX::UI::Widget::Analysis::StructuralAlignment
{
	StructuralAlignmentWidget::StructuralAlignmentWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
	{
		_registerEvent( VTX::Event::Global::STRUCTURAL_ALIGNMENT_COMPUTED );
	}
	void StructuralAlignmentWidget::receiveEvent( const VTX::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::Event::Global::STRUCTURAL_ALIGNMENT_COMPUTED )
		{
			const VTX::Event::VTXEventRef<const VTX::Analysis::StructuralAlignment::AlignmentResult> & castedEvent
				= dynamic_cast<
					const VTX::Event::VTXEventRef<const VTX::Analysis::StructuralAlignment::AlignmentResult> &>(
					p_event );

			VTX::Analysis::StructuralAlignment::AlignmentResult alignmentResult = castedEvent.ref;

			VTX_INFO( "RMSD : {} over {} residues",
					  alignmentResult.alignedResiduesRMSD,
					  alignmentResult.alignedResidueCount );

			if ( _isConcernedByAlignment( alignmentResult ) )
			{
				_moleculeList->resetRMSD( alignmentResult.staticMolecule );
				_moleculeList->setRMSD( alignmentResult.mobileMolecule,
										alignmentResult.alignedResiduesRMSD,
										alignmentResult.alignedResidueCount );
			}
		}
	}

	bool StructuralAlignmentWidget::_isConcernedByAlignment(
		const VTX::Analysis::StructuralAlignment::AlignmentResult & p_result ) const
	{
		bool staticMoleculeIsTicked = _moleculeList->getTickedModel() == p_result.staticMolecule;

		const std::vector<Model::BaseModel *> notTickedModels = _moleculeList->getNotTickedModels();
		bool								  mobileMoleculeIsInList
			= std::find( notTickedModels.begin(), notTickedModels.end(), p_result.mobileMolecule )
			  != notTickedModels.end();

		return staticMoleculeIsTicked && mobileMoleculeIsInList;
	}

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

		_moleculeList = WidgetFactory::get().instantiateWidget<StructuralAlignmentModelListWidget>(
			attributeWidget, "moleculeListWidget" );

		_moleculesComponent = WidgetFactory::get().instantiateWidget<CustomWidget::ModelListComponent>(
			attributeWidget, _moleculeList, "moleculeComponent" );

		_moleculesComponent->setTitle( "Molecules" );
		_alignmentParametersWidget = WidgetFactory::get().instantiateWidget<AlignParametersWidget>(
			attributeWidget, "alignmentParametersWidget" );
		_moleculesComponent->setFoldState( true );
		_moleculesComponent->addTypeFilter( ID::Model::MODEL_MOLECULE );
		_moleculesComponent->setContainsOnlyUniqueModel( true );

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

		attributesLayout->addAttribute( _moleculesComponent );
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
		connect( _moleculesComponent,
				 &CustomWidget::ModelListComponent::onModelListChange,
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
		const bool alignButtonEnabled = _moleculesComponent->getModelCount() >= 2;
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
			_moleculesComponent->addModel( selectedMolecules[ i ] );
	}

	void StructuralAlignmentWidget::_computeAlign() const
	{
		const Model::Molecule * const  staticMolecule	= _moleculeList->getTickedModel<Model::Molecule>();
		std::vector<Model::Molecule *> mobilesMolecules = _moleculeList->getNotTickedModels<Model::Molecule>();

		// TODO : Display error message here (red text under empty field ?)
		if ( staticMolecule == nullptr || mobilesMolecules.size() == 0 )
			return;

		const VTX::Analysis::StructuralAlignment::AlignmentParameters * const parameters
			= _alignmentParametersWidget->generateParameters();

		VTX_ACTION( new Action::Analysis::ComputeStructuralAlignment( staticMolecule, mobilesMolecules, parameters ) );

		delete parameters;
	}

} // namespace VTX::UI::Widget::Analysis::StructuralAlignment
