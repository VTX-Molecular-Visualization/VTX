#include "ui/qt/dock_widget/scene.hpp"
#include <app/action/animation.hpp>
#include <app/action/visibility.hpp>
#include <app/application/scene.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/component/scene/scene_item_component.hpp>

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
		
		auto * trajManager = new Widget::TrajectoryManager( this );
		_layout->addWidget( trajManager );
		
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

		_tree->setColumnCount( 1 );
		_tree->setUniformRowHeights( true );
		_tree->setHeaderHidden( true );
		_tree->setSelectionMode( QAbstractItemView::ExtendedSelection );
		//_tree->setSelectionBehavior( SelectionBehavior::SelectRows );
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
				auto [ depth, topLevelItem ] = _getDepth( p_item );

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
								_addTreeItem( { chain->getName(), index++, chain->getResidueCount() }, p_item );
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
								_addTreeItem( { residue->getName(), index, residue->getAtomCount() }, p_item );
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
								_addTreeItem( { atom->getName(), index, 0 }, p_item );
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

		// itemChanged.
		connect(
			_tree,
			&QTreeWidget::itemChanged,
			this,
			[ this ]( QTreeWidgetItem * const p_item, const int p_column )
			{
				if ( p_column == 0 )
				{
					const bool checked			   = p_item->checkState( 0 ) == Qt::Checked;
					WidgetData widgetData		   = p_item->data( 0, Qt::UserRole ).value<WidgetData>();
					auto [ depth, topLevelWidget ] = _getDepth( p_item );

					assert( _systemComponents.contains( topLevelWidget ) );

					using namespace App::Action::Visibility;
					switch ( depth )
					{
					case E_DEPTH::SYSTEM:
					{
						App::ACTION_SYSTEM().execute<SetVisible<E_ITEM_TYPE::SYSTEM>>(
							_systemComponents.at( topLevelWidget ), checked, std::nullopt
						);
						break;
					}
					case E_DEPTH::CHAIN:
					{
						App::ACTION_SYSTEM().execute<SetVisible<E_ITEM_TYPE::CHAIN>>(
							_systemComponents.at( topLevelWidget ), checked, widgetData
						);
						break;
					}
					case E_DEPTH::RESIDUE:
					{
						App::ACTION_SYSTEM().execute<SetVisible<E_ITEM_TYPE::RESIDUE>>(
							_systemComponents.at( topLevelWidget ), checked, widgetData
						);
						break;
					}
					case E_DEPTH::ATOM:
					{
						App::ACTION_SYSTEM().execute<SetVisible<E_ITEM_TYPE::ATOM>>(
							_systemComponents.at( topLevelWidget ), checked, widgetData
						);
						break;
					}
					default: assert( true ); break;
					}
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
				WidgetData widgetData		   = p_item->data( 0, Qt::UserRole ).value<WidgetData>();
				auto [ depth, topLevelWidget ] = _getDepth( p_item );

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
		QTreeWidgetItem * item = new QTreeWidgetItem( p_parent );
		Qt::ItemFlags flags	   = Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsUserCheckable | Qt::ItemIsEnabled;

		item->setFlags( flags );
		item->setData( 0, Qt::UserRole, QVariant::fromValue( p_data.data ) );
		// item->setText( 0, QString::fromUtf8( p_data.name ) );
		item->setText( 0, QString::fromStdString( p_data.name.data() ).append( " (%1)" ).arg( p_data.childrenCount ) );
		// item->setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( model.getTypeId() ) );
		item->setCheckState( 0, Qt::Checked );

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

			_loadFuncs.emplace( item, p_loadFunc.value() );
			_systemComponents.emplace( item, p_systemComponent.value() );

			_tree->addTopLevelItem( item );
		}
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

} // namespace VTX::UI::QT::DockWidget
