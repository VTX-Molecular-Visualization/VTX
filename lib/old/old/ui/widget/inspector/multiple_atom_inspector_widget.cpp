#include "multiple_atom_inspector_widget.hpp"
#include "action/action_manager.hpp"
#include "model/bond.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "style.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/inspector/inspector_widget.hpp"
#include "ui/widget_factory.hpp"
#include "util/ui.hpp"
#include "vtx_app.hpp"
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <set>

namespace VTX::UI::Widget::Inspector
{
	MultipleAtomWidget::MoleculeView::MoleculeView( const Model::Molecule * p_molecule )
	{
		_view = MVC::MvcManager::get().instantiateView<View::CallbackView<const Model::Molecule, MultipleAtomWidget>>(
			p_molecule, ID::View::UI_INSPECTOR_ATOM );
	}
	MultipleAtomWidget::MoleculeView::~MoleculeView()
	{
		MVC::MvcManager::get()
			.deleteView<View::CallbackView<const Model::Molecule, MultipleAtomWidget>, const Model::Molecule>(
				_view, ID::View::UI_INSPECTOR_ATOM );
	}

	void MultipleAtomWidget::MoleculeViewContainer::addViewOnMolecule( const Model::Molecule * p_molecule )
	{
		std::map<const Model::Molecule *, MoleculeView *>::const_iterator it = _mapMolecules.find( p_molecule );

		if ( it == _mapMolecules.end() )
		{
			MoleculeView * const moleculeView = new MoleculeView( p_molecule );
			_mapMolecules[ p_molecule ]		  = moleculeView;
			moleculeView->getView().setCallback( _linkedInspector, &MultipleAtomWidget::_eventCalledOnMolecule );
		}
		else
			it->second->addViewer();
	}
	void MultipleAtomWidget::MoleculeViewContainer::removeViewOnMolecule( const Model::Molecule * p_molecule )
	{
		_mapMolecules[ p_molecule ]->removeViewer();

		if ( !_mapMolecules[ p_molecule ]->hasViewer() )
		{
			delete ( _mapMolecules[ p_molecule ] );
			_mapMolecules.erase( p_molecule );
		}
	}
	void MultipleAtomWidget::MoleculeViewContainer::clear()
	{
		for ( const std::pair<const Model::Molecule *, MoleculeView *> & pair : _mapMolecules )
		{
			delete ( pair.second );
		}

		_mapMolecules.clear();
	}

	MultipleAtomWidget::MultipleAtomWidget( QWidget * p_parent ) :
		MultipleModelInspectorWidget( p_parent, ID::View::UI_INSPECTOR_ATOM ) {};

	MultipleAtomWidget::~MultipleAtomWidget() {}

	void MultipleAtomWidget::_setupUi( const QString & p_name )
	{
		MultipleModelInspectorWidget::_setupUi( p_name );

		// TRANSFORM SECTION ///////////////////////////////////////////////////////////////////////////////////////////
		_transformSection
			= VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSection>( this, "transform_section " );

		_transformWidget
			= VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::TransformWidget>( this, "transformWidget" );
		_transformWidget->displayPosition( true );
		_transformWidget->displayRotation( false );
		_transformWidget->displayScale( false );
		_transformWidget->setEnabled( false );

		_transformSection->setBody( _transformWidget );
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// INFO SECTION ////////////////////////////////////////////////////////////////////////////////////////////////
		_infoSection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
			this, "inspector_item_section" );

		_genericNameLabel = new CustomWidget::QLabelMultiField( this );
		_genericNameLabel->setWordWrap( true );

		_nameLabel = new CustomWidget::QLabelMultiField( this );
		_nameLabel->setWordWrap( true );

		_bondInfoLabel = new QLabel( this );

		_infoSection->appendField( "Name", _nameLabel );
		_infoSection->appendField( "Full Name", _genericNameLabel );
		_infoSection->appendField( "Bonds", _bondInfoLabel );
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		_appendSection( _transformSection );
		_appendSection( _infoSection );

