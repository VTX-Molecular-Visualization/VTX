#include "ui/old_ui/ui/widget/inspector/multiple_molecule_inspector_widget.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/ui/layout/attribute_list_layout.hpp"
#include "ui/old_ui/ui/main_window.hpp"
#include "ui/old_ui/ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/folding_button.hpp"
#include "ui/old_ui/ui/widget/inspector/inspector_widget.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <QBoxLayout>
#include <QFont>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <app/old/action/instantiated_representation.hpp>
#include <app/old/action/molecule.hpp>
#include <app/old/action/transformable.hpp>
#include <app/old/application/representation/representation_manager.hpp>
#include <app/old/event/global.hpp>

namespace VTX::UI::Widget::Inspector
{
	MultipleMoleculeWidget::MultipleMoleculeWidget( QWidget * p_parent ) :
		MultipleModelInspectorWidget( p_parent, ID::View::UI_INSPECTOR_MOLECULE_STRUCTURE )
	{
		_registerEvent( VTX::App::Old::Event::Global::LATE_UPDATE );
	};

	MultipleMoleculeWidget::~MultipleMoleculeWidget() {}

	void MultipleMoleculeWidget::_setupUi( const QString & p_name )
	{
		MultipleModelInspectorWidget::_setupUi( p_name );

		_transformSection
			= VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSection>( this, "inspector_item_section" );
		_transformWidget = VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::TransformWidget>(
			this, "inspector_molecule_transform" );
		_transformSection->setBody( _transformWidget );

		_representationSection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
			this, "inspector_item_section" );

		_representationWidget
			= VTX::UI::WidgetFactory::get().instantiateWidget<Representation::RepresentationInspectorSection>(
				this, "inspector_instantiated_representation" );
		_representationWidget->setActionButtonVisibility(
			Representation::RepresentationInspectorSection::ActionButtons::All );

		_subRepresentationWidget
			= VTX::UI::WidgetFactory::get().instantiateWidget<Representation::InstantiatedRepresentationListWidget>(
				this, "sub_representation_widget" );

		_subRepresentationFoldingButton = VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::FoldingButton>(
			this, _subRepresentationWidget, "_sub_representation_folding_button" );
		_subRepresentationFoldingButton->setTitle( "Representations in children" );

		_moleculeColor
			= VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::ColorFieldButton>( this, "molecule_color" );

		_representationSection->appendField( _representationWidget );
		_representationSection->appendField( _subRepresentationFoldingButton );
		_representationSection->appendField( "Molecule color", _moleculeColor );

		_trajectorySection
			= VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSection>( this, "inspector_item_section" );
		_trajectoryWidget
			= WidgetFactory::get().instantiateWidget<CustomWidget::TrajectoryWidget>( this, "inspector_trajectory" );
		_trajectorySection->setBody( _trajectoryWidget );

