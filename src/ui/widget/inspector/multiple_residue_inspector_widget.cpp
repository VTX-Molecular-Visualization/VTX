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
#include "util/ui.hpp"
#include <QBoxLayout>
#include <QFont>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>

namespace VTX::UI::Widget::Inspector
{
	MultipleResidueWidget::MultipleResidueWidget( QWidget * p_parent ) :
		MultipleModelInspectorWidget( p_parent, ID::View::UI_INSPECTOR_RESIDUE ) {};

	MultipleResidueWidget::~MultipleResidueWidget() {}

	void MultipleResidueWidget::_setupUi( const QString & p_name )
	{
		MultipleModelInspectorWidget::_setupUi( p_name );

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

		_bondsLabel = new QLabel( this );
		_bondsLabel->setWordWrap( true );
		_infoSection->appendField( "Bonds", _bondsLabel );

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

	void MultipleResidueWidget::_endOfFrameRefresh( const SectionFlag & p_flag )
	{
		MultipleModelInspectorWidget::_endOfFrameRefresh();

		_resetFieldStates( p_flag );

		const std::unordered_set<Model::Residue *> & targets = getTargets();

		if ( targets.size() > 0 )
		{
			const QString headerTitle = QString::fromStdString( "Residue (" + std::to_string( targets.size() ) + ")" );
			_getHeader()->setHeaderTitle( headerTitle );

			const QPixmap * symbolPixmap = Style::IconConst::get().getModelSymbol( VTX::ID::Model::MODEL_RESIDUE );
			_getHeader()->setHeaderIcon( *symbolPixmap );

			for ( const Model::Residue * residue : targets )
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
						_nbAtomsLabel->updateWithNewValue( std::to_string( residue->getRealAtomCount() ) );

					_appendBondInfo( *residue );
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
			_bondsLabel->setText( "" );
		}
	}

	void MultipleResidueWidget::localize()
	{
		_representationSection->setHeaderTitle( "Representations" );
		_infoSection->setHeaderTitle( "Infos" );
		_infoSection->localize();
	}

	void MultipleResidueWidget::_onRepresentationPresetChange( const int p_presetIndex )
	{
		VTX_ACTION( new Action::Residue::ChangeRepresentationPreset( getTargets(), p_presetIndex ) );
	}
	void MultipleResidueWidget::_onRepresentationChange(
		const Model::Representation::InstantiatedRepresentation & p_representation,
		const Model::Representation::MEMBER_FLAG &				  p_flag )
	{
		if ( !signalsBlocked() )
		{
			VTX_ACTION( new Action::Residue::ApplyRepresentation( getTargets(), p_representation, p_flag ) );
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
				switch ( p_representation.getRibbonData()._colorMode )
				{
				case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM:
					VTX_ACTION( new Action::InstantiatedRepresentation::ChangeColor( getTargets(), p_color ) );
					break;

				case Generic::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN: _changeMoleculesColor( p_color ); break;
				case Generic::SECONDARY_STRUCTURE_COLOR_MODE::JMOL:
				case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CHAIN:
				case Generic::SECONDARY_STRUCTURE_COLOR_MODE::RESIDUE: break;

				default:
					VTX_WARNING( "SECONDARY_STRUCTURE_COLOR_MODE "
								 + std::to_string( int( p_representation.getRibbonData()._colorMode ) )
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
					VTX_ACTION( new Action::InstantiatedRepresentation::ChangeColor( getTargets(), p_color ) );
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

		for ( const Model::Residue * const item : getTargets() )
		{
			molecules.emplace( item->getMoleculePtr() );
		}

		VTX_ACTION( new Action::Molecule::ChangeColor( molecules, p_color ) );
	}
	void MultipleResidueWidget::_onRevertRepresentation() const
	{
		VTX_ACTION( new Action::Residue::RemoveRepresentation( getTargets() ) );
	}

	void MultipleResidueWidget::_appendBondInfo( const Model::Residue & p_residue )
	{
		QString						  bondInfoStr = _bondsLabel->text();
		const Model::Molecule * const moleculePtr = p_residue.getMoleculePtr();
		for ( uint i = p_residue.getIndexFirstBond(); i < p_residue.getIndexFirstBond() + p_residue.getBondCount();
			  i++ )
		{
			const Model::Bond * const bond = moleculePtr->getBond( i );
			if ( bond == nullptr )
				continue;

			Util::UI::appendBondInfo( *bond, bondInfoStr );
		}

		_bondsLabel->setText( bondInfoStr );
	}

} // namespace VTX::UI::Widget::Inspector
