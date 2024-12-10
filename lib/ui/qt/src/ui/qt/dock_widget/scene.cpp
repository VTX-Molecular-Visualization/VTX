#include "ui/qt/dock_widget/scene.hpp"
#include <app/application/scene.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/component/chemistry/system.hpp>
#include <app/component/scene/scene_item_component.hpp>

namespace VTX::UI::QT::DockWidget
{

	Scene::Scene( QWidget * p_parent ) : Core::BaseDockWidget<Scene>( "Scene", p_parent )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

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
				int				  depth	 = 0;
				QTreeWidgetItem * parent = p_item;
				while ( parent->parent() )
				{
					++depth;
					parent = parent->parent();
				}

				assert( _loadFuncs.contains( parent ) );

				// Remove placeholder.
				assert( p_item->childCount() == 1 );
				delete p_item->takeChild( 0 );

				// Load children.
				const LoadFunc & loadFunc = _loadFuncs.at( parent );
				loadFunc( depth, p_item );
			}
		);

		// itemCollapsed.
		_tree->connect(
			_tree.get(),
			&QTreeWidget::itemCollapsed,
			this,
			[ this ]( QTreeWidgetItem * const p_item ) { _resetTreeItem( p_item ); }
		);

		// customContextMenuRequested.
		_tree->connect( _tree.get(), &QTreeWidget::customContextMenuRequested, this, []( const QPoint & p_pos ) {} );

		// Connect callbacks.
		using namespace App::Component::Scene;
		App::SCENE().onSceneItemAdded += [ this ]( const SceneItemComponent & p_itemComponent )
		{
			using namespace App;

			if ( App::ECS_REGISTRY().hasComponent<App::Component::Chemistry::System>( p_itemComponent ) )
			{
				auto & system = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::System>( p_itemComponent );

				// Add with concept.
				_addTopLevelData(
					TreeItemData { p_itemComponent.getName(),
								   WidgetData( p_itemComponent.getPersistentSceneID() ),
								   system.getChains().size() },
					LoadFunc(
						[ this, &system ]( const uint p_level, QTreeWidgetItem * const p_item )
						{
							WidgetData parentWidgetData = p_item->data( 0, Qt::UserRole ).value<WidgetData>();

							switch ( p_level )
							{
							case 0: // Load chains.
							{
								WidgetData index = 0;
								for ( auto & chain : system.getChains() )
								{
									_addChildLevelData(
										p_item, { chain->getName(), index++, chain->getResidueCount() }
									);
								}
							}
							break;

							case 1: // Load residues.
							{
								auto * chain = system.getChain( parentWidgetData );
								assert( chain );
								for ( size_t index = chain->getIndexFirstResidue();
									  index <= chain->getIndexLastResidue();
									  ++index )
								{
									auto * residue = system.getResidue( index );
									_addChildLevelData(
										p_item, { residue->getName(), index, residue->getAtomCount() }
									);
								}
							}
							break;

							case 2: // Load atoms.
							{
								auto * residue = system.getResidue( parentWidgetData );
								assert( residue );
								for ( size_t index = residue->getIndexFirstAtom(); index <= residue->getIndexLastAtom();
									  ++index )
								{
									auto * atom = system.getAtom( atom_index_t( index ) );
									_addChildLevelData( p_item, { atom->getName(), index, 0 } );
								}
							}
							break;

							default: assert( true ); break;
							}
						}
					)
				);
			}
		};

		_layout->addWidget( _tree.get() );
	}

	void Scene::_addTopLevelData( const TreeItemData & p_data, const LoadFunc & p_loadFunc )
	{
		QTreeWidgetItem * item = new QTreeWidgetItem();

		auto * p = item->parent();

		Qt::ItemFlags flags = Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsUserCheckable;

		item->setFlags( flags );
		item->setData( 0, Qt::UserRole, QVariant::fromValue( p_data.data ) );
		item->setText( 0, QString::fromUtf8( p_data.name ) );
		// item->setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( model.getTypeId() ) );
		item->setCheckState( 0, Qt::Checked );

		if ( p_data.childrenCount != 0 )
		{
			_resetTreeItem( item );
		}

		assert( not _loadFuncs.contains( item ) );
		_loadFuncs.emplace( item, p_loadFunc );

		_tree->addTopLevelItem( item );
	}

	void Scene::_addChildLevelData( QTreeWidgetItem * const p_item, const TreeItemData & p_data )
	{
		QTreeWidgetItem * child = new QTreeWidgetItem( p_item );
		child->setText( 0, QString::fromStdString( p_data.name.data() ).append( " (%1)" ).arg( p_data.childrenCount ) );

		child->setData( 0, Qt::UserRole, QVariant::fromValue( p_data.data ) );

		if ( p_data.childrenCount != 0 )
		{
			_resetTreeItem( child );
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
