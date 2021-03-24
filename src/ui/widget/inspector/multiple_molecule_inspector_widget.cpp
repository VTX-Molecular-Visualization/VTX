#include "multiple_molecule_inspector_widget.hpp"
#include "action/action_manager.hpp"
#include "action/instantiated_representation.hpp"
#include "action/molecule.hpp"
#include "action/transformable.hpp"
#include "representation/representation_manager.hpp"
#include "style.hpp"
#include "ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/widget_factory.hpp"
#include "view/ui/widget/inspector/all_inspector_view.hpp"
#include <QBoxLayout>
#include <QFont>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>

namespace VTX::UI::Widget::Inspector
{
	MultipleMoleculeWidget::MultipleMoleculeWidget( QWidget * p_parent ) : InspectorItemWidget( p_parent ) {};

	MultipleMoleculeWidget::~MultipleMoleculeWidget() {}

	void MultipleMoleculeWidget::_setupUi( const QString & p_name )
	{
		InspectorItemWidget::_setupUi( p_name );

		_transformSection
			= VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSection>( this, "inspector_item_section" );
		_transformWidget = VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::TransformWidget>(
			this, "inspector_molecule_transform" );
		_transformSection->setBody( _transformWidget );

		_representationSection
			= VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSection>( this, "inspector_item_section" );
		_representationWidget
			= VTX::UI::WidgetFactory::get().instantiateWidget<Representation::RepresentationInspectorSection>(
				this, "inspector_instantiated_representation" );

		_representationSection->setBody( _representationWidget );

		_infoSection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
			this, "inspector_item_section" );
		_fullnameLabel = new CustomWidget::QLabelMultiField( this );
		_fullnameLabel->setWordWrap( true );
		_infoSection->appendField( "Full Name", _fullnameLabel );

		_nbChainsLabel = new CustomWidget::QLabelMultiField( this );
		_nbChainsLabel->setWordWrap( true );
		_infoSection->appendField( "Nb Chains", _nbChainsLabel );

		_nbResiduesLabel = new CustomWidget::QLabelMultiField( this );
		_nbResiduesLabel->setWordWrap( true );
		_infoSection->appendField( "Nb Residues", _nbResiduesLabel );

		_nbAtomsLabel = new CustomWidget::QLabelMultiField( this );
		_nbAtomsLabel->setWordWrap( true );
		_infoSection->appendField( "Nb Atoms", _nbAtomsLabel );

		_appendSection( _transformSection );
		_appendSection( _representationSection );
		_appendSection( _infoSection );

