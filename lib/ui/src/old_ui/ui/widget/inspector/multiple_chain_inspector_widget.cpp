#include "ui/old_ui/ui/widget/inspector/multiple_chain_inspector_widget.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/ui/main_window.hpp"
#include "ui/old_ui/ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/folding_button.hpp"
#include "ui/old_ui/ui/widget/inspector/inspector_widget.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <app/action/chain.hpp>
#include <app/action/instantiated_representation.hpp>
#include <app/action/molecule.hpp>
#include <app/application/representation/representation_manager.hpp>
#include <app/component/chemistry/molecule.hpp>

namespace VTX::UI::Widget::Inspector
{
	MultipleChainWidget::MultipleChainWidget( QWidget * p_parent ) :
		MultipleModelInspectorWidget( p_parent, ID::View::UI_INSPECTOR_CHAIN ) {};

	MultipleChainWidget::~MultipleChainWidget() {}

	void MultipleChainWidget::_setupUi( const QString & p_name )
	{
		MultipleModelInspectorWidget::_setupUi( p_name );

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

		_representationSection->appendField( _representationWidget );
		_representationSection->appendField( _subRepresentationFoldingButton );

		_infoSection
			= VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSection>( this, "inspector_item_section" );

		QWidget * const infoSectionWidget = new QWidget( this );
		infoSectionWidget->setContentsMargins( 0, 0, 0, 0 );
		_infoSectionlayout = new Layout::AttributeListLayout( infoSectionWidget );

		_fullnameLabel = new QLabel( this );
		_fullnameLabel->setWordWrap( true );
		_infoSectionlayout->addAttribute( _fullnameLabel, "Full Name" );

		_residuesCountLabel = new QLabel( this );
		_residuesCountLabel->setWordWrap( true );
		_infoSectionlayout->addAttribute( _residuesCountLabel, "Residues count" );

		_residuesSumLabel = new QLabel( this );
		_residuesSumLabel->setWordWrap( true );
		_infoSectionlayout->addAttribute( _residuesSumLabel, "Residues sum" );

		_atomsCountLabel = new QLabel( this );
		_atomsCountLabel->setWordWrap( true );
		_infoSectionlayout->addAttribute( _atomsCountLabel, "Atoms count" );

		_atomsSumLabel = new QLabel( this );
		_atomsSumLabel->setWordWrap( true );
		_infoSectionlayout->addAttribute( _atomsSumLabel, "Atoms sum" );

		_indexLabel = new QLabel( this );
		_indexLabel->setWordWrap( true );
		_infoSectionlayout->addAttribute( _indexLabel, "Index" );

		_infoSection->setBody( infoSectionWidget );

		_appendSection( _representationSection );
		_appendSection( _infoSection );

		const bool oldBlockState = blockSignals( true );
		refresh();
		blockSignals( oldBlockState );
	}

	void MultipleChainWidget::_setupSlots()
	{
		QMenu * const headerMenu = new QMenu( this );

		QAction * inspectorToMoleculeAction = new QAction( "Molecule", this );
		connect( inspectorToMoleculeAction, &QAction::triggered, this, &MultipleChainWidget::_setInspectorToMolecule );
		headerMenu->addAction( inspectorToMoleculeAction );

		QAction * inspectorToResidueAction = new QAction( "Residue", this );
		connect( inspectorToResidueAction, &QAction::triggered, this, &MultipleChainWidget::_setInspectorToResidue );
		headerMenu->addAction( inspectorToResidueAction );

		QAction * inspectorToAtomAction = new QAction( "Atom", this );
		connect( inspectorToAtomAction, &QAction::triggered, this, &MultipleChainWidget::_setInspectorToAtom );
		headerMenu->addAction( inspectorToAtomAction );

		_getHeader()->setMenu( headerMenu );

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

		const std::unordered_set<App::Component::Chemistry::Chain *> & targets = getTargets();

		if ( targets.size() > 0 )
		{
			const QString headerTitle = QString::fromStdString( "Chain (" + std::to_string( targets.size() ) + ")" );
			_getHeader()->setHeaderTitle( headerTitle );

			const QPixmap * symbolPixmap = Style::IconConst::get().getModelSymbol( App::ID::Model::MODEL_CHAIN );
			_getHeader()->setHeaderIcon( *symbolPixmap );

			int		currentChainDisplayedCount = 0;
			uint	residueSum				   = 0;
			uint	atomSum					   = 0;
			QString fullNameText			   = QString();
			QString residueCountText		   = QString();
			QString atomCountText			   = QString();
			QString indexText				   = QString();

			for ( const App::Component::Chemistry::Chain * chain : targets )
			{
				if ( bool( p_flag & SectionFlag::REPRESENTATION ) )
				{
					_representationWidget->updateWithNewValue( *chain->getRepresentation() );

					for ( App::Application::Representation::InstantiatedRepresentation * representation :
						  chain->getSubRepresentations() )
					{
						_subRepresentationWidget->addModel( representation );
					}

					const std::string strTitle = "Representations in children ("
												 + std::to_string( _subRepresentationWidget->getModelCount() ) + ")";
					_subRepresentationFoldingButton->setTitle( QString::fromStdString( strTitle ) );
				}

				if ( bool( p_flag & SectionFlag::INFOS ) )
				{
					// Count residue sum
					const uint residueCount = chain->getRealResidueCount();
					residueSum += chain->getRealResidueCount();

					// Count atom sum
					const uint atomCount = chain->computeRealAtomCount();
					atomSum += atomCount;

					if ( currentChainDisplayedCount < Style::MULTIPLE_INSPECTOR_INFO_DATA_COUNT_DISPLAYED )
					{
						if ( currentChainDisplayedCount > 0 )
						{
							fullNameText.append( " ; " );
							residueCountText.append( " ; " );
							atomCountText.append( " ; " );
							indexText.append( " ; " );
						}

						fullNameText.append( QString::fromStdString( chain->getName() ) );
						residueCountText.append( QString::fromStdString( std::to_string( residueCount ) ) );
						atomCountText.append( QString::fromStdString( std::to_string( atomCount ) ) );
						indexText.append( QString::fromStdString( std::to_string( chain->getIndex() ) ) );
					}
					else if ( currentChainDisplayedCount == Style::MULTIPLE_INSPECTOR_INFO_DATA_COUNT_DISPLAYED )
					{
						fullNameText.append( "..." );
						residueCountText.append( "..." );
						atomCountText.append( "..." );
						indexText.append( "..." );
					}
				}

				currentChainDisplayedCount++;
			}

			if ( bool( p_flag & SectionFlag::INFOS ) )
			{
				const bool displaySums = targets.size() > 1;

				if ( displaySums )
				{
					_residuesSumLabel->setText( QString::fromStdString( std::to_string( residueSum ) ) );
					_atomsSumLabel->setText( QString::fromStdString( std::to_string( atomSum ) ) );
				}

				_infoSectionlayout->setAttributeVisibility( _residuesSumLabel, displaySums );
				_infoSectionlayout->setAttributeVisibility( _atomsSumLabel, displaySums );

				_fullnameLabel->setText( fullNameText );
				_residuesCountLabel->setText( residueCountText );
				_atomsCountLabel->setText( atomCountText );
				_indexLabel->setText( indexText );
			}
		}

		blockSignals( blockSignalState );
	}

