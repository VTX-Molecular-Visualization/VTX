#include "ui/qt/dock_widget/scene.hpp"
#include <QAbstractItemModel>
#include <QHeaderView>
#include <app/action/animation.hpp>
#include <app/action/selection.hpp>
#include <app/action/visibility.hpp>
#include <app/application/scene.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/component/scene/scene_item_component.hpp>
#include <app/selection/system_data.hpp>
#include <unordered_set>
#include <util/string.hpp>

namespace VTX::UI::QT::DockWidget
{

	Scene::Scene( QWidget * p_parent ) : Core::BaseDockWidget<Scene>( "Scene", p_parent )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		// trajectories
		// add an horizontal line as separator
		auto * topLineSeparator = new QFrame( this );
		topLineSeparator->setFrameShape( QFrame::HLine );
		topLineSeparator->setFrameShadow( QFrame::Plain );
		_layout->addWidget( topLineSeparator );

		//auto * trajManager = new Widget::TrajectoryManager( this );
		//_layout->addWidget( trajManager );

		// add an horizontal line as separator
		auto * bottomLineSeparator = new QFrame( this );
		bottomLineSeparator->setFrameShape( QFrame::HLine );
		bottomLineSeparator->setFrameShadow( QFrame::Plain );
		_layout->addWidget( bottomLineSeparator );

		// Search bar.
		/*
		auto * searchBar = new QLineEdit( _root );
		searchBar->setPlaceholderText( "Search..." );
		_layout->addWidget( searchBar );
		*/

		// Toolbar.
		// auto * toolbar = new QToolBar( widget );
		// layout->addWidget( toolbar );
		// toolbar->addAction( "A" );
		// toolbar->addAction( "O" );

		// Setup tree.
		_tree = new QTreeWidget( this );

		_tree->setColumnCount( 2 );
		//_tree->setUniformRowHeights( true );
		_tree->setHeaderHidden( true );
		_tree->setSelectionMode( QAbstractItemView::ExtendedSelection );
		//_tree->setSelectionBehavior( QAbstractItemView::SelectionBehavior::SelectRows );

		_tree->setColumnWidth( 1, 20 );
		_tree->header()->setSectionResizeMode( 0, QHeaderView::ResizeToContents );
		_tree->header()->setSectionResizeMode( 1, QHeaderView::Fixed );

		//_tree->header()->setSectionResizeMode( 1, QHeaderView::ResizeToContents );

		// auto * model = new QItemSelectionModel();
		//_tree->setSelectionModel( model );

		// QItemSelectionModel * selectionModel = _tree->selectionModel();

