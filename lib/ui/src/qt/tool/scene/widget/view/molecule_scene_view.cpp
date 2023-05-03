#include "ui/qt/tool/scene/widget/view/molecule_scene_view.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/qt/action/atom.hpp"
#include "ui/qt/action/category.hpp"
#include "ui/qt/action/chain.hpp"
#include "ui/qt/action/molecule.hpp"
#include "ui/qt/action/residue.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/contextual_menu.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/style.hpp"
#include "ui/qt/tool/keys.hpp"
#include "ui/qt/tool/scene/widget/scene_item_selection_model.hpp"
#include "ui/qt/util.hpp"
#include "ui/qt/widget/contextual_menu/contextual_menu_selection.hpp"
#include "ui/qt/widget_factory.hpp"
#include <QScrollBar>
#include <app/action/atom.hpp>
#include <app/action/category.hpp>
#include <app/action/chain.hpp>
#include <app/action/molecule.hpp>
#include <app/action/residue.hpp>
#include <app/action/selection.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/event/global.hpp>
#include <app/internal/math/range.hpp>
#include <app/mvc.hpp>
#include <util/logger.hpp>
#include <util/string.hpp>

namespace VTX::UI::QT::Tool::Scene::Widget::View
{
	MoleculeSceneView::MoleculeSceneView( App::Component::Chemistry::Molecule * const p_model,
										  QWidget * const							  p_parent ) :
		VTX::App::Core::View::BaseView<App::Component::Chemistry::Molecule>( p_model ),
		SceneItemWidget( p_parent )
	{
		_registerEvent( VTX::App::Event::Global::SETTINGS_CHANGE );
	}

	MoleculeSceneView ::~MoleculeSceneView() { _clearLoadedItems(); }

	void MoleculeSceneView::notify( const VTX::App::Core::Event::VTXEvent * const p_event )
	{
		if ( p_event->name == VTX::App::Event::Model::MOLECULE_VISIBILITY )
		{
			_refreshItemVisibility( _getMoleculeTreeWidgetItem(), _model->isVisible() );
		}
		else if ( p_event->name == VTX::App::Event::Model::CATEGORY_VISIBILITY )
		{
			const VTX::App::Core::Event::VTXEventArg<App::Internal::ChemDB::Category::TYPE> * const castedEventData
				= dynamic_cast<const VTX::App::Core::Event::VTXEventArg<App::Internal::ChemDB::Category::TYPE> *>(
					p_event );
			const App::Component::Chemistry::Category & category = _model->getCategory( castedEventData->get() );

			if ( _isMoleculeExpanded() )
			{
				_refreshItemVisibility( _getTreeWidgetItem( category ), category.isVisible() );
			}
		}
		else if ( p_event->name == VTX::App::Event::Model::CHAIN_VISIBILITY )
		{
			const VTX::App::Core::Event::VTXEventArg<uint> * const castedEventData
				= dynamic_cast<const VTX::App::Core::Event::VTXEventArg<uint> *>( p_event );
			const uint									index	 = castedEventData->get();
			const App::Component::Chemistry::Chain &	chain	 = *_model->getChain( index );
			const App::Component::Chemistry::Category & category = *( _model->getCategoryFromChain( chain ) );

			// Check if items are visible before refresh it. If not, the will be update when they will appear

			if ( _isCategoryExpanded( category ) )
			{
				_refreshItemVisibility( _getTreeWidgetItem( chain ), chain.isVisible() );
			}
		}
		else if ( p_event->name == VTX::App::Event::Model::RESIDUE_VISIBILITY )
		{
			const VTX::App::Core::Event::VTXEventArg<uint> * const castedEventData
				= dynamic_cast<const VTX::App::Core::Event::VTXEventArg<uint> *>( p_event );
			const uint								   index   = castedEventData->get();
			const App::Component::Chemistry::Residue & residue = *_model->getResidue( index );

			// Check if items are visible before refresh it. If not, the will be update when they will appear
			if ( _isChainExpanded( *residue.getChainPtr() ) )
			{
				_refreshItemVisibility( _getTreeWidgetItem( residue ), residue.isVisible() );
			}
		}
		else if ( p_event->name == VTX::App::Event::Model::ATOM_VISIBILITY )
		{
			const VTX::App::Core::Event::VTXEventArg<uint> * const castedEventData
				= dynamic_cast<const VTX::App::Core::Event::VTXEventArg<uint> *>( p_event );
			const App::Component::Chemistry::Atom & atom = *_model->getAtom( castedEventData->get() );

			if ( _isResidueExpanded( *atom.getResiduePtr() ) )
			{
				_refreshItemVisibility( _getTreeWidgetItem( atom ), atom.isVisible() );
			}
		}
		else if ( p_event->name == VTX::App::Event::Model::VISIBILITY )
		{
			_refreshItemsVisibility();
		}
		else if ( p_event->name == VTX::App::Event::Model::DATA_CHANGE )
		{
			_clearLoadedItems();
			_updateMoleculeStructure();
			_refreshSize();
		}
		else if ( p_event->name == VTX::App::Event::Model::DISPLAY_NAME_CHANGE )
		{
			_getMoleculeTreeWidgetItem()->setText( 0, QString::fromStdString( _model->getDisplayName() ) );
		}
	}