	void MultipleChainWidget::_resetFieldStates( const SectionFlag & p_flag )
	{
		if ( bool( p_flag & SectionFlag::REPRESENTATION ) )
		{
			_representationWidget->resetState();
			_subRepresentationWidget->clearModels();
		}

		if ( bool( p_flag & SectionFlag::INFOS ) )
		{
			_fullnameLabel->setText( "" );
			_residuesCountLabel->setText( "" );
			_residuesSumLabel->setText( "" );
			_atomsCountLabel->setText( "" );
			_atomsSumLabel->setText( "" );
			_indexLabel->setText( "" );
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
		VTX_ACTION( new App::Action::Chain::ChangeRepresentationPreset( getTargets(), p_presetIndex ) );
	}
	void MultipleChainWidget::_onRepresentationChange(
		const App::Application::Representation::InstantiatedRepresentation & p_representation,
		const App::Application::Representation::MEMBER_FLAG &				 p_flag )
	{
		if ( !signalsBlocked() )
		{
			VTX_ACTION( new App::Action::Chain::ApplyRepresentation( getTargets(), p_representation, p_flag ) );
		}
	}

	void MultipleChainWidget::_onRepresentationColorChange(
		const App::Application::Representation::InstantiatedRepresentation & p_representation,
		const Util::Color::Rgba &											 p_color,
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
					VTX_ACTION( new App::Action::InstantiatedRepresentation::ChangeColor( getTargets(), p_color ) );
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

	void MultipleChainWidget::_changeMoleculesColor( const Util::Color::Rgba & p_color ) const
	{
		std::unordered_set<App::Component::Chemistry::Molecule *> molecules
			= std::unordered_set<App::Component::Chemistry::Molecule *>();

		for ( const App::Component::Chemistry::Chain * const item : getTargets() )
		{
			molecules.emplace( item->getMoleculePtr() );
		}

		VTX_ACTION( new App::Action::Molecule::ChangeColor( molecules, p_color ) );
	}

	void MultipleChainWidget::_onRevertRepresentation() const
	{
		VTX_ACTION( new App::Action::Chain::RemoveRepresentation( getTargets() ) );
	}
	void MultipleChainWidget::_onApplyRepresentationToChildren() const
	{
		VTX_ACTION( new App::Action::Chain::RemoveChildrenRepresentations( getTargets() ) );
	}

	void MultipleChainWidget::_setInspectorToMolecule() const
	{
		VTXApp::get()
			.getMainWindow()
			.getWidget<Inspector::InspectorWidget>( UI::ID::Window::INSPECTOR )
			.forceInspector( Inspector::InspectorWidget::INSPECTOR_TYPE::MOLECULE );
	}
	void MultipleChainWidget::_setInspectorToResidue() const
	{
		VTXApp::get()
			.getMainWindow()
			.getWidget<Inspector::InspectorWidget>( UI::ID::Window::INSPECTOR )
			.forceInspector( Inspector::InspectorWidget::INSPECTOR_TYPE::RESIDUE );
	}
	void MultipleChainWidget::_setInspectorToAtom() const
	{
		VTXApp::get()
			.getMainWindow()
			.getWidget<Inspector::InspectorWidget>( UI::ID::Window::INSPECTOR )
			.forceInspector( Inspector::InspectorWidget::INSPECTOR_TYPE::ATOM );
	}

} // namespace VTX::UI::Widget::Inspector