		const bool oldBlockState = blockSignals( true );
		refresh();
		blockSignals( oldBlockState );
	}

	void MultipleAtomWidget::_setupSlots()
	{
		QMenu * const headerMenu = new QMenu( this );

		QAction * inspectorToMoleculeAction = new QAction( "Molecule", this );
		connect( inspectorToMoleculeAction, &QAction::triggered, this, &MultipleAtomWidget::_setInspectorToMolecule );
		headerMenu->addAction( inspectorToMoleculeAction );

		QAction * inspectorToChainAction = new QAction( "Chain", this );
		connect( inspectorToChainAction, &QAction::triggered, this, &MultipleAtomWidget::_setInspectorToChain );
		headerMenu->addAction( inspectorToChainAction );

		QAction * inspectorToResidueAction = new QAction( "Residue", this );
		connect( inspectorToResidueAction, &QAction::triggered, this, &MultipleAtomWidget::_setInspectorToResidue );
		headerMenu->addAction( inspectorToResidueAction );

		_getHeader()->setMenu( headerMenu );
	};

	void MultipleAtomWidget::_endOfFrameRefresh( const SectionFlag & p_flag )
	{
		MultipleModelInspectorWidget::_endOfFrameRefresh( p_flag );

		_resetFieldStates( p_flag );

		const std::unordered_set<Model::Atom *> & targets = getTargets();

		if ( targets.size() > 0 )
		{
			const QString headerTitle = QString::fromStdString( "Atom (" + std::to_string( targets.size() ) + ")" );
			_getHeader()->setHeaderTitle( headerTitle );

			const QPixmap * symbolPixmap = Style::IconConst::get().getModelSymbol( VTX::ID::Model::MODEL_ATOM );
			_getHeader()->setHeaderIcon( *symbolPixmap );

			for ( const Model::Atom * atom : targets )
			{
				if ( bool( p_flag & SectionFlag::TRAJECTORY_TIMER ) )
				{
					_transformCache.setTranslation( atom->getLocalPosition() );
					_transformWidget->updateWithNewValue( _transformCache );
				}
				if ( bool( p_flag & SectionFlag::INFOS ) )
				{
					_nameLabel->updateWithNewValue( atom->getName() );
					_genericNameLabel->updateWithNewValue( atom->getSymbolName() );
				}
			}

			_fillBondData();
		}
	}

	void MultipleAtomWidget::_resetFieldStates( const SectionFlag & p_flag )
	{
		if ( bool( p_flag & SectionFlag::TRAJECTORY_TIMER ) )
		{
			_transformCache.reset();
			_transformWidget->resetState();
		}
		if ( bool( p_flag & SectionFlag::INFOS ) )
		{
			_nameLabel->resetState();
			_genericNameLabel->resetState();
			_bondInfoLabel->setText( "" );
		}
	}

	void MultipleAtomWidget::_fillBondData()
	{
		std::set<uint> pendingBondIDs = std::set<uint>();

		auto comparer = []( const Model::Bond * p_lhs, const Model::Bond * p_rhs )
		{ return Model::Bond::comparer( *p_lhs, *p_rhs ); };

		std::set<const Model::Bond *, decltype( comparer )> fullBonds( comparer );

		for ( const Model::Atom * const atom : getTargets() )
		{
			const uint					  atomIndex		 = atom->getIndex();
			const Model::Residue * const  residue		 = atom->getResiduePtr();
			const Model::Molecule * const moleculeParent = residue->getMoleculePtr();

			for ( uint bondIndex = residue->getIndexFirstBond();
				  bondIndex < ( residue->getIndexFirstBond() + residue->getBondCount() );
				  bondIndex++ )
			{
				const Model::Bond * const bond = moleculeParent->getBond( bondIndex );

				if ( bond == nullptr )
					continue;

				if ( bond->getIndexFirstAtom() == atomIndex || bond->getIndexSecondAtom() == atomIndex )
				{
					if ( pendingBondIDs.find( bondIndex ) == pendingBondIDs.cend() )
					{
						pendingBondIDs.emplace( bondIndex );
					}
					else
					{
						pendingBondIDs.erase( bondIndex );
						fullBonds.emplace( bond );

						if ( fullBonds.size() >= Style::INSPECTOR_INFO_BOND_COUNT_DISPLAYED )
							break;
					}
				}
			}

			if ( fullBonds.size() >= Style::INSPECTOR_INFO_BOND_COUNT_DISPLAYED )
				break;
		}

		QString bondInfoStr;
		for ( const Model::Bond * const bond : fullBonds )
		{
			Util::UI::appendBondInfo( *bond, bondInfoStr );
		}
		_bondInfoLabel->setText( bondInfoStr );
	}

	void MultipleAtomWidget::localize()
	{
		_infoSection->setHeaderTitle( "Infos" );
		_infoSection->localize();
	}

	void MultipleAtomWidget::_eventCalledOnMolecule( const Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::TRAJECTORY_FRAME_CHANGE )
			_onTargetChangeEvent( nullptr, p_event );
	}

	void MultipleAtomWidget::clearTargets()
	{
		MultipleModelInspectorWidget::clearTargets();
		_moleculeViewerContainer.clear();
	}
	void MultipleAtomWidget::addTarget( Model::Atom * const p_target )
	{
		MultipleModelInspectorWidget::addTarget( p_target );
		_moleculeViewerContainer.addViewOnMolecule( p_target->getMoleculePtr() );
	}
	void MultipleAtomWidget::removeTarget( Model::Atom * const p_target )
	{
		MultipleModelInspectorWidget::removeTarget( p_target );
		_moleculeViewerContainer.removeViewOnMolecule( p_target->getMoleculePtr() );
	}

	void MultipleAtomWidget::_setInspectorToMolecule() const
	{
		VTXApp::get()
			.getMainWindow()
			.getWidget<Inspector::InspectorWidget>( ID::UI::Window::INSPECTOR )
			.forceInspector( Inspector::InspectorWidget::INSPECTOR_TYPE::MOLECULE );
	}
	void MultipleAtomWidget::_setInspectorToChain() const
	{
		VTXApp::get()
			.getMainWindow()
			.getWidget<Inspector::InspectorWidget>( ID::UI::Window::INSPECTOR )
			.forceInspector( Inspector::InspectorWidget::INSPECTOR_TYPE::CHAIN );
	}
	void MultipleAtomWidget::_setInspectorToResidue() const
	{
		VTXApp::get()
			.getMainWindow()
			.getWidget<Inspector::InspectorWidget>( ID::UI::Window::INSPECTOR )
			.forceInspector( Inspector::InspectorWidget::INSPECTOR_TYPE::RESIDUE );
	}
} // namespace VTX::UI::Widget::Inspector