		const bool oldBlockState = blockSignals( true );
		refresh();
		blockSignals( oldBlockState );
	}

	void MultipleMoleculeWidget::_setupSlots()
	{
		connect( _transformWidget,
				 &CustomWidget::TransformWidget::onValueChange,
				 this,
				 &MultipleMoleculeWidget::_onTransformChange );

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
	};

	void MultipleMoleculeWidget::refresh( const SectionFlag & p_flag )
	{
		if ( _isRefreshFreezed() )
			return;

		_resetFieldStates( p_flag );

		if ( _targets.size() > 0 )
		{
			const QString headerTitle
				= QString::fromStdString( "Molecule (" + std::to_string( _targets.size() ) + ")" );
			_getHeader()->setHeaderTitle( headerTitle );

			const QPixmap * symbolPixmap = Style::IconConst::get().getModelSymbol( ID::Model::MODEL_MOLECULE );
			_getHeader()->setHeaderIcon( *symbolPixmap );

			for ( const Model::Molecule * molecule : _targets )
			{
				if ( bool( p_flag & SectionFlag::TRANSFORM ) )
				{
					_transformWidget->updateWithNewValue( molecule->getTransform() );
				}

				if ( bool( p_flag & SectionFlag::REPRESENTATION ) )
				{
					_representationWidget->updateWithNewValue( *molecule->getRepresentation() );
				}

				if ( bool( p_flag & SectionFlag::INFOS ) )
				{
					if ( !_fullnameLabel->hasDifferentData() )
						_fullnameLabel->updateWithNewValue( molecule->getName() );
					if ( !_nbChainsLabel->hasDifferentData() )
						_nbChainsLabel->updateWithNewValue( std::to_string( molecule->getChainCount() ) );
					if ( !_nbResiduesLabel->hasDifferentData() )
						_nbResiduesLabel->updateWithNewValue( std::to_string( molecule->getResidueCount() ) );
					if ( !_nbAtomsLabel->hasDifferentData() )
						_nbAtomsLabel->updateWithNewValue( std::to_string( molecule->getAtomCount() ) );
				}
			}
		}
	}

	void MultipleMoleculeWidget::_resetFieldStates( const SectionFlag & p_flag )
	{
		if ( bool( p_flag & SectionFlag::TRANSFORM ) )
			_transformWidget->resetState();

		if ( bool( p_flag & SectionFlag::REPRESENTATION ) )
			_representationWidget->resetState();

		if ( bool( p_flag & SectionFlag::INFOS ) )
		{
			_fullnameLabel->resetState();
			_nbChainsLabel->resetState();
			_nbResiduesLabel->resetState();
			_nbAtomsLabel->resetState();
		}
	}

	void MultipleMoleculeWidget::localize()
	{
		_transformSection->setHeaderTitle( "Transform" );
		_representationSection->setHeaderTitle( "Representations" );
		_infoSection->setHeaderTitle( "Infos" );
		_infoSection->localize();
	}

	void MultipleMoleculeWidget::clearTargets()
	{
		for ( Model::Molecule * const molecule : _targets )
		{
			// unlink inspector to prevent target update in view destructor
			MVC::MvcManager::get()
				.getView<View::Inspector::MoleculeInspectorView>( molecule, ID::View::UI_INSPECTOR_MOLECULE_STRUCTURE )
				->setLinkedInspector( nullptr );
			MVC::MvcManager::get().deleteView( molecule, ID::View::UI_INSPECTOR_MOLECULE_STRUCTURE );
		}

		_targets.clear();
	}
	void MultipleMoleculeWidget::addTarget( Model::Molecule * const p_target, const bool p_refresh )
	{
		_targets.emplace( p_target );

		View::Inspector::MoleculeInspectorView * const view
			= MVC::MvcManager::get().instantiateView<View::Inspector::MoleculeInspectorView>(
				p_target, ID::View::UI_INSPECTOR_MOLECULE_STRUCTURE );

		view->setLinkedInspector( this );

		if ( p_refresh )
			refresh();
	}
	void MultipleMoleculeWidget::removeTarget( Model::Molecule * const p_target, const bool p_refresh )
	{
		_targets.erase( p_target );

		if ( MVC::MvcManager::get().hasView( p_target, ID::View::UI_INSPECTOR_MOLECULE_STRUCTURE ) )
			MVC::MvcManager::get().deleteView( p_target, ID::View::UI_INSPECTOR_MOLECULE_STRUCTURE );

		if ( p_refresh )
			refresh();
	}

	void MultipleMoleculeWidget::_onTransformChange( const Math::Transform & p_transform ) const
	{
		if ( !signalsBlocked() )
		{
			VTX_ACTION( new Action::Transformable::ApplyTransform( _targets, p_transform ) );
		}
	}
	void MultipleMoleculeWidget::_onRepresentationPresetChange( const int p_presetIndex ) const
	{
		VTX_ACTION( new Action::Molecule::ChangeRepresentationPreset( _targets, p_presetIndex ) );
	}
	void MultipleMoleculeWidget::_onRepresentationChange(
		const Model::Representation::InstantiatedRepresentation & p_representation,
		const Model::Representation::MEMBER_FLAG &				  p_flag ) const
	{
		if ( !signalsBlocked() )
		{
			VTX_ACTION( new Action::Molecule::ApplyRepresentation( _targets, p_representation, p_flag ) );
		}
	}

	void MultipleMoleculeWidget::_onRepresentationColorChange(
		const Model::Representation::InstantiatedRepresentation & p_representation,
		const Color::Rgb &										  p_color,
		const bool												  p_ssColor ) const
	{
		if ( p_ssColor )
		{
			switch ( p_representation.getSecondaryStructureColorMode() )
			{
			case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM:
				VTX_ACTION( new Action::InstantiatedRepresentation::ChangeColor( _targets, p_color ) );
				break;

			case Generic::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN:
				VTX_ACTION( new Action::Molecule::ChangeColor( _targets, p_color ) );
				break;

			case Generic::SECONDARY_STRUCTURE_COLOR_MODE::JMOL:
			case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CHAIN:
			case Generic::SECONDARY_STRUCTURE_COLOR_MODE::RESIDUE: break;

			default:
				VTX_WARNING( "SECONDARY_STRUCTURE_COLOR_MODE "
							 + std::to_string( int( p_representation.getSecondaryStructureColorMode() ) )
							 + " not managed in MultipleMoleculeWidget::_onRepresentationColorChange." );
				break;
			}
		}
		else
		{
			switch ( p_representation.getColorMode() )
			{
			case Generic::COLOR_MODE::ATOM_CUSTOM:
			case Generic::COLOR_MODE::CUSTOM:
				VTX_ACTION( new Action::InstantiatedRepresentation::ChangeColor( _targets, p_color ) );
				break;

			case Generic::COLOR_MODE::ATOM_PROTEIN:
			case Generic::COLOR_MODE::PROTEIN:
				VTX_ACTION( new Action::Molecule::ChangeColor( _targets, p_color ) );
				break;

			case Generic::COLOR_MODE::ATOM_CHAIN:
			case Generic::COLOR_MODE::CHAIN:
			case Generic::COLOR_MODE::RESIDUE: break;

			default:
				VTX_WARNING( "COLOR_MODE " + std::to_string( int( p_representation.getColorMode() ) )
							 + " not managed in MultipleMoleculeWidget::_onRepresentationColorChange." );
				break;
			}
		}
	} // namespace VTX::UI::Widget::Inspector

	void MultipleMoleculeWidget::_onRevertRepresentation() const
	{
		VTX_ACTION( new Action::Molecule::RemoveRepresentation( _targets ) );
	}

} // namespace VTX::UI::Widget::Inspector
