#include "multiple_chain_inspector_widget.hpp"
#include "action/action_manager.hpp"
#include "action/chain.hpp"
#include "action/instantiated_representation.hpp"
#include "action/molecule.hpp"
#include "model/molecule.hpp"
#include "representation/representation_manager.hpp"
#include "style.hpp"
#include "ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/widget_factory.hpp"
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>

namespace VTX::UI::Widget::Inspector
{
	MultipleChainWidget::MultipleChainWidget( QWidget * p_parent ) :
		MultipleModelInspectorWidget( p_parent, ID::View::UI_INSPECTOR_CHAIN ) {};

	MultipleChainWidget::~MultipleChainWidget() {}

	void MultipleChainWidget::_setupUi( const QString & p_name )
	{
		MultipleModelInspectorWidget::_setupUi( p_name );

		_representationSection
			= VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSection>( this, "inspector_item_section" );
		_representationWidget
			= VTX::UI::WidgetFactory::get().instantiateWidget<Representation::RepresentationInspectorSection>(
				this, "inspector_instantiated_representation" );
		_representationWidget->setActionButtonVisibility(
			Representation::RepresentationInspectorSection::ActionButtons::All );

		_representationSection->setBody( _representationWidget );

		_infoSection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
			this, "inspector_item_section" );
		_fullnameLabel = new CustomWidget::QLabelMultiField( this );
		_fullnameLabel->setWordWrap( true );
		_infoSection->appendField( "Full Name", _fullnameLabel );

		_nbResiduesLabel = new CustomWidget::QLabelMultiField( this );
		_nbResiduesLabel->setWordWrap( true );
		_infoSection->appendField( "Nb Residues", _nbResiduesLabel );

		_appendSection( _representationSection );
		_appendSection( _infoSection );

		const bool oldBlockState = blockSignals( true );
		refresh();
		blockSignals( oldBlockState );
	}

	void MultipleChainWidget::_setupSlots()
	{
		connect( _representationWidget,
				 &Representation::RepresentationInspectorSection::onRepresentationPresetChange,
				 this,
				 &MultipleChainWidget::_onRepresentationPresetChange );

		connect( _representationWidget,
				 &Representation::RepresentationInspectorSection::onRepresentationChange,
				 this,
				 &MultipleChainWidget::_onRepresentationChange );

		connect( _representationWidget,
				 &Representation::RepresentationInspectorSection::onRepresentationColorChange,
				 this,
				 &MultipleChainWidget::_onRepresentationColorChange );

		connect( _representationWidget,
				 &Representation::RepresentationInspectorSection::onRevertRepresentation,
				 this,
				 &MultipleChainWidget::_onRevertRepresentation );

		connect( _representationWidget,
				 &Representation::RepresentationInspectorSection::onApplyRepresentationToChildren,
				 this,
				 &MultipleChainWidget::_onApplyRepresentationToChildren );
	};

	void MultipleChainWidget::_endOfFrameRefresh( const SectionFlag & p_flag )
	{
		MultipleModelInspectorWidget::_endOfFrameRefresh( p_flag );

		bool blockSignalState = blockSignals( true );
		_resetFieldStates( p_flag );

		const std::unordered_set<Model::Chain *> & targets = getTargets();

		if ( targets.size() > 0 )
		{
			const QString headerTitle = QString::fromStdString( "Chain (" + std::to_string( targets.size() ) + ")" );
			_getHeader()->setHeaderTitle( headerTitle );

			const QPixmap * symbolPixmap = Style::IconConst::get().getModelSymbol( VTX::ID::Model::MODEL_CHAIN );
			_getHeader()->setHeaderIcon( *symbolPixmap );

			for ( const Model::Chain * chain : targets )
			{
				if ( bool( p_flag & SectionFlag::REPRESENTATION ) )
				{
					_representationWidget->updateWithNewValue( *chain->getRepresentation() );
				}

				if ( bool( p_flag & SectionFlag::INFOS ) )
				{
					if ( !_fullnameLabel->hasDifferentData() )
						_fullnameLabel->updateWithNewValue( chain->getName() );
					if ( !_nbResiduesLabel->hasDifferentData() )
						_nbResiduesLabel->updateWithNewValue( std::to_string( chain->getRealResidueCount() ) );
				}
			}
		}
		blockSignals( blockSignalState );
	}

	void MultipleChainWidget::_resetFieldStates( const SectionFlag & p_flag )
	{
		if ( bool( p_flag & SectionFlag::REPRESENTATION ) )
			_representationWidget->resetState();

		if ( bool( p_flag & SectionFlag::INFOS ) )
		{
			_fullnameLabel->resetState();
			_nbResiduesLabel->resetState();
		}
	}

	void MultipleChainWidget::localize()
	{
		_representationSection->setHeaderTitle( "Representation" );
		_infoSection->setHeaderTitle( "Infos" );
		_infoSection->localize();
	}

	void MultipleChainWidget::_onRepresentationPresetChange( const int p_presetIndex )
	{
		VTX_ACTION( new Action::Chain::ChangeRepresentationPreset( getTargets(), p_presetIndex ) );
	}
	void MultipleChainWidget::_onRepresentationChange(
		const Model::Representation::InstantiatedRepresentation & p_representation,
		const Model::Representation::MEMBER_FLAG &				  p_flag )
	{
		if ( !signalsBlocked() )
		{
			VTX_ACTION( new Action::Chain::ApplyRepresentation( getTargets(), p_representation, p_flag ) );
		}
	}

	void MultipleChainWidget::_onRepresentationColorChange(
		const Model::Representation::InstantiatedRepresentation & p_representation,
		const Color::Rgb &										  p_color,
		const bool												  p_ssColor )
	{
		if ( !signalsBlocked() )
		{
			if ( p_ssColor )
			{
				switch ( p_representation.getRibbonData().colorMode )
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
								 + std::to_string( int( p_representation.getRibbonData().colorMode ) )
								 + " not managed in MultipleChainWidget::_onRepresentationColorChange." );
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
								 + " not managed in MultipleChainWidget::_onRepresentationColorChange." );
					break;
				}
			}
		}
	}

	void MultipleChainWidget::_changeMoleculesColor( const Color::Rgb & p_color ) const
	{
		std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

		for ( const Model::Chain * const item : getTargets() )
		{
			molecules.emplace( item->getMoleculePtr() );
		}

		VTX_ACTION( new Action::Molecule::ChangeColor( molecules, p_color ) );
	}

	void MultipleChainWidget::_onRevertRepresentation() const
	{
		VTX_ACTION( new Action::Chain::RemoveRepresentation( getTargets() ) );
	}
	void MultipleChainWidget::_onApplyRepresentationToChildren() const
	{
		VTX_ACTION( new Action::Chain::RemoveChildrenRepresentations( getTargets() ) );
	}

} // namespace VTX::UI::Widget::Inspector
