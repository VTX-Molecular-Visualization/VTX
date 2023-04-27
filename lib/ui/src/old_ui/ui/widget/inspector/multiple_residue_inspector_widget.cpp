#include "ui/old_ui/ui/widget/inspector/multiple_residue_inspector_widget.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/ui/layout/attribute_list_layout.hpp"
#include "ui/old_ui/ui/main_window.hpp"
#include "ui/old_ui/ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/old_ui/ui/widget/inspector/inspector_widget.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/util/ui.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <QBoxLayout>
#include <QFont>
#include <QGridLayout>
#include <QPixmap>
#include <app/action/instantiated_representation.hpp>
#include <app/action/molecule.hpp>
#include <app/action/residue.hpp>
#include <app/action/transformable.hpp>
#include <app/application/representation/representation_manager.hpp>
#include <app/component/chemistry/molecule.hpp>

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

		_infoSection
			= VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSection>( this, "inspector_item_section" );

		QWidget * const infoSectionWidget = new QWidget( this );
		infoSectionWidget->setContentsMargins( 0, 0, 0, 0 );

		_infoSectionLayout = new Layout::AttributeListLayout( infoSectionWidget );

		_fullnameLabel = new QLabel( this );
		_fullnameLabel->setWordWrap( true );
		_infoSectionLayout->addAttribute( _fullnameLabel, "Full Name" );

		_atomsCountLabel = new QLabel( this );
		_atomsCountLabel->setWordWrap( true );
		_infoSectionLayout->addAttribute( _atomsCountLabel, "Atoms count" );

		_atomsSumLabel = new QLabel( this );
		_atomsSumLabel->setWordWrap( true );
		_infoSectionLayout->addAttribute( _atomsSumLabel, "Atoms sum" );

		_bondsLabel = new QLabel( this );
		_bondsLabel->setWordWrap( true );
		_infoSectionLayout->addAttribute( _bondsLabel, "Bonds" );

		_infoSection->setBody( infoSectionWidget );

		_appendSection( _representationSection );
		_appendSection( _infoSection );

		const bool oldBlockState = blockSignals( true );
		refresh();
		blockSignals( oldBlockState );
	}

	void MultipleResidueWidget::_setupSlots()
	{
		QMenu * const headerMenu = new QMenu( this );

		QAction * inspectorToMoleculeAction = new QAction( "Molecule", this );
		connect(
			inspectorToMoleculeAction, &QAction::triggered, this, &MultipleResidueWidget::_setInspectorToMolecule );
		headerMenu->addAction( inspectorToMoleculeAction );

		QAction * inspectorToChainAction = new QAction( "Chain", this );
		connect( inspectorToChainAction, &QAction::triggered, this, &MultipleResidueWidget::_setInspectorToChain );
		headerMenu->addAction( inspectorToChainAction );

		QAction * inspectorToAtomAction = new QAction( "Atom", this );
		connect( inspectorToAtomAction, &QAction::triggered, this, &MultipleResidueWidget::_setInspectorToAtom );
		headerMenu->addAction( inspectorToAtomAction );

		_getHeader()->setMenu( headerMenu );

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

		const std::unordered_set<App::Component::Chemistry::Residue *> & targets = getTargets();

		if ( targets.size() > 0 )
		{
			const QString headerTitle = QString::fromStdString( "Residue (" + std::to_string( targets.size() ) + ")" );
			_getHeader()->setHeaderTitle( headerTitle );

			const QPixmap * symbolPixmap = Style::IconConst::get().getModelSymbol( VTX::ID::Model::MODEL_RESIDUE );
			_getHeader()->setHeaderIcon( *symbolPixmap );

			int		currentResidueDisplayedCount = 0;
			uint	atomSum						 = 0;
			QString atomCountText				 = QString();
			QString fullNameText				 = QString();

			for ( const App::Component::Chemistry::Residue * residue : targets )
			{
				if ( bool( p_flag & SectionFlag::REPRESENTATION ) )
				{
					_representationWidget->updateWithNewValue( *residue->getRepresentation() );
				}

				if ( bool( p_flag & SectionFlag::INFOS ) )
				{
					// Count atom sum
					const uint atomCount = residue->getRealAtomCount();
					atomSum += atomCount;

					if ( currentResidueDisplayedCount < Style::MULTIPLE_INSPECTOR_INFO_DATA_COUNT_DISPLAYED )
					{
						if ( currentResidueDisplayedCount > 0 )
						{
							fullNameText.append( " ; " );
							atomCountText.append( " ; " );
						}

						fullNameText.append( QString::fromStdString( residue->getSymbolName() ) );
						atomCountText.append( QString::fromStdString( std::to_string( atomCount ) ) );
					}
					else if ( currentResidueDisplayedCount == Style::MULTIPLE_INSPECTOR_INFO_DATA_COUNT_DISPLAYED )
					{
						fullNameText.append( "..." );
						atomCountText.append( "..." );
					}

					_appendBondInfo( *residue );
				}

				currentResidueDisplayedCount++;
			}

			if ( bool( p_flag & SectionFlag::REPRESENTATION ) )
			{
				_infoSectionLayout->setAttributeVisibility( _atomsSumLabel, currentResidueDisplayedCount > 1 );

				_fullnameLabel->setText( fullNameText );
				_atomsCountLabel->setText( atomCountText );
				_atomsSumLabel->setText( QString::fromStdString( std::to_string( atomSum ) ) );
			}
		}
	}

	void MultipleResidueWidget::_resetFieldStates( const SectionFlag & p_flag )
	{
		if ( bool( p_flag & SectionFlag::REPRESENTATION ) )
			_representationWidget->resetState();

		if ( bool( p_flag & SectionFlag::INFOS ) )
		{
			_fullnameLabel->setText( "" );
			_atomsCountLabel->setText( "" );
			_atomsSumLabel->setText( "" );
			_bondsLabel->setText( "" );
			_bondInfoCount = 0;
		}
	}

	void MultipleResidueWidget::localize()
	{
		_representationSection->setHeaderTitle( "Representation" );
		_infoSection->setHeaderTitle( "Infos" );
		_infoSection->localize();
	}

	void MultipleResidueWidget::_onRepresentationPresetChange( const int p_presetIndex )
	{
		VTX_ACTION( new App::Action::Residue::ChangeRepresentationPreset( getTargets(), p_presetIndex ) );
	}
	void MultipleResidueWidget::_onRepresentationChange(
		const App::Application::Representation::InstantiatedRepresentation & p_representation,
		const App::Application::Representation::MEMBER_FLAG &				 p_flag )
	{
		if ( !signalsBlocked() )
		{
			VTX_ACTION( new App::Action::Residue::ApplyRepresentation( getTargets(), p_representation, p_flag ) );
		}
	}

	void MultipleResidueWidget::_onRepresentationColorChange(
		const App::Application::Representation::InstantiatedRepresentation & p_representation,
		const Color::Rgba &													 p_color,
		const bool															 p_ssColor )
	{
		if ( !signalsBlocked() )
		{
			if ( p_ssColor )
			{
				switch ( p_representation.getRibbonData().colorMode )
				{
				case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM:
					VTX_ACTION( new App::Action::InstantiatedRepresentation::ChangeColor( getTargets(), p_color ) );
					break;

				case Generic::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN: _changeMoleculesColor( p_color ); break;
				case Generic::SECONDARY_STRUCTURE_COLOR_MODE::JMOL:
				case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CHAIN:
				case Generic::SECONDARY_STRUCTURE_COLOR_MODE::RESIDUE: break;

				default:
					VTX_WARNING( "SECONDARY_STRUCTURE_COLOR_MODE "
								 + std::to_string( int( p_representation.getRibbonData().colorMode ) )
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
					VTX_ACTION( new App::Action::InstantiatedRepresentation::ChangeColor( getTargets(), p_color ) );
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

	void MultipleResidueWidget::_changeMoleculesColor( const Color::Rgba & p_color ) const
	{
		std::unordered_set<App::Component::Chemistry::Molecule *> molecules
			= std::unordered_set<App::Component::Chemistry::Molecule *>();

		for ( const App::Component::Chemistry::Residue * const item : getTargets() )
		{
			molecules.emplace( item->getMoleculePtr() );
		}

		VTX_ACTION( new App::Action::Molecule::ChangeColor( molecules, p_color ) );
	}
	void MultipleResidueWidget::_onRevertRepresentation() const
	{
		VTX_ACTION( new App::Action::Residue::RemoveRepresentation( getTargets() ) );
	}

	void MultipleResidueWidget::_appendBondInfo( const App::Component::Chemistry::Residue & p_residue )
	{
		if ( _bondInfoCount >= Style::INSPECTOR_INFO_BOND_COUNT_DISPLAYED )
			return;

		QString											  bondInfoStr = _bondsLabel->text();
		const App::Component::Chemistry::Molecule * const moleculePtr = p_residue.getMoleculePtr();
		for ( uint i = p_residue.getIndexFirstBond(); i < p_residue.getIndexFirstBond() + p_residue.getBondCount();
			  i++ )
		{
			const App::Component::Chemistry::Bond * const bond = moleculePtr->getBond( i );

			if ( bond == nullptr )
				continue;

			Util::UI::appendBondInfo( *bond, bondInfoStr );
			_bondInfoCount++;

			if ( _bondInfoCount >= Style::INSPECTOR_INFO_BOND_COUNT_DISPLAYED )
			{
				bondInfoStr.append( "\n..." );
				break;
			}
		}

		_bondsLabel->setText( bondInfoStr );
	}

	void MultipleResidueWidget::_setInspectorToMolecule() const
	{
		VTXApp::get()
			.getMainWindow()
			.getWidget<Inspector::InspectorWidget>( ID::UI::Window::INSPECTOR )
			.forceInspector( Inspector::InspectorWidget::INSPECTOR_TYPE::MOLECULE );
	}
	void MultipleResidueWidget::_setInspectorToChain() const
	{
		VTXApp::get()
			.getMainWindow()
			.getWidget<Inspector::InspectorWidget>( ID::UI::Window::INSPECTOR )
			.forceInspector( Inspector::InspectorWidget::INSPECTOR_TYPE::CHAIN );
	}
	void MultipleResidueWidget::_setInspectorToAtom() const
	{
		VTXApp::get()
			.getMainWindow()
			.getWidget<Inspector::InspectorWidget>( ID::UI::Window::INSPECTOR )
			.forceInspector( Inspector::InspectorWidget::INSPECTOR_TYPE::ATOM );
	}

} // namespace VTX::UI::Widget::Inspector