		//_tree->setSizeAdjustPolicy( QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents );
		//_tree->setSizePolicy( QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Minimum );
		//_tree->setHorizontalScrollBarPolicy( Qt::ScrollBarPolicy::ScrollBarAlwaysOff );
		//_tree->setVerticalScrollBarPolicy( Qt::ScrollBarPolicy::ScrollBarAlwaysOff );
		_tree->setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );
		//_tree->setEditTriggers( EditTrigger::SelectedClicked );
		_tree->setExpandsOnDoubleClick( false );

		// itemExpanded.
		_tree->connect(
			_tree.get(),
			&QTreeWidget::itemExpanded,
			this,
			[ & ]( QTreeWidgetItem * const p_item )
			{
				// Get item depth.
				const auto & [ depth, topLevelItem ] = _getDepth( p_item );

				assert( _loadFuncs.contains( topLevelItem ) );

				// Remove placeholder.
				assert( p_item->childCount() == 1 );
				delete p_item->takeChild( 0 );

				// Load children.
				_loadFuncs.at( topLevelItem )( E_DEPTH( depth ), p_item );
			}
		);

		// itemCollapsed.
		_tree->connect(
			_tree.get(),
			&QTreeWidget::itemCollapsed,
			this,
			[ this ]( QTreeWidgetItem * const p_item )
			{
				_tree->blockSignals( true );
				_resetTreeItem( p_item );
				_tree->blockSignals( false );
			}
		);

		// Connect callbacks.

		// customContextMenuRequested.
		_tree->connect( _tree.get(), &QTreeWidget::customContextMenuRequested, this, []( const QPoint & p_pos ) {} );

		// onSceneItemAdded.
		using namespace App::Component::Scene;
		App::SCENE().onSceneItemAdded += [ this ]( const SceneItemComponent & p_system )
		{
			using namespace App;

			if ( App::ECS_REGISTRY().hasComponent<App::Component::Chemistry::System>( p_system ) )
			{
				auto & system = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::System>( p_system );

				// Add top level item.
				_addTreeItem(
					{ p_system.getName(), WidgetData( p_system.getPersistentSceneID() ), system.getChains().size() },
					nullptr,
					[ this, &system ]( const E_DEPTH p_depth, QTreeWidgetItem * const p_item )
					{
						_tree->blockSignals( true );

						WidgetData parentWidgetData = p_item->data( 0, Qt::UserRole ).value<WidgetData>();

						switch ( p_depth )
						{
						case E_DEPTH::SYSTEM: // Load chains.
						{
							WidgetData index = 0;
							for ( auto & chain : system.getChains() )
							{
								_addTreeItem(
									{ chain->getName(),
									  index++,
									  chain->getResidueCount(),
									  chain->isFullyVisible() ? E_VISIBILITY::VISIBLE
									  : chain->isVisible()	  ? E_VISIBILITY::PARTIAL
															  : E_VISIBILITY::HIDDEN },
									p_item
								);
							}
						}
						break;

						case E_DEPTH::CHAIN: // Load residues.
						{
							auto * chain = system.getChain( parentWidgetData );
							assert( chain );
							for ( size_t index = chain->getIndexFirstResidue(); index <= chain->getIndexLastResidue();
								  ++index )
							{
								auto * residue = system.getResidue( index );
								_addTreeItem(
									{ residue->getName(),
									  index,
									  residue->getAtomCount(),
									  residue->isFullyVisible() ? E_VISIBILITY::VISIBLE
									  : residue->isVisible()	? E_VISIBILITY::PARTIAL
																: E_VISIBILITY::HIDDEN },
									p_item
								);
							}
						}
						break;

						case E_DEPTH::RESIDUE: // Load atoms.
						{
							auto * residue = system.getResidue( parentWidgetData );
							assert( residue );
							for ( size_t index = residue->getIndexFirstAtom(); index <= residue->getIndexLastAtom();
								  ++index )
							{
								auto * atom = system.getAtom( atom_index_t( index ) );
								_addTreeItem(
									{ atom->getName(),
									  index,
									  0,
									  atom->isVisible() ? E_VISIBILITY::VISIBLE : E_VISIBILITY::HIDDEN },
									p_item
								);
							}
						}
						break;

						default: assert( true ); break;
						}

						_tree->blockSignals( false );
					},
					&system
				);
			}
		};

		// itemClicked.
		connect(
			_tree,
			&QTreeWidget::itemClicked,
			this,
			[ this ]( QTreeWidgetItem * const p_item, const int p_column )
			{
				WidgetData widgetData				   = p_item->data( 0, Qt::UserRole ).value<WidgetData>();
				const auto & [ depth, topLevelWidget ] = _getDepth( p_item );

				assert( _systemComponents.contains( topLevelWidget ) );

				// Selection.
				if ( p_column == 0 )
				{
					using namespace App;
					using namespace App::Action::Selection;
					using namespace App::Selection;

					// Get selectable component.
					const auto & system		= *_systemComponents[ topLevelWidget ];
					const auto & selectable = ECS_REGISTRY().getComponent<Component::Scene::Selectable>( system );

					// Selectable data.
					SystemData selectionData( selectable );
					selectionData.clear();

					switch ( depth )
					{
					case E_DEPTH::SYSTEM:
					{
						selectionData.selectAll();
						break;
					}
					case E_DEPTH::CHAIN:
					{
						selectionData.selectFullChain( *system.getChain( widgetData ) );
						break;
					}
					case E_DEPTH::RESIDUE:
					{
						selectionData.selectFullResidue( *system.getResidue( widgetData ) );
						break;
					}
					case E_DEPTH::ATOM:
					{
						selectionData.selectAtom( *system.getAtom( atom_index_t( widgetData ) ) );
						break;
					}
					default: assert( true ); break;
					}

					App::ACTION_SYSTEM().execute<Select>( selectionData, AssignmentType::SET );
				}
				// Visibility
				else if ( p_column == 1 )
				{
					// Select.
					E_VISIBILITY visibility = p_item->data( 1, Qt::UserRole ).value<E_VISIBILITY>();
					bool		 isVisible	= visibility == E_VISIBILITY::VISIBLE ? 0 : 1;

					using namespace App::Action::Visibility;
					switch ( depth )
					{
					case E_DEPTH::SYSTEM:
					{
						App::ACTION_SYSTEM().execute<SetVisible<E_ITEM_TYPE::SYSTEM>>(
							_systemComponents.at( topLevelWidget ), isVisible, std::nullopt
						);
						break;
					}
					case E_DEPTH::CHAIN:
					{
						App::ACTION_SYSTEM().execute<SetVisible<E_ITEM_TYPE::CHAIN>>(
							_systemComponents.at( topLevelWidget ), isVisible, widgetData
						);
						break;
					}
					case E_DEPTH::RESIDUE:
					{
						App::ACTION_SYSTEM().execute<SetVisible<E_ITEM_TYPE::RESIDUE>>(
							_systemComponents.at( topLevelWidget ), isVisible, widgetData
						);
						break;
					}
					case E_DEPTH::ATOM:
					{
						App::ACTION_SYSTEM().execute<SetVisible<E_ITEM_TYPE::ATOM>>(
							_systemComponents.at( topLevelWidget ), isVisible, widgetData
						);
						break;
					}
					default: assert( true ); break;
					}
				}
				else
				{
					assert( true );
				}
			}
		);

		// itemDoubleClicked.
		connect(
			_tree,
			&QTreeWidget::itemDoubleClicked,
			this,
			[ this ]( QTreeWidgetItem * const p_item, const int p_column )
			{
				if ( p_column != 0 )
				{
					return;
				}

				WidgetData widgetData				   = p_item->data( 0, Qt::UserRole ).value<WidgetData>();
				const auto & [ depth, topLevelWidget ] = _getDepth( p_item );

				assert( _systemComponents.contains( topLevelWidget ) );

				using namespace App::Action::Animation;
				switch ( depth )
				{
				case E_DEPTH::SYSTEM:
				{
					// App::ACTION_SYSTEM().execute<Orient>( _systemComponents.at( topLevelWidget )->getAABB() );
					break;
				}
				case E_DEPTH::CHAIN:
				{
					break;
				}
				case E_DEPTH::RESIDUE:
				{
					break;
				}
				case E_DEPTH::ATOM:
				{
					break;
				}
				default: assert( true ); break;
				}
			}
		);

		_layout->addWidget( _tree.get() );
	}

	std::pair<Scene::E_DEPTH, QTreeWidgetItem * const> Scene::_getDepth( QTreeWidgetItem * const p_item ) const
	{
		uint			  depth	 = 1;
		QTreeWidgetItem * parent = p_item;
		while ( parent->parent() )
		{
			++depth;
			parent = parent->parent();
		}
		return { E_DEPTH( depth ), parent };
	}

	void Scene::_addTreeItem(
		const TreeItemData &							   p_data,
		QTreeWidgetItem * const							   p_parent,
		std::optional<const LoadFunc>					   p_loadFunc,
		std::optional<App::Component::Chemistry::System *> p_systemComponent
	)
	{
		_tree->blockSignals( true );

		QTreeWidgetItem * item	= new QTreeWidgetItem( p_parent );
		Qt::ItemFlags	  flags = Qt::ItemFlag::ItemIsSelectable | Qt::ItemIsEnabled;

		item->setFlags( flags );
		item->setData( 0, Qt::UserRole, QVariant::fromValue( p_data.data ) );
		// item->setText( 0, QString::fromUtf8( p_data.name ) );
		item->setText( 0, QString::fromStdString( p_data.name.data() ).append( " (%1)" ).arg( p_data.childrenCount ) );
		// item->setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( model.getTypeId() ) );
		// item->setCheckState( 1, Qt::Checked );
		item->setTextAlignment( 1, Qt::AlignRight );

		if ( p_data.childrenCount != 0 )
		{
			_resetTreeItem( item );
		}

		// Add top level item with data load function.
		if ( not p_parent )
		{
			assert( p_loadFunc.has_value() );
			assert( p_systemComponent.has_value() );

			assert( not _loadFuncs.contains( item ) );
			assert( not _systemComponents.contains( item ) );

			App::Component::Chemistry::System * const system = p_systemComponent.value();

			_loadFuncs.emplace( item, p_loadFunc.value() );
			_systemComponents.emplace( item, system );

			_tree->addTopLevelItem( item );

			// Connect selection callback.
			auto & selectable = App::ECS_REGISTRY().getComponent<App::Component::Scene::Selectable>( *system );
			selectable.onSelect += []( const App::Selection::SelectionData & p_selectionData )
			{
				const App::Selection::SystemData & castedSelectionData
					= dynamic_cast<const App::Selection::SystemData &>( p_selectionData );
			};

			// Default visibility.
			_applyVisibility( E_VISIBILITY::VISIBLE, item );

			// Visibility callback.
			system->onVisibilityChange +=
				[ this,
				  item,
				  system ]( App::Component::Chemistry::AtomIndexRangeList, App::Core::VISIBILITY_APPLY_MODE )
			{
				Util::Chrono chrono;
				chrono.start();

				_applyVisibility(
					system->isFullyVisible() ? E_VISIBILITY::VISIBLE
					: system->isVisible()	 ? E_VISIBILITY::PARTIAL
											 : E_VISIBILITY::HIDDEN,
					item
				);

				// Loop over children.
				for ( int i = 0; i < item->childCount(); ++i )
				{
					QTreeWidgetItem * const chainWidget = item->child( i );
					WidgetData				chainId		= chainWidget->data( 0, Qt::UserRole ).value<WidgetData>();
					const auto * const		chain		= system->getChain( chainId );

					_applyVisibility(
						chain->isFullyVisible() ? E_VISIBILITY::VISIBLE
						: chain->isVisible()	? E_VISIBILITY::PARTIAL
												: E_VISIBILITY::HIDDEN,
						chainWidget
					);
					// Again.
					for ( int j = 0; j < chainWidget->childCount(); ++j )
					{
						QTreeWidgetItem * const residueWidget = chainWidget->child( j );
						WidgetData				residueId = residueWidget->data( 0, Qt::UserRole ).value<WidgetData>();
						const auto * const		residue	  = system->getResidue( residueId );
						_applyVisibility(
							residue->isFullyVisible() ? E_VISIBILITY::VISIBLE
							: residue->isVisible()	  ? E_VISIBILITY::PARTIAL
													  : E_VISIBILITY::HIDDEN,
							residueWidget
						);
						// Again.
						for ( int k = 0; k < residueWidget->childCount(); ++k )
						{
							QTreeWidgetItem * const atomWidget = residueWidget->child( k );
							WidgetData				atomId = atomWidget->data( 0, Qt::UserRole ).value<WidgetData>();
							const auto * const		atom   = system->getAtom( atom_index_t( atomId ) );
							_applyVisibility(
								atom->isVisible() ? E_VISIBILITY::VISIBLE : E_VISIBILITY::HIDDEN, atomWidget
							);
						}
					}
				}

				float elapsed = chrono.elapsedTime();
				VTX_TRACE( "Widget visibility update: {}", Util::String::durationToStr( elapsed ) );

				/*
				// Get residues to refresh.
				// Need a full atom loop...
				std::unordered_set<size_t> residues;
				std::unordered_set<size_t> chains;

				for ( const atom_index_t atomIndex : p_range )
				{
					auto * atom = system->getAtom( atomIndex );
					assert( atom );
					residues.insert( atom->getResiduePtr()->getIndex() );
					chains.insert( atom->getResiduePtr()->getChainPtr()->getIndex() );
				}
				*/
			};
		}
		else
		{
			_applyVisibility( p_data.visibility, item );
		}

		_tree->blockSignals( false );
	}

	void Scene::_resetTreeItem( QTreeWidgetItem * const p_item )
	{
		// Remove all children.
		while ( p_item->childCount() > 0 )
		{
			delete p_item->takeChild( 0 );
		}

		// Add placeholder children.
		QTreeWidgetItem * child = new QTreeWidgetItem( p_item );
		child->setText( 0, QString::fromUtf8( "Loading..." ) );
	}

	void Scene::_applyVisibility( const E_VISIBILITY p_visibility, QTreeWidgetItem * const p_item )
	{
		switch ( p_visibility )
		{
		case E_VISIBILITY::VISIBLE: p_item->setIcon( 1, QIcon( ":/sprite/visibility/full.png" ) ); break;
		case E_VISIBILITY::HIDDEN: p_item->setIcon( 1, QIcon( ":/sprite/visibility/hidden.png" ) ); break;
		case E_VISIBILITY::PARTIAL: p_item->setIcon( 1, QIcon( ":/sprite/visibility/partial.png" ) ); break;
		default: assert( true ); break;
		}

		p_item->setData( 1, Qt::UserRole, QVariant::fromValue( p_visibility ) );
	}

} // namespace VTX::UI::QT::DockWidget