		_autoRotateSection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
			this, "inspector_item_section" );
		_autoRotateField = WidgetFactory::get().instantiateWidget<CustomWidget::AutoRotateWidget>(
			this, "inspector_autoRotateVectorField" );
		_autoRotateSection->setBody( _autoRotateField );

		_infoSection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
			this, "inspector_item_section" );
		_fullnameLabel = new CustomWidget::QLabelMultiField( this );
		_fullnameLabel->setWordWrap( true );
		_infoSection->appendField( "Full Name", _fullnameLabel );

		_nbChainsLabel = new CustomWidget::QLabelMultiField( this );
		_nbChainsLabel->setWordWrap( true );
		_infoSection->appendField( "Chains count", _nbChainsLabel );

		_nbResiduesLabel = new CustomWidget::QLabelMultiField( this );
		_nbResiduesLabel->setWordWrap( true );
		_infoSection->appendField( "Residues count", _nbResiduesLabel );

		_nbAtomsLabel = new CustomWidget::QLabelMultiField( this );
		_nbAtomsLabel->setWordWrap( true );
		_infoSection->appendField( "Atoms count", _nbAtomsLabel );

		_appendSection( _transformSection );
		_appendSection( _representationSection );
		_appendSection( _trajectorySection );
		_appendSection( _autoRotateSection );
		_appendSection( _infoSection );

		const bool oldBlockState = blockSignals( true );
		refresh();
		blockSignals( oldBlockState );
	}

	void MultipleMoleculeWidget::_setupSlots()
	{
		QMenu * const headerMenu = new QMenu( this );

		QAction * inspectorToChainAction = new QAction( "Chain", this );
		connect( inspectorToChainAction, &QAction::triggered, this, &MultipleMoleculeWidget::_setInspectorToChain );
		headerMenu->addAction( inspectorToChainAction );

		QAction * inspectorToResidueAction = new QAction( "Residue", this );
		connect( inspectorToResidueAction, &QAction::triggered, this, &MultipleMoleculeWidget::_setInspectorToResidue );
		headerMenu->addAction( inspectorToResidueAction );

		QAction * inspectorToAtomAction = new QAction( "Atom", this );
		connect( inspectorToAtomAction, &QAction::triggered, this, &MultipleMoleculeWidget::_setInspectorToAtom );
		headerMenu->addAction( inspectorToAtomAction );

		_getHeader()->setMenu( headerMenu );

		connect( _transformWidget,
				 &CustomWidget::TransformWidget::onValueChange,
				 this,
				 &MultipleMoleculeWidget::_onTransformChange );
		connect( _transformWidget,
				 &CustomWidget::TransformWidget::onPositionDragged,
				 this,
				 &MultipleMoleculeWidget::_onPositionDragged );
		connect( _transformWidget,
				 &CustomWidget::TransformWidget::onRotationDragged,
				 this,
				 &MultipleMoleculeWidget::_onRotationDragged );
		connect( _transformWidget,
				 &CustomWidget::TransformWidget::onScaleDragged,
				 this,
				 &MultipleMoleculeWidget::_onScaleDragged );

		connect( _representationWidget,
				 &Representation::RepresentationInspectorSection::onRepresentationPresetChange,
				 this,
				 &MultipleMoleculeWidget::_onRepresentationPresetChange );

		connect( _representationWidget,
				 &Representation::RepresentationInspectorSection::onRepresentationChange,
				 this,
				 &MultipleMoleculeWidget::_onRepresentationChange );

		connect( _representationWidget,
				 &Representation::RepresentationInspectorSection::onRepresentationColorChange,
				 this,
				 &MultipleMoleculeWidget::_onRepresentationColorChange );

		connect( _representationWidget,
				 &Representation::RepresentationInspectorSection::onRevertRepresentation,
				 this,
				 &MultipleMoleculeWidget::_onRevertRepresentation );

		connect( _representationWidget,
				 &Representation::RepresentationInspectorSection::onApplyRepresentationToChildren,
				 this,
				 &MultipleMoleculeWidget::_onApplyRepresentationToChildren );

		connect( _moleculeColor,
				 &CustomWidget::ColorFieldButton::onValueChange,
				 this,
				 &MultipleMoleculeWidget::_onMoleculeColorChange );
	};

	void MultipleMoleculeWidget::_endOfFrameRefresh( const SectionFlag & p_flag )
	{
		MultipleModelInspectorWidget::_endOfFrameRefresh( p_flag );

		bool blockSignalState = blockSignals( true );
		_resetFieldStates( p_flag );

		const std::unordered_set<App::Old::Component::Chemistry::Molecule *> & targets = getTargets();

		if ( targets.size() > 0 )
		{
			const QString headerTitle = QString::fromStdString( "Molecule (" + std::to_string( targets.size() ) + ")" );
			_getHeader()->setHeaderTitle( headerTitle );

			const QPixmap * symbolPixmap = Style::IconConst::get().getModelSymbol( App::Old::ID::Model::MODEL_MOLECULE );
			_getHeader()->setHeaderIcon( *symbolPixmap );

			for ( App::Old::Component::Chemistry::Molecule * molecule : targets )
			{
				if ( bool( p_flag & SectionFlag::TRANSFORM ) )
				{
					_transformWidget->updateWithNewValue( molecule->getTransform() );
				}

				if ( bool( p_flag & SectionFlag::REPRESENTATION ) )
				{
					_representationWidget->updateWithNewValue( *molecule->getRepresentation() );

					for ( App::Old::Application::Representation::InstantiatedRepresentation * representation :
						  molecule->getSubRepresentations() )
					{
						_subRepresentationWidget->addModel( representation );
					}

					const std::string strTitle = "Representations in children ("
												 + std::to_string( _subRepresentationWidget->getModelCount() ) + ")";
					_subRepresentationFoldingButton->setTitle( QString::fromStdString( strTitle ) );
				}

				if ( bool( p_flag & SectionFlag::TRAJECTORY ) )
				{
					if ( molecule->hasTrajectory() )
					{
						_trajectorySection->setVisible( true );
						_trajectoryWidget->updateWithNewValue( *molecule );
					}
				}
				else if ( bool( p_flag & SectionFlag::TRAJECTORY_TIMER ) )
				{
					_trajectoryWidget->refreshTimer();
				}

				if ( bool( p_flag & SectionFlag::AUTO_ROTATE ) )
				{
					_autoRotateField->updateWithNewValue( *molecule );
				}

				if ( bool( p_flag & SectionFlag::INFOS ) )
				{
					_moleculeColor->updateWithNewValue( molecule->getColor() );

					_fullnameLabel->updateWithNewValue( molecule->getName() );
					_nbChainsLabel->updateWithNewValue( std::to_string( molecule->getRealChainCount() ) );
					_nbResiduesLabel->updateWithNewValue( std::to_string( molecule->getRealResidueCount() ) );
					_nbAtomsLabel->updateWithNewValue( std::to_string( molecule->getRealAtomCount() ) );
				}
			}
		}
		blockSignals( blockSignalState );
	}

	void MultipleMoleculeWidget::_resetFieldStates( const SectionFlag & p_flag )
	{
		if ( bool( p_flag & SectionFlag::TRANSFORM ) )
			_transformWidget->resetState();

		if ( bool( p_flag & SectionFlag::REPRESENTATION ) )
		{
			_representationWidget->resetState();
			_subRepresentationWidget->clearModels();
		}

		if ( bool( p_flag & SectionFlag::TRAJECTORY ) )
		{
			_trajectoryWidget->resetState();
			_trajectorySection->setVisible( false );
		}

		if ( bool( p_flag & SectionFlag::AUTO_ROTATE ) )
		{
			_autoRotateField->resetState();
		}

		if ( bool( p_flag & SectionFlag::INFOS ) )
		{
			_moleculeColor->resetState();
			_fullnameLabel->resetState();
			_nbChainsLabel->resetState();
			_nbResiduesLabel->resetState();
			_nbAtomsLabel->resetState();
		}
	}

	void MultipleMoleculeWidget::localize()
	{
		_transformSection->setHeaderTitle( "Transform" );
		_representationSection->setHeaderTitle( "Representation" );
		_trajectorySection->setHeaderTitle( "Trajectory" );
		_autoRotateSection->setHeaderTitle( "Auto rotation" );
		_infoSection->setHeaderTitle( "Infos" );
		_infoSection->localize();
	}

	void MultipleMoleculeWidget::_onTransformChange(
		const App::Old::Internal::Math::Transform &				  p_transform,
		const App::Old::Internal::Math::TRANSFORM_COMPOSANT_MASK & p_mask ) const
	{
		if ( !signalsBlocked() )
		{
			VTX_ACTION( new App::Old::Action::Transformable::ApplyTransform( getTargets(), p_transform, p_mask ) );
		}
	}

	void MultipleMoleculeWidget::_onPositionDragged( const Vec3f & p_delta ) const
	{
		if ( !signalsBlocked() )
		{
			std::unordered_set<App::Old::Component::Generic::BaseTransformable *> transformableSet
				= std::unordered_set<App::Old::Component::Generic::BaseTransformable *>();
			transformableSet.reserve( getTargets().size() );

			for ( App::Old::Component::Chemistry::Molecule * target : getTargets() )
				transformableSet.emplace( target );

			VTX_ACTION( new App::Old::Action::Transformable::Translate( transformableSet, p_delta ) );
		}
	}
	void MultipleMoleculeWidget::_onRotationDragged( const Vec3f & p_delta ) const
	{
		if ( !signalsBlocked() )
		{
			std::unordered_set<App::Old::Component::Generic::BaseTransformable *> transformableSet
				= std::unordered_set<App::Old::Component::Generic::BaseTransformable *>();
			transformableSet.reserve( getTargets().size() );

			for ( App::Old::Component::Chemistry::Molecule * target : getTargets() )
				transformableSet.emplace( target );

			VTX_ACTION( new App::Old::Action::Transformable::Rotate( transformableSet, p_delta ) );
		}
	}
	void MultipleMoleculeWidget::_onScaleDragged( const Vec3f & p_delta ) const
	{
		if ( !signalsBlocked() )
		{
			std::unordered_set<App::Old::Component::Generic::BaseTransformable *> transformableSet
				= std::unordered_set<App::Old::Component::Generic::BaseTransformable *>();
			transformableSet.reserve( getTargets().size() );

			for ( App::Old::Component::Chemistry::Molecule * target : getTargets() )
				transformableSet.emplace( target );
			VTX_ACTION( new App::Old::Action::Transformable::Scale( transformableSet, p_delta ) );
		}
	}

	void MultipleMoleculeWidget::_onRepresentationPresetChange( const int p_presetIndex ) const
	{
		VTX_ACTION( new App::Old::Action::Molecule::ChangeRepresentationPreset( getTargets(), p_presetIndex ) );
	}
	void MultipleMoleculeWidget::_onRepresentationChange(
		const App::Old::Application::Representation::InstantiatedRepresentation & p_representation,
		const App::Old::Application::Representation::MEMBER_FLAG &				 p_flag ) const
	{
		if ( !signalsBlocked() )
		{
			VTX_ACTION( new App::Old::Action::Molecule::ApplyRepresentation( getTargets(), p_representation, p_flag ) );
		}
	}

	void MultipleMoleculeWidget::_onRepresentationColorChange(
		const App::Old::Application::Representation::InstantiatedRepresentation & p_representation,
		const Util::Color::Rgba &											 p_color,
		const bool															 p_ssColor ) const
	{
		if ( p_ssColor )
		{
			switch ( p_representation.getRibbonData().colorMode )
			{
			case VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM:
				VTX_ACTION( new App::Old::Action::InstantiatedRepresentation::ChangeColor( getTargets(), p_color ) );
				break;

			case VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN:
				VTX_ACTION( new App::Old::Action::Molecule::ChangeColor( getTargets(), p_color ) );
				break;

			case VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE::JMOL:
			case VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE::CHAIN:
			case VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE::RESIDUE: break;

			default:
				VTX_WARNING(
					"SECONDARY_STRUCTURE_COLOR_MODE {} not managed in "
					"MultipleMoleculeWidget::_onRepresentationColorChange.",
					int( p_representation.getRibbonData().colorMode ) );
				break;
			}
		}
		else
		{
			switch ( p_representation.getColorMode() )
			{
			case VTX::Core::ChemDB::Color::COLOR_MODE::ATOM_CUSTOM:
			case VTX::Core::ChemDB::Color::COLOR_MODE::CUSTOM:
				VTX_ACTION( new App::Old::Action::InstantiatedRepresentation::ChangeColor( getTargets(), p_color ) );
				break;

			case VTX::Core::ChemDB::Color::COLOR_MODE::ATOM_PROTEIN:
			case VTX::Core::ChemDB::Color::COLOR_MODE::PROTEIN:
				VTX_ACTION( new App::Old::Action::Molecule::ChangeColor( getTargets(), p_color ) );
				break;

			case VTX::Core::ChemDB::Color::COLOR_MODE::ATOM_CHAIN:
			case VTX::Core::ChemDB::Color::COLOR_MODE::CHAIN:
			case VTX::Core::ChemDB::Color::COLOR_MODE::RESIDUE: break;

			default:
				VTX_WARNING( "COLOR_MODE {} not managed in MultipleMoleculeWidget::_onRepresentationColorChange.",
							 int( p_representation.getColorMode() ) );
				break;
			}
		}
	} // namespace VTX::UI::Widget::Inspector

	void MultipleMoleculeWidget::_onRevertRepresentation() const
	{
		VTX_ACTION( new App::Old::Action::Molecule::RemoveRepresentation( getTargets() ) );
	}
	void MultipleMoleculeWidget::_onApplyRepresentationToChildren() const
	{
		VTX_ACTION( new App::Old::Action::Molecule::RemoveChildrenRepresentations( getTargets() ) );
	}
	void MultipleMoleculeWidget::_onMoleculeColorChange( const Util::Color::Rgba & p_color ) const
	{
		VTX_ACTION( new App::Old::Action::Molecule::ChangeColor( getTargets(), p_color ) );
	}

	void MultipleMoleculeWidget::_setInspectorToChain() const
	{
		UI::VTXApp::get()
			.getMainWindow()
			.getWidget<Inspector::InspectorWidget>( UI::ID::Window::INSPECTOR )
			.forceInspector( Inspector::InspectorWidget::INSPECTOR_TYPE::CHAIN );
	}
	void MultipleMoleculeWidget::_setInspectorToResidue() const
	{
		UI::VTXApp::get()
			.getMainWindow()
			.getWidget<Inspector::InspectorWidget>( UI::ID::Window::INSPECTOR )
			.forceInspector( Inspector::InspectorWidget::INSPECTOR_TYPE::RESIDUE );
	}
	void MultipleMoleculeWidget::_setInspectorToAtom() const
	{
		UI::VTXApp::get()
			.getMainWindow()
			.getWidget<Inspector::InspectorWidget>( UI::ID::Window::INSPECTOR )
			.forceInspector( Inspector::InspectorWidget::INSPECTOR_TYPE::ATOM );
	}

} // namespace VTX::UI::Widget::Inspector
