#include "multiple_residue_inspector_widget.hpp"
#include "action/action_manager.hpp"
#include "action/instantiated_representation.hpp"
#include "action/molecule.hpp"
#include "action/residue.hpp"
#include "action/transformable.hpp"
#include "model/molecule.hpp"
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
	MultipleResidueWidget::MultipleResidueWidget( QWidget * p_parent ) : InspectorItemWidget( p_parent ) {};

	MultipleResidueWidget::~MultipleResidueWidget() {}

	void MultipleResidueWidget::_setupUi( const QString & p_name )
	{
		InspectorItemWidget::_setupUi( p_name );

		_representationSection
			= VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSection>( this, "inspector_item_section" );
		_representationWidget
			= VTX::UI::WidgetFactory::get().instantiateWidget<Representation::RepresentationInspectorSection>(
				this, "inspector_instantiated_representation" );

		_representationWidget->setActionButtonVisibility(
			Representation::RepresentationInspectorSection::ActionButtons::Revert );

		_representationSection->setBody( _representationWidget );

		_infoSection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
			this, "inspector_item_section" );
		_fullnameLabel = new CustomWidget::QLabelMultiField( this );
		_fullnameLabel->setWordWrap( true );
		_infoSection->appendField( "Full Name", _fullnameLabel );

		_nbAtomsLabel = new CustomWidget::QLabelMultiField( this );
		_nbAtomsLabel->setWordWrap( true );
		_infoSection->appendField( "Nb Atoms", _nbAtomsLabel );

		_appendSection( _representationSection );
		_appendSection( _infoSection );

		const bool oldBlockState = blockSignals( true );
		refresh();
		blockSignals( oldBlockState );
	}

	void MultipleResidueWidget::_setupSlots()
	{
		connect( _representationWidget,
				 &Representation::RepresentationInspectorSection::onRepresentationPresetChange,
				 this,
				 &MultipleResidueWidget::_onRepresentationPresetChange );

		connect( _representationWidget,
				 &Representation::RepresentationInspectorSection::onRepresentationChange,
				 this,
				 &MultipleResidueWidget::_onRepresentationChange );

		connect( _representationWidget,
				 &Representation::RepresentationInspectorSection::onRepresentationColorChange,
				 this,
				 &MultipleResidueWidget::_onRepresentationColorChange );

		connect( _representationWidget,
				 &Representation::RepresentationInspectorSection::onRevertRepresentation,
				 this,
				 &MultipleResidueWidget::_onRevertRepresentation );
	};

	void MultipleResidueWidget::refresh( const SectionFlag & p_flag )
	{
		if ( _isRefreshFreezed() )
			return;

		_resetFieldStates( p_flag );

		if ( _targets.size() > 0 )
		{
			const QString headerTitle = QString::fromStdString( "Residue (" + std::to_string( _targets.size() ) + ")" );
			_getHeader()->setHeaderTitle( headerTitle );

			const QPixmap * symbolPixmap = Style::IconConst::get().getModelSymbol( ID::Model::MODEL_RESIDUE );
			_getHeader()->setHeaderIcon( *symbolPixmap );

			for ( const Model::Residue * residue : _targets )
			{
				if ( bool( p_flag & SectionFlag::REPRESENTATION ) )
				{
					_representationWidget->updateWithNewValue( *residue->getRepresentation() );
				}

				if ( bool( p_flag & SectionFlag::INFOS ) )
				{
					if ( !_fullnameLabel->hasDifferentData() )
						_fullnameLabel->updateWithNewValue( residue->getSymbolName() );
					if ( !_nbAtomsLabel->hasDifferentData() )
						_nbAtomsLabel->updateWithNewValue( std::to_string( residue->getAtomCount() ) );
				}
			}
		}
	}

	void MultipleResidueWidget::_resetFieldStates( const SectionFlag & p_flag )
	{
		if ( bool( p_flag & SectionFlag::REPRESENTATION ) )
			_representationWidget->resetState();

		if ( bool( p_flag & SectionFlag::INFOS ) )
		{
			_fullnameLabel->resetState();
			_nbAtomsLabel->resetState();
		}
	}

	void MultipleResidueWidget::localize()
	{
		_representationSection->setHeaderTitle( "Representations" );
		_infoSection->setHeaderTitle( "Infos" );
		_infoSection->localize();
	}

	void MultipleResidueWidget::clearTargets()
	{
		for ( Model::Residue * const residue : _targets )
		{
			// unlink inspector to prevent target update in view destructor
			MVC::MvcManager::get()
				.getView<View::Inspector::ResidueInspectorView>( residue, ID::View::UI_INSPECTOR_RESIDUE )
				->setLinkedInspector( nullptr );
			MVC::MvcManager::get().deleteView( residue, ID::View::UI_INSPECTOR_RESIDUE );
		}

		_targets.clear();
	}
	void MultipleResidueWidget::addTarget( Model::Residue * const p_target, const bool p_refresh )
	{
		_targets.emplace( p_target );

		View::Inspector::ResidueInspectorView * const view
			= MVC::MvcManager::get().instantiateView<View::Inspector::ResidueInspectorView>(
				p_target, ID::View::UI_INSPECTOR_RESIDUE );

		view->setLinkedInspector( this );

		if ( p_refresh )
			refresh();
	}
	void MultipleResidueWidget::removeTarget( Model::Residue * const p_target, const bool p_refresh )
	{
		_targets.erase( p_target );

		if ( MVC::MvcManager::get().hasView( p_target, ID::View::UI_INSPECTOR_RESIDUE ) )
			MVC::MvcManager::get().deleteView( p_target, ID::View::UI_INSPECTOR_RESIDUE );

		if ( p_refresh )
			refresh();
	}
	void MultipleResidueWidget::_onRepresentationPresetChange( const int p_presetIndex )
	{
		_freezeRefresh( true );

		VTX_ACTION( new Action::Residue::ChangeRepresentationPreset( _targets, p_presetIndex ) );

		_freezeRefresh( false );
		refresh();
	}
	void MultipleResidueWidget::_onRepresentationChange(
		const Model::Representation::InstantiatedRepresentation & p_representation,
		const Model::Representation::MEMBER_FLAG &				  p_flag )
	{
		if ( !signalsBlocked() )
		{
			_freezeRefresh( true );
			VTX_ACTION( new Action::Residue::ApplyRepresentation( _targets, p_representation, p_flag ) );
			_freezeRefresh( false );
			// refresh();
		}
	}

	void MultipleResidueWidget::_onRepresentationColorChange(
		const Model::Representation::InstantiatedRepresentation & p_representation,
		const Color::Rgb &										  p_color,
		const bool												  p_ssColor )
	{
		if ( !signalsBlocked() )
		{
			if ( p_ssColor )
			{
				switch ( p_representation.getSecondaryStructureColorMode() )
				{
				case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM:
					VTX_ACTION( new Action::InstantiatedRepresentation::ChangeColor( _targets, p_color ) );
					break;

				case Generic::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN: _changeMoleculesColor( p_color ); break;
				case Generic::SECONDARY_STRUCTURE_COLOR_MODE::JMOL:
				case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CHAIN:
				case Generic::SECONDARY_STRUCTURE_COLOR_MODE::RESIDUE: break;

				default:
					VTX_WARNING( "SECONDARY_STRUCTURE_COLOR_MODE "
								 + std::to_string( int( p_representation.getSecondaryStructureColorMode() ) )
								 + " not managed in MultipleResidueWidget::_onRepresentationColorChange." );
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
				case Generic::COLOR_MODE::PROTEIN: _changeMoleculesColor( p_color ); break;

				case Generic::COLOR_MODE::ATOM_CHAIN:
				case Generic::COLOR_MODE::CHAIN:
				case Generic::COLOR_MODE::RESIDUE: break;

				default:
					VTX_WARNING( "COLOR_MODE " + std::to_string( int( p_representation.getColorMode() ) )
								 + " not managed in MultipleResidueWidget::_onRepresentationColorChange." );
					break;
				}
			}
		}
	}

	void MultipleResidueWidget::_changeMoleculesColor( const Color::Rgb & p_color ) const
	{
		std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

		for ( const Model::Residue * const item : _targets )
		{
			molecules.emplace( item->getMoleculePtr() );
		}

		VTX_ACTION( new Action::Molecule::ChangeColor( molecules, p_color ) );
	}
	void MultipleResidueWidget::_onRevertRepresentation() const
	{
		VTX_ACTION( new Action::Residue::RemoveRepresentation( _targets ) );
	}

} // namespace VTX::UI::Widget::Inspector