	void MoleculeSceneView::receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event )
	{
		SceneItemWidget::receiveEvent( p_event );

		if ( p_event.name == VTX::App::Event::Global::SETTINGS_CHANGE )
		{
			const VTX::App::Core::Event::VTXEventArg<const std::set<VTX::App::Application::Setting::PARAMETER> &> &
				castedEvent
				= dynamic_cast<const VTX::App::Core::Event::VTXEventArg<
					const std::set<VTX::App::Application::Setting::PARAMETER> &> &>( p_event );

			if ( castedEvent.get().find( VTX::App::Application::Setting::PARAMETER::SYMBOL_DISPLAY_MODE )
				 != castedEvent.get().end() )
			{
				_refreshSymbolDisplay( VTX_SETTING().getSymbolDisplayMode() );
			}
		}
	}

	bool MoleculeSceneView::containsModel( const App::Core::Model::BaseModel & p_model ) const
	{
		const VTX::App::VTX_ID &					modelTypeID = p_model.getTypeId();
		const App::Component::Chemistry::Molecule * linkedMolecule;

		if ( modelTypeID == App::ID::Model::MODEL_MOLECULE )
			linkedMolecule = static_cast<const App::Component::Chemistry::Molecule *>( &p_model );
		else if ( modelTypeID == App::ID::Model::MODEL_CATEGORY )
			linkedMolecule = static_cast<const App::Component::Chemistry::Category &>( p_model ).getMoleculePtr();
		else if ( modelTypeID == App::ID::Model::MODEL_CHAIN )
			linkedMolecule = static_cast<const App::Component::Chemistry::Chain &>( p_model ).getMoleculePtr();
		else if ( modelTypeID == App::ID::Model::MODEL_RESIDUE )
			linkedMolecule = static_cast<const App::Component::Chemistry::Residue &>( p_model ).getMoleculePtr();
		else if ( modelTypeID == App::ID::Model::MODEL_ATOM )
			linkedMolecule = static_cast<const App::Component::Chemistry::Atom &>( p_model ).getMoleculePtr();
		else // Not a valid type. Return false.
			return false;

		return linkedMolecule == _model;
	}

	std::vector<App::Core::Model::ID> MoleculeSceneView::getAllItemsFrom(
		const App::Core::Model::BaseModel & p_model ) const
	{
		const App::VTX_ID & typeID = p_model.getTypeId();

		if ( typeID == App::ID::Model::MODEL_MOLECULE )
		{
			return SceneItemWidget::getAllItemsFrom( p_model );
		}

		std::vector<App::Core::Model::ID> res;

		if ( typeID == App::ID::Model::MODEL_CATEGORY )
		{
			const App::Component::Chemistry::Category & category
				= static_cast<const App::Component::Chemistry::Category &>( p_model );
			res.emplace_back( category.getId() );
			_selectAllCategoriesFrom( res, category );
		}
		else if ( typeID == App::ID::Model::MODEL_CHAIN )
		{
			const App::Component::Chemistry::Chain & chain
				= static_cast<const App::Component::Chemistry::Chain &>( p_model );
			res.emplace_back( chain.getId() );
			_selectAllChainsFrom( res, chain );
		}
		else if ( typeID == App::ID::Model::MODEL_RESIDUE )
		{
			const App::Component::Chemistry::Residue & residue
				= static_cast<const App::Component::Chemistry::Residue &>( p_model );
			res.emplace_back( residue.getId() );
			_selectAllResiduesFrom( res, residue );
			_selectAllChainsFrom( res, *residue.getChainPtr() );
		}
		else if ( typeID == App::ID::Model::MODEL_ATOM )
		{
			const App::Component::Chemistry::Atom & atom
				= static_cast<const App::Component::Chemistry::Atom &>( p_model );
			res.emplace_back( atom.getId() );
			_selectAllAtomsFrom( res, atom );
			_selectAllResiduesFrom( res, *atom.getResiduePtr() );
			_selectAllChainsFrom( res, *atom.getChainPtr() );
		}

		return res;
	}
	std::vector<App::Core::Model::ID> MoleculeSceneView::getAllItemsTo(
		const App::Core::Model::BaseModel & p_model ) const
	{
		const App::VTX_ID & typeID = p_model.getTypeId();

		if ( typeID == App::ID::Model::MODEL_MOLECULE )
		{
			return SceneItemWidget::getAllItemsTo( p_model );
		}

		std::vector<App::Core::Model::ID> res;

		if ( typeID == App::ID::Model::MODEL_CATEGORY )
		{
			const App::Component::Chemistry::Category & category
				= static_cast<const App::Component::Chemistry::Category &>( p_model );
			res.emplace_back( category.getId() );
			_selectAllCategoriesTo( res, category );
		}
		else if ( typeID == App::ID::Model::MODEL_CHAIN )
		{
			const App::Component::Chemistry::Chain & chain
				= static_cast<const App::Component::Chemistry::Chain &>( p_model );
			res.emplace_back( chain.getId() );
			_selectAllChainsTo( res, chain );
		}
		else if ( typeID == App::ID::Model::MODEL_RESIDUE )
		{
			const App::Component::Chemistry::Residue & residue
				= static_cast<const App::Component::Chemistry::Residue &>( p_model );
			res.emplace_back( residue.getId() );
			_selectAllResiduesFrom( res, residue );
			_selectAllChainsTo( res, *residue.getChainPtr() );
		}
		else if ( typeID == App::ID::Model::MODEL_ATOM )
		{
			const App::Component::Chemistry::Atom & atom
				= static_cast<const App::Component::Chemistry::Atom &>( p_model );
			res.emplace_back( atom.getId() );
			_selectAllAtomsFrom( res, atom );
			_selectAllResiduesFrom( res, *atom.getResiduePtr() );
			_selectAllChainsTo( res, *atom.getChainPtr() );
		}

		return res;
	}

	QTreeWidgetItem * MoleculeSceneView::getLastVisibleItem()
	{
		QTreeWidgetItem * lastItemVisible = _getMoleculeTreeWidgetItem();

		while ( lastItemVisible->childCount() > 0 && lastItemVisible->isExpanded() )
			lastItemVisible = lastItemVisible->child( lastItemVisible->childCount() - 1 );

		return lastItemVisible;
	}

	void MoleculeSceneView::_setupUi( const QString & p_name ) { SceneItemWidget::_setupUi( p_name ); }
	void MoleculeSceneView::_setupSlots()
	{
		SceneItemWidget::_setupSlots();

		connect( this, &QTreeWidget::itemChanged, this, &MoleculeSceneView::_onItemChanged );
		connect( this, &QTreeWidget::itemDoubleClicked, this, &MoleculeSceneView::_onItemDoubleClicked );
	}

	void MoleculeSceneView::_onItemChanged( QTreeWidgetItem * const p_item, const int p_column )
	{
		if ( p_column == 0 )
		{
			if ( p_item == _getMoleculeTreeWidgetItem() )
			{
				std::string itemTxt = p_item->text( 0 ).toStdString();
				if ( itemTxt != _model->getDisplayName() )
				{
					_reformatMoleculeName( itemTxt );

					if ( itemTxt.empty() )
					{
						itemTxt = _model->getDisplayName();
					}
					else if ( itemTxt != _model->getDisplayName() )
					{
						VTX_ACTION( new VTX::App::Action::Molecule::Rename( *_model, itemTxt ) );
					}

					const bool oldSignalState = blockSignals( true );
					p_item->setText( 0, QString::fromStdString( itemTxt ) );
					blockSignals( oldSignalState );

					return;
				}
			}

			_doEnableStateChangeAction( p_item );
		}
	}
	void MoleculeSceneView::_onItemDoubleClicked( const QTreeWidgetItem * const p_item, const int p_column ) const
	{
		const App::Core::Model::ID & modelId	 = _getModelIDFromItem( *p_item );
		const App::VTX_ID &			 modelTypeId = VTX::MVC_MANAGER().getModelTypeID( modelId );

		if ( modelTypeId == App::ID::Model::MODEL_MOLECULE )
		{
			App::Component::Chemistry::Molecule & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( modelId );
			VTX_ACTION( new QT::Action::Molecule::Orient( model ) );
		}
		else if ( modelTypeId == App::ID::Model::MODEL_CATEGORY )
		{
			App::Component::Chemistry::Category & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Category>( modelId );
			VTX_ACTION( new QT::Action::Category::Orient( model ) );
		}
		else if ( modelTypeId == App::ID::Model::MODEL_CHAIN )
		{
			App::Component::Chemistry::Chain & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Chain>( modelId );
			VTX_ACTION( new QT::Action::Chain::Orient( model ) );
		}
		else if ( modelTypeId == App::ID::Model::MODEL_RESIDUE )
		{
			App::Component::Chemistry::Residue & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Residue>( modelId );
			VTX_ACTION( new QT::Action::Residue::Orient( model ) );
		}
		else if ( modelTypeId == App::ID::Model::MODEL_ATOM )
		{
			App::Component::Chemistry::Atom & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Atom>( modelId );
			VTX_ACTION( new QT::Action::Atom::Orient( model ) );
		}
	}
	void MoleculeSceneView::_onItemExpanded( QTreeWidgetItem * const p_item )
	{
		const App::Core::Model::ID & modelId	 = _getModelIDFromItem( *p_item );
		const App::VTX_ID &			 modelTypeId = VTX::MVC_MANAGER().getModelTypeID( modelId );

		if ( modelTypeId == App::ID::Model::MODEL_MOLECULE )
		{
			_expandMolecule( p_item );
		}
		else if ( modelTypeId == App::ID::Model::MODEL_CATEGORY )
		{
			_expandCategory( p_item );
		}
		else if ( modelTypeId == App::ID::Model::MODEL_CHAIN )
		{
			_expandChain( p_item );
		}
		else if ( modelTypeId == App::ID::Model::MODEL_RESIDUE )
		{
			_expandResidue( p_item );
		}

		_refreshSelection( App::Application::Selection::SelectionManager::get().getSelectionModel() );
		SceneItemWidget::_onItemExpanded( p_item );
	}
	void MoleculeSceneView::_onItemCollapsed( QTreeWidgetItem * const p_item )
	{
		_collapseItem( *p_item );
		SceneItemWidget::_onItemCollapsed( p_item );
	}

	void MoleculeSceneView::_onCustomContextMenuCalled( const QPoint & p_clicPos )
	{
		const QTreeWidgetItem * const targetedItem = itemAt( p_clicPos );

		if ( targetedItem == nullptr )
			return;

		const App::Core::Model::ID & modelId	   = _getModelIDFromItem( *targetedItem );
		const App::VTX_ID &			 modelTypeId   = VTX::MVC_MANAGER().getModelTypeID( modelId );
		const QPoint				 globalClicPos = mapToGlobal( p_clicPos );

		App::Application::Selection::SelectionModel & selection
			= App::Application::Selection::SelectionManager::get().getSelectionModel();
		UI::QT::ContextualMenu & contextualMenu = QT_APP()->getMainWindow().getContextualMenu();

		if ( modelTypeId == App::ID::Model::MODEL_MOLECULE )
		{
			App::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( modelId );
			if ( selection.isMoleculeFullySelected( molecule ) )
			{
				contextualMenu
					.getMenu<VTX::UI::QT::Widget::ContextualMenu::ContextualMenuSelection>(
						Tool::ContextualMenu::SELECTION )
					->setFocusedTarget( &molecule );

				contextualMenu.pop( Tool::ContextualMenu::SELECTION, &selection, globalClicPos );
			}
		}
		else if ( modelTypeId == App::ID::Model::MODEL_CATEGORY )
		{
			App::Component::Chemistry::Category & category
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Category>( modelId );
			if ( selection.isCategoryFullySelected( category ) )
			{
				contextualMenu
					.getMenu<VTX::UI::QT::Widget::ContextualMenu::ContextualMenuSelection>(
						Tool::ContextualMenu::SELECTION )
					->setFocusedTarget( &category );

				contextualMenu.pop( Tool::ContextualMenu::SELECTION, &selection, globalClicPos );
			}
		}
		else if ( modelTypeId == App::ID::Model::MODEL_CHAIN )
		{
			App::Component::Chemistry::Chain & chain
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Chain>( modelId );
			if ( selection.isChainFullySelected( chain ) )
			{
				contextualMenu
					.getMenu<VTX::UI::QT::Widget::ContextualMenu::ContextualMenuSelection>(
						Tool::ContextualMenu::SELECTION )
					->setFocusedTarget( &chain );

				contextualMenu.pop( Tool::ContextualMenu::SELECTION, &selection, globalClicPos );
			}
		}
		else if ( modelTypeId == App::ID::Model::MODEL_RESIDUE )
		{
			App::Component::Chemistry::Residue & residue
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Residue>( modelId );
			if ( selection.isResidueFullySelected( residue ) )
			{
				contextualMenu
					.getMenu<VTX::UI::QT::Widget::ContextualMenu::ContextualMenuSelection>(
						Tool::ContextualMenu::SELECTION )
					->setFocusedTarget( &residue );

				contextualMenu.pop( Tool::ContextualMenu::SELECTION, &selection, globalClicPos );
			}
		}
		else if ( modelTypeId == App::ID::Model::MODEL_ATOM )
		{
			App::Component::Chemistry::Atom & atom
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Atom>( modelId );
			if ( selection.isAtomSelected( atom ) )
			{
				contextualMenu
					.getMenu<VTX::UI::QT::Widget::ContextualMenu::ContextualMenuSelection>(
						Tool::ContextualMenu::SELECTION )
					->setFocusedTarget( &atom );

				contextualMenu.pop( Tool::ContextualMenu::SELECTION, &selection, globalClicPos );
			}
		}
	}

	void MoleculeSceneView::_createTopLevelObject()
	{
		collapseItem( _getMoleculeTreeWidgetItem() );

		clear();
		_clearLoadedItems();

		SceneItemWidget::_createTopLevelObject();

		const QTreeWidgetItem::ChildIndicatorPolicy childIndicatorPolicy
			= _model->getChainCount() > 0 ? QTreeWidgetItem::ChildIndicatorPolicy::ShowIndicator
										  : QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicator;

		topLevelItem( 0 )->setChildIndicatorPolicy( childIndicatorPolicy );
	}

	void MoleculeSceneView::_updateMoleculeStructure()
	{
		QTreeWidgetItem * const item = _getMoleculeTreeWidgetItem();

		if ( !item->isExpanded() )
			return;

		for ( int i = 0; i < int( App::Internal::ChemDB::Category::TYPE::COUNT ); i++ )
		{
			QTreeWidgetItem * const currentCategoryItem = item->child( i );

			const App::Component::Chemistry::Category & category
				= _model->getCategory( App::Internal::ChemDB::Category::TYPE( i ) );

			if ( category.isEmpty() )
			{
				currentCategoryItem->setHidden( true );
			}
			else if ( currentCategoryItem->isExpanded() )
			{
				_updateCategoryStructure( category, *currentCategoryItem );
			}
		}
	}
	void MoleculeSceneView::_updateCategoryStructure( const App::Component::Chemistry::Category & p_category,
													  QTreeWidgetItem &							  p_item )
	{
		const App::Component::Chemistry::Molecule & molecule	 = *p_category.getMoleculePtr();
		std::vector<uint>							chainIndexes = p_category.getChains();

		uint currentChainIndexInCategory = 0;
		int	 currentChildIndex			 = 0;

		while ( currentChildIndex < p_item.childCount() )
		{
			QTreeWidgetItem * const currentChainItem = p_item.child( currentChildIndex );

			if ( currentChainIndexInCategory >= chainIndexes.size() )
			{
				delete ( p_item.takeChild( p_item.childCount() - 1 ) );
				continue;
			}

			const uint chainIndex = chainIndexes[ currentChainIndexInCategory ];

			const App::Core::Model::ID &				   chainItemID = _getModelIDFromItem( *currentChainItem );
			const App::Component::Chemistry::Chain * const chain	   = _model->getChain( chainIndex );

			if ( chain == nullptr )
			{
				currentChainIndexInCategory++;
			}
			else if ( chainItemID != chain->getId() )
			{
				// Chain index never changes
				// if ( currentChildIndex == 0 )
				//{
				//	delete ( item->takeChild( currentChildIndex ) );
				//}
				// else
				{
					currentChainItem->setHidden( true );
					currentChildIndex++;
				}
			}
			else
			{
				currentChainIndexInCategory++;
				currentChildIndex++;

				if ( currentChainItem->isExpanded() )
					_updateChainStructure( *chain, *currentChainItem );
			}
		}
	}
	void MoleculeSceneView::_updateChainStructure( const App::Component::Chemistry::Chain & p_chain,
												   QTreeWidgetItem &						p_item )
	{
		const App::Component::Chemistry::Molecule & molecule			= *p_chain.getMoleculePtr();
		uint										currentResidueIndex = 0;
		int											currentChildIndex	= 0;

		while ( currentChildIndex < p_item.childCount() )
		{
			QTreeWidgetItem * const currentResidueItem = p_item.child( currentChildIndex );

			if ( currentResidueIndex >= p_chain.getResidueCount() )
			{
				delete ( p_item.takeChild( p_item.childCount() - 1 ) );
				continue;
			}

			const App::Core::Model::ID &					 residueItemID = _getModelIDFromItem( *currentResidueItem );
			const App::Component::Chemistry::Residue * const residue
				= molecule.getResidue( p_chain.getIndexFirstResidue() + currentResidueIndex );

			if ( residue == nullptr )
			{
				currentResidueIndex++;
			}
			else if ( residueItemID != residue->getId() )
			{
				if ( currentChildIndex == 0 )
				{
					delete ( p_item.takeChild( currentChildIndex ) );
				}
				else
				{
					currentResidueItem->setHidden( true );
					currentChildIndex++;
				}
			}
			else
			{
				currentResidueIndex++;
				currentChildIndex++;

				if ( currentResidueItem->isExpanded() )
					_updateResidueStructure( *residue, *currentResidueItem );
			}
		}
	}
	void MoleculeSceneView::_updateResidueStructure( const App::Component::Chemistry::Residue & p_residue,
													 QTreeWidgetItem &							p_item )
	{
		App::Component::Chemistry::Molecule & molecule			= *p_residue.getMoleculePtr();
		uint								  currentAtomIndex	= 0;
		int									  currentChildIndex = 0;

		while ( currentChildIndex < p_item.childCount() )
		{
			QTreeWidgetItem * const currentAtomItem = p_item.child( currentChildIndex );

			// Delete all items out of range
			if ( currentAtomIndex >= p_residue.getAtomCount() )
			{
				delete ( p_item.takeChild( p_item.childCount() - 1 ) );
				continue;
			}

			const App::Core::Model::ID &				  atomItemID = _getModelIDFromItem( *currentAtomItem );
			const App::Component::Chemistry::Atom * const atom
				= molecule.getAtom( p_residue.getIndexFirstAtom() + currentAtomIndex );

			if ( atom == nullptr )
			{
				currentAtomIndex++;
			}
			else if ( atomItemID != atom->getId() )
			{
				if ( currentChildIndex == 0 )
				{
					delete ( p_item.takeChild( currentChildIndex ) );
				}
				else
				{
					currentAtomItem->setHidden( true );
					currentChildIndex++;
				}
			}
			else
			{
				currentAtomIndex++;
				currentChildIndex++;
			}
		}
	}

	void MoleculeSceneView::_clearLoadedItems()
	{
		for ( const std::pair<QTreeWidgetItem *, QList<QTreeWidgetItem *> *> & pair : _mapLoadedItems )
		{
			QList<QTreeWidgetItem *> * const listItems = pair.second;
			while ( listItems->size() > 0 )
			{
				delete listItems->takeLast();
			}

			delete ( listItems );
		}

		_mapLoadedItems.clear();
	}

	void MoleculeSceneView::_expandAll( QTreeWidgetItem * const p_from )
	{
		const App::VTX_ID & modelTypeId = VTX::MVC_MANAGER().getModelTypeID( _getModelIDFromItem( *p_from ) );

		_enableSignals( false );

		if ( modelTypeId == App::ID::Model::MODEL_MOLECULE )
			_expandMolecule( p_from, true );
		else if ( modelTypeId == App::ID::Model::MODEL_CHAIN )
			_expandChain( p_from, true );
		else if ( modelTypeId == App::ID::Model::MODEL_RESIDUE )
			_expandResidue( p_from, true );

		_enableSignals( true );

		p_from->setExpanded( true );
	}
	void MoleculeSceneView::_expandMolecule( QTreeWidgetItem * const p_moleculeItem, const bool p_forceExpandChildren )
	{
		_enableSignals( false );

		QList<QTreeWidgetItem *> * itemsPtr;

		if ( p_moleculeItem->childCount() > 0 )
		{
			itemsPtr = new QList<QTreeWidgetItem *>();
			_fillListWithItemChildren( *p_moleculeItem, *itemsPtr );
		}
		else if ( _mapLoadedItems.find( p_moleculeItem ) != _mapLoadedItems.end() )
		{
			itemsPtr = _mapLoadedItems[ p_moleculeItem ];
			p_moleculeItem->addChildren( *itemsPtr );
			_mapLoadedItems.erase( p_moleculeItem );
		}
		else
		{
			itemsPtr								 = new QList<QTreeWidgetItem *>();
			QList<QTreeWidgetItem *> &	   items	 = *itemsPtr;
			std::vector<QTreeWidgetItem *> nullItems = std::vector<QTreeWidgetItem *>();

			const std::vector<App::Component::Chemistry::Category *> categories	   = _model->getFilledCategories();
			const size_t											 categoryCount = categories.size();
			items.reserve( categoryCount );
			nullItems.reserve( categoryCount );

			for ( int i = 0; i < int( App::Internal::ChemDB::Category::TYPE::COUNT ); i++ )
			{
				const App::Component::Chemistry::Category & category
					= _model->getCategory( App::Internal::ChemDB::Category::TYPE( i ) );

				QTreeWidgetItem * const categoryView = new QTreeWidgetItem();

				_applyCategoryDataOnItem( category, *categoryView );

				if ( category.isEmpty() )
					nullItems.emplace_back( categoryView );

				items.append( categoryView );
			}

			p_moleculeItem->addChildren( items );

			// setHidden Need to be called after adding items in view
			for ( QTreeWidgetItem * const nullItem : nullItems )
				nullItem->setHidden( true );
		}

		// Update visibility here because it can be modified when the parent is collapsed
		for ( QTreeWidgetItem * const item : *itemsPtr )
		{
			if ( item->isHidden() )
				continue;

			const App::Core::Model::ID &				categoryID = _getModelIDFromItem( *item );
			const App::Component::Chemistry::Category & category
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Category>( categoryID );

			const Qt::CheckState newCheckState = QT::Util::getCheckState( category.isVisible() );
			item->setCheckState( 0, newCheckState );

			if ( p_forceExpandChildren || _getItemExpandState( *item ) )
			{
				item->setExpanded( true );
				_expandCategory( item, p_forceExpandChildren );
			}
		}

		delete itemsPtr;

		_enableSignals( true );
	}
	void MoleculeSceneView::_expandCategory( QTreeWidgetItem * const p_categoryItem, const bool p_forceExpandChildren )
	{
		_enableSignals( false );

		QList<QTreeWidgetItem *> * itemsPtr;

		if ( p_categoryItem->childCount() > 0 )
		{
			itemsPtr = new QList<QTreeWidgetItem *>();
			_fillListWithItemChildren( *p_categoryItem, *itemsPtr );
		}
		else if ( _mapLoadedItems.find( p_categoryItem ) != _mapLoadedItems.end() )
		{
			itemsPtr = _mapLoadedItems[ p_categoryItem ];
			p_categoryItem->addChildren( *itemsPtr );
			_mapLoadedItems.erase( p_categoryItem );
		}
		else
		{
			itemsPtr								 = new QList<QTreeWidgetItem *>();
			QList<QTreeWidgetItem *> &	   items	 = *itemsPtr;
			std::vector<QTreeWidgetItem *> nullItems = std::vector<QTreeWidgetItem *>();

			const App::Core::Model::ID &				categoryId = _getModelIDFromItem( *p_categoryItem );
			const App::Component::Chemistry::Category & category
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Category>( categoryId );

			const std::vector<uint> & chains = category.getChains();

			const size_t chainCount = chains.size();
			nullItems.reserve( chainCount );
			items.reserve( chainCount );

			for ( uint chainIndex : chains )
			{
				const App::Component::Chemistry::Chain * const chainPtr	 = _model->getChain( chainIndex );
				QTreeWidgetItem * const						   chainView = new QTreeWidgetItem();

				if ( chainPtr == nullptr )
				{
					nullItems.emplace_back( chainView );
				}
				else
				{
					_applyChainDataOnItem( *chainPtr, *chainView, category.getCategoryEnum() );
				}

				items.append( chainView );
			}

			p_categoryItem->addChildren( items );

			// setHidden Need to be called after adding items in view
			for ( QTreeWidgetItem * const nullItem : nullItems )
				nullItem->setHidden( true );
		}

		// Update visibility here because it can be modified when the parent is collapsed
		for ( QTreeWidgetItem * const item : *itemsPtr )
		{
			if ( item->isHidden() )
				continue;

			const App::Core::Model::ID &			 chainID = _getModelIDFromItem( *item );
			const App::Component::Chemistry::Chain & chain
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Chain>( chainID );

			const Qt::CheckState newCheckState = QT::Util::getCheckState( chain.isVisible() );
			item->setCheckState( 0, newCheckState );

			if ( p_forceExpandChildren || _getItemExpandState( *item ) )
			{
				item->setExpanded( true );
				_expandChain( item, p_forceExpandChildren );
			}
		}

		delete itemsPtr;

		_enableSignals( true );
	}
	void MoleculeSceneView::_expandChain( QTreeWidgetItem * const p_chainItem, const bool p_forceExpandChildren )
	{
		_enableSignals( false );

		QList<QTreeWidgetItem *> * itemsPtr;

		p_chainItem->setData( 0, EXPAND_STATE_ROLE, true );

		if ( p_chainItem->childCount() > 0 )
		{
			itemsPtr = new QList<QTreeWidgetItem *>();
			_fillListWithItemChildren( *p_chainItem, *itemsPtr );
		}
		else if ( _mapLoadedItems.find( p_chainItem ) != _mapLoadedItems.end() )
		{
			itemsPtr = _mapLoadedItems[ p_chainItem ];
			p_chainItem->addChildren( *itemsPtr );
			_mapLoadedItems.erase( p_chainItem );
		}
		else
		{
			itemsPtr								 = new QList<QTreeWidgetItem *>();
			QList<QTreeWidgetItem *> &	   items	 = *itemsPtr;
			std::vector<QTreeWidgetItem *> nullItems = std::vector<QTreeWidgetItem *>();

			const App::Core::Model::ID &			 chainId = _getModelIDFromItem( *p_chainItem );
			const App::Component::Chemistry::Chain & chain
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Chain>( chainId );
			const uint residueCount = chain.getResidueCount();
			nullItems.reserve( residueCount );
			items.reserve( residueCount );

			const uint indexFirstResidue = chain.getIndexFirstResidue();
			const uint indexLastResidue	 = chain.getIndexFirstResidue() + residueCount - 1;

			for ( uint i = chain.getIndexFirstResidue(); i < chain.getIndexFirstResidue() + residueCount; i++ )
			{
				const App::Component::Chemistry::Residue * const residuePtr	 = _model->getResidue( i );
				QTreeWidgetItem * const							 residueView = new QTreeWidgetItem();

				if ( residuePtr == nullptr )
				{
					nullItems.emplace_back( residueView );
				}
				else
				{
					_applyResidueDataOnItem( *residuePtr, *residueView );
				}

				items.append( residueView );
			}

			p_chainItem->addChildren( items );

			// Need to be called after adding items in view
			for ( QTreeWidgetItem * const nullItem : nullItems )
				nullItem->setHidden( true );
		}

		// Update visibility here because it can be modified when the parent is collapsed
		for ( QTreeWidgetItem * const item : *itemsPtr )
		{
			if ( item->isHidden() )
				continue;

			const App::Core::Model::ID &			   residueID = _getModelIDFromItem( *item );
			const App::Component::Chemistry::Residue & residue
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Residue>( residueID );

			const Qt::CheckState newCheckState = QT::Util::getCheckState( residue.isVisible() );
			item->setCheckState( 0, newCheckState );

			if ( p_forceExpandChildren || _getItemExpandState( *item ) )
			{
				item->setExpanded( true );
				_expandResidue( item, p_forceExpandChildren );
			}
		}

		delete ( itemsPtr );

		_enableSignals( true );
	}
	void MoleculeSceneView::_expandResidue( QTreeWidgetItem * const p_residueItem, const bool p_forceExpandChildren )
	{
		_enableSignals( false );

		QList<QTreeWidgetItem *> * itemsPtr;

		p_residueItem->setData( 0, EXPAND_STATE_ROLE, true );

		if ( p_residueItem->childCount() > 0 )
		{
			itemsPtr = new QList<QTreeWidgetItem *>();
			_fillListWithItemChildren( *p_residueItem, *itemsPtr );
		}
		else if ( _mapLoadedItems.find( p_residueItem ) != _mapLoadedItems.end() )
		{
			itemsPtr = _mapLoadedItems[ p_residueItem ];
			p_residueItem->addChildren( *itemsPtr );
			_mapLoadedItems.erase( p_residueItem );
		}
		else
		{
			itemsPtr								 = new QList<QTreeWidgetItem *>();
			QList<QTreeWidgetItem *> &	   items	 = *itemsPtr;
			std::vector<QTreeWidgetItem *> nullItems = std::vector<QTreeWidgetItem *>();

			const App::Core::Model::ID &			   residueId = _getModelIDFromItem( *p_residueItem );
			const App::Component::Chemistry::Residue & residue
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Residue>( residueId );
			const uint atomCount = residue.getAtomCount();
			nullItems.reserve( atomCount );
			items.reserve( atomCount );

			for ( uint i = residue.getIndexFirstAtom(); i < residue.getIndexFirstAtom() + atomCount; i++ )
			{
				const App::Component::Chemistry::Atom * const atomPtr  = _model->getAtom( i );
				QTreeWidgetItem * const						  atomView = new QTreeWidgetItem();

				if ( atomPtr == nullptr )
				{
					nullItems.emplace_back( atomView );
				}
				else
				{
					_applyAtomDataOnItem( *atomPtr, *atomView );
				}

				items.append( atomView );
			}

			p_residueItem->addChildren( items );

			// Need to be called after adding items in view
			for ( QTreeWidgetItem * const nullItem : nullItems )
				nullItem->setHidden( true );
		}

		for ( QTreeWidgetItem * const item : *itemsPtr )
		{
			if ( item->isHidden() )
				continue;

			const App::Core::Model::ID &			atomID = _getModelIDFromItem( *item );
			const App::Component::Chemistry::Atom & atom
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Atom>( atomID );

			const Qt::CheckState newCheckState = QT::Util::getCheckState( atom.isVisible() );
			item->setCheckState( 0, newCheckState );
		}

		_enableSignals( true );
	}

	void MoleculeSceneView::_collapseItem( QTreeWidgetItem & p_item )
	{
		_enableSignals( false );

		p_item.setData( 0, EXPAND_STATE_ROLE, false );

		QList<QTreeWidgetItem *> * const items = new QList( p_item.takeChildren() );
		_mapLoadedItems.emplace( &p_item, items );

		_enableSignals( true );
	}

	void MoleculeSceneView::_fillListWithItemChildren( const QTreeWidgetItem &	  p_parent,
													   QList<QTreeWidgetItem *> & p_list ) const
	{
		const uint childCount = p_parent.childCount();
		p_list.reserve( childCount );

		for ( uint i = 0; i < childCount; i++ )
			p_list.append( p_parent.child( i ) );
	}
	void MoleculeSceneView::_applyMoleculeDataOnItem( const App::Component::Chemistry::Molecule & p_molecule,
													  QTreeWidgetItem &							  p_item ) const
	{
		p_item.setData( 0, MODEL_ID_ROLE, QVariant::fromValue<VTX::App::Core::Model::ID>( p_molecule.getId() ) );
		p_item.setText( 0, QString::fromStdString( p_molecule.getDisplayName() ) );
		p_item.setIcon( 0, *VTX::UI::Style::IconConst::get().getModelSymbol( p_molecule.getTypeId() ) );

		const QTreeWidgetItem::ChildIndicatorPolicy childIndicatorPolicy
			= p_molecule.getChainCount() > 0 ? QTreeWidgetItem::ChildIndicatorPolicy::ShowIndicator
											 : QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicator;

		p_item.setChildIndicatorPolicy( childIndicatorPolicy );
	}
	void MoleculeSceneView::_applyCategoryDataOnItem( const App::Component::Chemistry::Category & p_category,
													  QTreeWidgetItem &							  p_item ) const
	{
		p_item.setData( 0, MODEL_ID_ROLE, QVariant::fromValue( p_category.getId() ) );
		p_item.setData( 0, CATEGORY_ROLE, QVariant::fromValue( int( p_category.getCategoryEnum() ) ) );
		p_item.setText( 0, QString::fromStdString( p_category.getName() ) );
		p_item.setIcon( 0, *VTX::UI::Style::IconConst::get().getResidueCategorySymbol( p_category.getCategoryEnum() ) );

		// Always show indicator, if the category has no child, it is remove from the molecule
		p_item.setChildIndicatorPolicy( QTreeWidgetItem::ChildIndicatorPolicy::ShowIndicator );
	}
	void MoleculeSceneView::_applyChainDataOnItem( const App::Component::Chemistry::Chain &	   p_chain,
												   QTreeWidgetItem &						   p_item,
												   const App::Internal::ChemDB::Category::TYPE p_category ) const
	{
		p_item.setData( 0, MODEL_ID_ROLE, QVariant::fromValue( p_chain.getId() ) );
		p_item.setData( 0, CATEGORY_ROLE, QVariant::fromValue( int( p_category ) ) );
		p_item.setText( 0, QString::fromStdString( p_chain.getDefaultName() ) );
		p_item.setIcon( 0, *VTX::UI::Style::IconConst::get().getModelSymbol( p_chain.getTypeId() ) );

		// Always show indicator, if chain has no child, it is remove from the molecule
		p_item.setChildIndicatorPolicy( QTreeWidgetItem::ChildIndicatorPolicy::ShowIndicator );
	}
	void MoleculeSceneView::_applyResidueDataOnItem( const App::Component::Chemistry::Residue & p_residue,
													 QTreeWidgetItem &							p_item ) const
	{
		p_item.setData( 0, MODEL_ID_ROLE, QVariant::fromValue( p_residue.getId() ) );
		_applyResidueNameOnItem( p_residue, p_item, VTX_SETTING().getSymbolDisplayMode() );
		p_item.setIcon( 0, *VTX::UI::Style::IconConst::get().getModelSymbol( p_residue.getTypeId() ) );

		// Always show indicator, if residue has no child, it is remove from the molecule
		p_item.setChildIndicatorPolicy( QTreeWidgetItem::ChildIndicatorPolicy::ShowIndicator );
	}
	void MoleculeSceneView::_applyAtomDataOnItem( const App::Component::Chemistry::Atom & p_atom,
												  QTreeWidgetItem &						  p_item ) const
	{
		p_item.setData( 0, MODEL_ID_ROLE, QVariant::fromValue( p_atom.getId() ) );
		p_item.setText( 0,
						QString::fromStdString( p_atom.getSymbolStr() + " " + std::to_string( p_atom.getIndex() ) ) );
		p_item.setIcon( 0, *VTX::UI::Style::IconConst::get().getModelSymbol( p_atom.getTypeId() ) );
		p_item.setChildIndicatorPolicy( QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicator );
	}

	void MoleculeSceneView::_applyResidueNameOnItem(
		const App::Component::Chemistry::Residue &					p_residue,
		QTreeWidgetItem &											p_item,
		const App::Internal::ChemDB::Residue::SYMBOL_DISPLAY_MODE & p_symbolDisplayMode ) const
	{
		const std::string * text;
		switch ( p_symbolDisplayMode )
		{
		case App::Internal::ChemDB::Residue::SYMBOL_DISPLAY_MODE::SHORT: text = &p_residue.getSymbolStr(); break;
		case App::Internal::ChemDB::Residue::SYMBOL_DISPLAY_MODE::LONG: text = &p_residue.getSymbolName(); break;
		default:
			VTX_WARNING( "Symbol style " + std::to_string( int( p_symbolDisplayMode ) )
						 + " not managed in MoleculeSceneView::_applyResidueNameOnItem." );

			text = &p_residue.getSymbolStr();
			break;
		}

		p_item.setText( 0,
						QString::fromStdString( *text + " " + std::to_string( p_residue.getIndexInOriginalChain() ) ) );
	}

	void MoleculeSceneView::_doEnableStateChangeAction( const QTreeWidgetItem * const p_item ) const
	{
		const App::Core::Model::ID & modelId	  = _getModelIDFromItem( *p_item );
		const App::VTX_ID &			 modelTypeId  = VTX::MVC_MANAGER().getModelTypeID( modelId );
		const bool					 modelEnabled = p_item->checkState( 0 ) == Qt::CheckState::Checked ? true : false;

		const App::Application::Selection::SelectionModel & selection
			= App::Application::Selection::SelectionManager::get().getSelectionModel();

		const VTX::App::Action::VISIBILITY_MODE visibilityMode
			= modelEnabled ? VTX::App::Action::VISIBILITY_MODE::SHOW : VTX::App::Action::VISIBILITY_MODE::HIDE;

		if ( modelTypeId == App::ID::Model::MODEL_MOLECULE )
		{
			App::Component::Chemistry::Molecule & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( modelId );

			if ( selection.isMoleculeFullySelected( model ) )
			{
				VTX_ACTION( new VTX::App::Action::Selection::ChangeVisibility(
					selection, model, modelTypeId, visibilityMode ) );
			}
			else
			{
				VTX_ACTION( new VTX::App::Action::Molecule::ChangeVisibility( model, visibilityMode ) );
			}
		}
		else if ( modelTypeId == App::ID::Model::MODEL_CATEGORY )
		{
			App::Component::Chemistry::Category & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Category>( modelId );

			if ( selection.isCategoryFullySelected( model ) )
			{
				VTX_ACTION( new VTX::App::Action::Selection::ChangeVisibility(
					selection, model, modelTypeId, visibilityMode ) );
			}
			else
			{
				VTX_ACTION( new VTX::App::Action::Category::ChangeVisibility( model, visibilityMode ) );
			}
		}
		else if ( modelTypeId == App::ID::Model::MODEL_CHAIN )
		{
			App::Component::Chemistry::Chain & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Chain>( modelId );

			if ( selection.isChainFullySelected( model ) )
			{
				VTX_ACTION( new VTX::App::Action::Selection::ChangeVisibility(
					selection, model, modelTypeId, visibilityMode ) );
			}
			else
			{
				VTX_ACTION( new VTX::App::Action::Chain::ChangeVisibility( model, visibilityMode ) );
			}
		}
		else if ( modelTypeId == App::ID::Model::MODEL_RESIDUE )
		{
			App::Component::Chemistry::Residue & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Residue>( modelId );

			if ( selection.isResidueFullySelected( model ) )
			{
				VTX_ACTION( new VTX::App::Action::Selection::ChangeVisibility(
					selection, model, modelTypeId, visibilityMode ) );
			}
			else
			{
				VTX_ACTION( new VTX::App::Action::Residue::ChangeVisibility( model, visibilityMode ) );
			}
		}
		else if ( modelTypeId == App::ID::Model::MODEL_ATOM )
		{
			App::Component::Chemistry::Atom & model
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Atom>( modelId );

			if ( selection.isAtomSelected( model ) )
			{
				VTX_ACTION( new VTX::App::Action::Selection::ChangeVisibility(
					selection, model, modelTypeId, visibilityMode ) );
			}
			else
			{
				VTX_ACTION( new VTX::App::Action::Atom::ChangeVisibility( model, visibilityMode ) );
			}
		}
	}
	void MoleculeSceneView::_reformatMoleculeName( std::string & p_moleculeName ) const
	{
		// Trim
		VTX::Util::String::trim( p_moleculeName );

		// Forbid line feed
		const size_t firstLinefeedIndex = p_moleculeName.find_first_of( '\n' );
		if ( firstLinefeedIndex != std::string::npos )
			p_moleculeName.erase( firstLinefeedIndex );

		// Clamp size
		if ( p_moleculeName.size() > Style::MOLECULE_NAME_MAXIMUM_SIZE )
			p_moleculeName = p_moleculeName.substr( 0, Style::MOLECULE_NAME_MAXIMUM_SIZE );
	}

	void MoleculeSceneView::_fillItemSelection( const App::Application::Selection::SelectionModel & p_selection,
												QItemSelection &									p_itemSelection )
	{
		const App::Application::Selection::SelectionModel::MapMoleculeIds & items = p_selection.getMoleculesMap();
		const App::Application::Selection::SelectionModel::MapMoleculeIds::const_iterator itMoleculeItem
			= items.find( _model->getId() );

		if ( itMoleculeItem != items.end() )
		{
			QTreeWidgetItem * const moleculeItem = _getMoleculeTreeWidgetItem();
			p_itemSelection.append( QItemSelectionRange( indexFromItem( moleculeItem ) ) );

			if ( moleculeItem->isExpanded() )
			{
				const App::Component::Chemistry::Category * topCategory				= nullptr;
				const App::Component::Chemistry::Category * previousCategory		= nullptr;
				QModelIndex									topCategoryItemIndex	= QModelIndex();
				QModelIndex									bottomCategoryItemIndex = QModelIndex();

				for ( const App::Component::Chemistry::Category * const category : _model->getCategories() )
				{
					if ( category->isEmpty() )
					{
						continue;
					}

					QTreeWidgetItem * const categoryItem = moleculeItem->child( int( category->getCategoryEnum() ) );

					if ( p_selection.isCategorySelected( *category ) )
					{
						if ( topCategoryItemIndex.isValid() )
						{
							// if not contiguous, add new range
							if ( int( category->getCategoryEnum() )
								 != ( int( previousCategory->getCategoryEnum() ) + 1 ) )
							{
								p_itemSelection.append(
									QItemSelectionRange( topCategoryItemIndex, bottomCategoryItemIndex ) );
								topCategoryItemIndex = indexFromItem( categoryItem );
								topCategory			 = previousCategory;
							}

							bottomCategoryItemIndex = indexFromItem( categoryItem );
						}
						else
						{
							topCategoryItemIndex	= indexFromItem( categoryItem );
							topCategory				= category;
							bottomCategoryItemIndex = indexFromItem( categoryItem );
						}

						previousCategory = category;
					}
					else
					{
						if ( topCategoryItemIndex.isValid() )
						{
							p_itemSelection.append(
								QItemSelectionRange( topCategoryItemIndex, bottomCategoryItemIndex ) );
						}
					}

					if ( !categoryItem->isExpanded() )
						continue;

					const App::Component::Chemistry::Chain * topChain			  = nullptr;
					const App::Component::Chemistry::Chain * previousChain		  = nullptr;
					QModelIndex								 topChainItemIndex	  = QModelIndex();
					QModelIndex								 bottomChainItemIndex = QModelIndex();

					for ( const App::Application::Selection::SelectionModel::PairChainIds & pairChain :
						  itMoleculeItem->second )
					{
						const App::Component::Chemistry::Chain & chain = *_model->getChain( pairChain.first );
						const int chainItemIndex = category->getChainInnerIndex( chain.getIndex() );

						// TODO Better management of chains not in current category
						if ( chainItemIndex == -1 )
							continue;

						QTreeWidgetItem * const chainItem = categoryItem->child( chainItemIndex );

						if ( topChainItemIndex.isValid() )
						{
							// if not contiguous, add new range
							if ( chain.getIndex() != uint( previousChain->getIndex() + 1 ) )
							{
								p_itemSelection.append(
									QItemSelectionRange( topChainItemIndex, bottomChainItemIndex ) );
								topChainItemIndex = indexFromItem( chainItem );
								topChain		  = &chain;
							}

							bottomChainItemIndex = indexFromItem( chainItem );
						}
						else
						{
							topChainItemIndex	 = indexFromItem( chainItem );
							topChain			 = &chain;
							bottomChainItemIndex = indexFromItem( chainItem );
						}

						previousChain = &chain;

						if ( !chainItem->isExpanded() )
							continue;

						const App::Component::Chemistry::Residue * topResidue			  = nullptr;
						const App::Component::Chemistry::Residue * previousResidue		  = nullptr;
						QModelIndex								   topResidueItemIndex	  = QModelIndex();
						QModelIndex								   bottomResidueItemIndex = QModelIndex();
						for ( const App::Application::Selection::SelectionModel::PairResidueIds & pairResidue :
							  pairChain.second )
						{
							const App::Component::Chemistry::Residue & residue
								= *_model->getResidue( pairResidue.first );
							QTreeWidgetItem * const residueItem
								= chainItem->child( residue.getIndex() - chain.getIndexFirstResidue() );

							if ( topResidueItemIndex.isValid() )
							{
								// if not contiguous, add new range
								if ( residue.getIndex() != uint( previousResidue->getIndex() + 1 ) )
								{
									p_itemSelection.append(
										QItemSelectionRange( topResidueItemIndex, bottomResidueItemIndex ) );
									topResidueItemIndex = indexFromItem( residueItem );
									topResidue			= &residue;
								}

								bottomResidueItemIndex = indexFromItem( residueItem );
							}
							else
							{
								topResidueItemIndex	   = indexFromItem( residueItem );
								topResidue			   = &residue;
								bottomResidueItemIndex = indexFromItem( residueItem );
							}

							previousResidue = &residue;

							if ( !residueItem->isExpanded() )
								continue;

							const App::Component::Chemistry::Atom * topAtom				= nullptr;
							const App::Component::Chemistry::Atom * previousAtom		= nullptr;
							QModelIndex								topAtomItemIndex	= QModelIndex();
							QModelIndex								bottomAtomItemIndex = QModelIndex();
							for ( const uint & atomId : pairResidue.second )
							{
								const App::Component::Chemistry::Atom & atom = *_model->getAtom( atomId );
								QTreeWidgetItem * const					atomItem
									= residueItem->child( atomId - residue.getIndexFirstAtom() );

								if ( topAtomItemIndex.isValid() )
								{
									// if not contiguous, add new range
									if ( atom.getIndex() != uint( previousAtom->getIndex() + 1 ) )
									{
										p_itemSelection.append(
											QItemSelectionRange( topAtomItemIndex, bottomAtomItemIndex ) );
										topAtom			 = &atom;
										topAtomItemIndex = indexFromItem( atomItem );
									}

									bottomAtomItemIndex = indexFromItem( atomItem );
								}
								else
								{
									topAtomItemIndex	= indexFromItem( atomItem );
									topAtom				= &atom;
									bottomAtomItemIndex = indexFromItem( atomItem );
								}

								previousAtom = &atom;
							}
							if ( topAtomItemIndex.isValid() )
							{
								p_itemSelection.append( QItemSelectionRange( topAtomItemIndex, bottomAtomItemIndex ) );
							}
						}

						if ( topResidueItemIndex.isValid() )
						{
							p_itemSelection.append(
								QItemSelectionRange( topResidueItemIndex, bottomResidueItemIndex ) );
						}
					}

					if ( topChainItemIndex.isValid() )
					{
						p_itemSelection.append( QItemSelectionRange( topChainItemIndex, bottomChainItemIndex ) );
					}
				}

				if ( topCategoryItemIndex.isValid() )
				{
					p_itemSelection.append( QItemSelectionRange( topCategoryItemIndex, bottomCategoryItemIndex ) );
				}
			}
		}
	}

	void MoleculeSceneView::_refreshSymbolDisplay(
		const App::Internal::ChemDB::Residue::SYMBOL_DISPLAY_MODE & p_displayMode )
	{
		_refreshSymbolDisplayRecursive( _getMoleculeTreeWidgetItem(), p_displayMode );
		_clearLoadedItems();
	}
	void MoleculeSceneView::_refreshSymbolDisplayRecursive(
		QTreeWidgetItem * const										p_item,
		const App::Internal::ChemDB::Residue::SYMBOL_DISPLAY_MODE & p_displayMode )
	{
		const App::Core::Model::ID & modelId	 = _getModelIDFromItem( *p_item );
		const App::VTX_ID &			 modelTypeId = VTX::MVC_MANAGER().getModelTypeID( modelId );

		if ( modelTypeId == App::ID::Model::MODEL_CHAIN )
		{
			for ( int i = 0; i < p_item->childCount(); i++ )
			{
				QTreeWidgetItem * const					   child   = p_item->child( i );
				const App::Core::Model::ID &			   modelId = _getModelIDFromItem( *child );
				const App::Component::Chemistry::Residue & residue
					= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Residue>( modelId );

				_applyResidueNameOnItem( residue, *child, p_displayMode );
			}
		}
		else
		{
			for ( int i = 0; i < p_item->childCount(); i++ )
			{
				_refreshSymbolDisplayRecursive( p_item->child( i ), p_displayMode );
			}
		}
	}

	QTreeWidgetItem * const MoleculeSceneView::_getMoleculeTreeWidgetItem() const { return topLevelItem( 0 ); }

	QTreeWidgetItem * const MoleculeSceneView::_getTreeWidgetItem(
		const App::Component::Chemistry::Category & p_category ) const
	{
		return topLevelItem( 0 )->child( int( p_category.getCategoryEnum() ) );
	}
	QTreeWidgetItem * const MoleculeSceneView::_getTreeWidgetItem(
		const App::Component::Chemistry::Chain & p_chain ) const
	{
		const App::Component::Chemistry::Category * const category = _model->getCategoryFromChain( p_chain );
		const int childIndex									   = category->getChainInnerIndex( p_chain.getIndex() );

		return _getTreeWidgetItem( *category )->child( childIndex );
	}
	QTreeWidgetItem * const MoleculeSceneView::_getTreeWidgetItem(
		const App::Component::Chemistry::Residue & p_residue ) const
	{
		const App::Component::Chemistry::Chain * const chain = p_residue.getChainPtr();
		const uint childIndex								 = p_residue.getIndex() - chain->getIndexFirstResidue();

		return _getTreeWidgetItem( *chain )->child( childIndex );
	}
	QTreeWidgetItem * const MoleculeSceneView::_getTreeWidgetItem(
		const App::Component::Chemistry::Atom & p_atom ) const
	{
		const App::Component::Chemistry::Residue * const residue	= p_atom.getResiduePtr();
		const uint										 childIndex = p_atom.getIndex() - residue->getIndexFirstAtom();

		return _getTreeWidgetItem( *residue )->child( childIndex );
	}

	bool MoleculeSceneView::_isMoleculeExpanded() const { return _getMoleculeTreeWidgetItem()->childCount() > 0; }
	bool MoleculeSceneView::_isCategoryExpanded( const App::Component::Chemistry::Category & p_category ) const
	{
		return _isMoleculeExpanded() && _getTreeWidgetItem( p_category )->childCount() > 0;
	}
	bool MoleculeSceneView::_isChainExpanded( const App::Component::Chemistry::Chain & p_chain ) const
	{
		const App::Component::Chemistry::Category * const category = _model->getCategoryFromChain( p_chain );
		return _isCategoryExpanded( *category ) && _getTreeWidgetItem( p_chain )->childCount() > 0;
	}
	bool MoleculeSceneView::_isResidueExpanded( const App::Component::Chemistry::Residue & p_residue ) const
	{
		const App::Component::Chemistry::Chain * const chain = p_residue.getChainPtr();
		return _isChainExpanded( *chain ) && _getTreeWidgetItem( p_residue )->childCount() > 0;
	}

	bool MoleculeSceneView::_canDragAtPos( const QPoint & p_position ) const
	{
		// Can only drag from the molecule
		return itemAt( p_position ) == _getMoleculeTreeWidgetItem();
	}

	bool MoleculeSceneView::_itemCanBeRenamed( const QTreeWidgetItem * p_item )
	{
		const App::Application::Selection::SelectionModel & selection
			= VTX::App::Application::Selection::SelectionManager::get().getSelectionModel();
		return p_item == _getMoleculeTreeWidgetItem() && selection.isMoleculeFullySelected( *_model );
	}

	App::Internal::ChemDB::Category::TYPE MoleculeSceneView::_getCategoryFromItem(
		const QTreeWidgetItem & p_item ) const
	{
		const QVariant & dataID = p_item.data( 0, CATEGORY_ROLE );
		return App::Internal::ChemDB::Category::TYPE( dataID.value<int>() );
	}

	void MoleculeSceneView::_selectAllCategoriesFrom( std::vector<App::Core::Model::ID> &		  p_selection,
													  const App::Component::Chemistry::Category & p_itemFrom ) const
	{
		const std::vector<App::Component::Chemistry::Category *> & categories = _model->getCategories();

		for ( int iCategory = int( p_itemFrom.getCategoryEnum() ) + 1; iCategory < categories.size(); iCategory++ )
		{
			p_selection.emplace_back(
				_model->getCategory( App::Internal::ChemDB::Category::TYPE( iCategory ) ).getId() );
		}
	}
	void MoleculeSceneView::_selectAllChainsFrom( std::vector<App::Core::Model::ID> &	   p_selection,
												  const App::Component::Chemistry::Chain & p_itemFrom ) const
	{
		for ( uint iChain = p_itemFrom.getIndex() + 1; iChain < _model->getChainCount(); iChain++ )
			p_selection.emplace_back( _model->getChain( iChain )->getId() );
	}
	void MoleculeSceneView::_selectAllResiduesFrom( std::vector<App::Core::Model::ID> &		   p_selection,
													const App::Component::Chemistry::Residue & p_itemFrom ) const
	{
		const App::Component::Chemistry::Chain & chain = *( p_itemFrom.getChainPtr() );

		for ( uint iResidue = p_itemFrom.getIndex() + 1; iResidue <= chain.getIndexLastResidue(); iResidue++ )
		{
			p_selection.emplace_back( _model->getResidue( iResidue )->getId() );
		}
	}
	void MoleculeSceneView::_selectAllAtomsFrom( std::vector<App::Core::Model::ID> &	 p_selection,
												 const App::Component::Chemistry::Atom & p_itemFrom ) const
	{
		const App::Component::Chemistry::Residue & residue = *( p_itemFrom.getResiduePtr() );

		for ( uint iAtom = p_itemFrom.getIndex() + 1; iAtom < residue.getIndexFirstAtom() + residue.getAtomCount();
			  iAtom++ )
		{
			p_selection.emplace_back( _model->getAtom( iAtom )->getId() );
		}
	}

	void MoleculeSceneView::_selectAllCategoriesTo( std::vector<App::Core::Model::ID> &			p_selection,
													const App::Component::Chemistry::Category & p_itemFrom ) const
	{
		const std::vector<App::Component::Chemistry::Category *> & categories = _model->getCategories();

		for ( int iCategory = 0; iCategory < int( p_itemFrom.getCategoryEnum() ); iCategory++ )
		{
			p_selection.emplace_back(
				_model->getCategory( App::Internal::ChemDB::Category::TYPE( iCategory ) ).getId() );
		}
	}
	void MoleculeSceneView::_selectAllChainsTo( std::vector<App::Core::Model::ID> &		 p_selection,
												const App::Component::Chemistry::Chain & p_itemFrom ) const
	{
		for ( uint iChain = 0; iChain < p_itemFrom.getIndex(); iChain++ )
			p_selection.emplace_back( _model->getChain( iChain )->getId() );
	}
	void MoleculeSceneView::_selectAllResiduesTo( std::vector<App::Core::Model::ID> &		 p_selection,
												  const App::Component::Chemistry::Residue & p_itemFrom ) const
	{
		const App::Component::Chemistry::Chain & chain = *( p_itemFrom.getChainPtr() );

		for ( uint iResidue = chain.getIndexFirstResidue(); iResidue < p_itemFrom.getIndex(); iResidue++ )
			p_selection.emplace_back( _model->getResidue( iResidue )->getId() );
	}
	void MoleculeSceneView::_selectAllAtomsTo( std::vector<App::Core::Model::ID> &	   p_selection,
											   const App::Component::Chemistry::Atom & p_itemFrom ) const
	{
		const App::Component::Chemistry::Residue & residue = *( p_itemFrom.getResiduePtr() );

		for ( uint iAtom = residue.getIndexFirstAtom(); iAtom < p_itemFrom.getIndex(); iAtom++ )
			p_selection.emplace_back( _model->getAtom( iAtom )->getId() );
	}

} // namespace VTX::UI::QT::Tool::Scene::Widget::View
