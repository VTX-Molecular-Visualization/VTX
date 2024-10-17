#ifndef __VTX_UI_QT_DOCK_WIDGET_SCENE__
#define __VTX_UI_QT_DOCK_WIDGET_SCENE__

#include "ui/qt/core/base_dock_widget.hpp"
#include <QDockWidget>
#include <QLineEdit>
#include <QPointer>
#include <QToolBar>
#include <QTreeWidget>
#include <app/application/scene.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/component/scene/scene_item_component.hpp>

namespace VTX::UI::QT::DockWidget
{

	struct Data
	{
		uint level;
	};

	// Static polymorphism.
	template<typename T>
	concept TreeItemData = requires( T t ) {
		{ t.getName() } -> std::same_as<const std::string_view>;
		{ t.getPersistentSceneID() } -> std::same_as<int>;
	};

	// Test structs.
	struct TestData
	{
		std::string_view name		  = "Default molecule";
		int				 persistentId = 0;

		const std::string_view getName() const { return name; }
		int					   getPersistentSceneID() const { return persistentId; }
	};

	struct TestData2
	{
		std::string_view myName = "Default molecule adapted";
		int				 myID	= 0;

		const std::string_view getMyName() const { return myName; }
		int					   getMyID() const { return myID; }
	};

	// Adapter.
	class BaseAdapterTreeItemData
	{
	  public:
		virtual const std::string_view name() const = 0;
		virtual int					   id() const	= 0;
		virtual ~BaseAdapterTreeItemData()			= default;
	};

	class AdapterSceneItemComponent : public BaseAdapterTreeItemData
	{
	  public:
		AdapterSceneItemComponent( const VTX::App::Component::Scene::SceneItemComponent & p_item ) : _item( p_item ) {}

		const std::string_view name() const override { return _item.getName(); }
		int					   id() const override { return _item.getPersistentSceneID(); }

	  private:
		const VTX::App::Component::Scene::SceneItemComponent & _item;
	};

	class AdapterTestData2 : public BaseAdapterTreeItemData
	{
	  public:
		AdapterTestData2( const TestData2 & p_item ) : _item( p_item ) {}

		const std::string_view name() const override { return _item.getMyName(); }
		int					   id() const override { return _item.getMyID(); }

	  private:
		const TestData2 & _item;
	};

	class Scene : public Core::BaseDockWidget<Scene>
	{
	  public:
		using WidgetData = size_t;
		struct TreeItemData
		{
			std::string_view name;
			WidgetData		 data;
			size_t			 childrenCount;
		};
		using LoadFunc = std::function<std::vector<TreeItemData>( const uint p_level, const WidgetData p_data )>;

		Scene( QWidget * p_parent ) : Core::BaseDockWidget<Scene>( "Scene", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

			// Search bar.
			auto * searchBar = new QLineEdit( _root );
			searchBar->setPlaceholderText( "Search..." );
			_layout->addWidget( searchBar );

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
			_tree->setExpandsOnDoubleClick( true );

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
					const auto		 data	  = loadFunc( depth, p_item->data( 0, Qt::UserRole ).value<WidgetData>() );
					for ( const auto & d : data )
					{
						// TODO: factorise with top level.
						QTreeWidgetItem * child = new QTreeWidgetItem( p_item );
						child->setText(
							0, QString::fromStdString( d.name.data() ).append( " (%1)" ).arg( d.childrenCount )
						);

						child->setData( 0, Qt::UserRole, QVariant::fromValue( d.data ) );

						if ( d.childrenCount != 0 )
						{
							_resetTreeItem( child );
						}
					}
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
			_tree->connect(
				_tree.get(), &QTreeWidget::customContextMenuRequested, this, []( const QPoint & p_pos ) {}
			);

			// Connect callbacks.
			using namespace App::Component::Scene;
			App::SCENE().onSceneItemAdded += [ this ]( const SceneItemComponent & p_item )
			{
				using namespace App;

				if ( App::ECS_REGISTRY().hasComponent<App::Component::Chemistry::Molecule>( p_item ) )
				{
					auto & molecule = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Molecule>( p_item );

					// Add with concept.
					addTopLevelData(
						TreeItemData { p_item.getName(),
									   WidgetData( p_item.getPersistentSceneID() ),
									   molecule.getChains().size() },
						LoadFunc(
							[ &p_item,
							  &molecule ]( const uint p_level, const WidgetData p_data ) -> std::vector<TreeItemData>
							{
								std::vector<TreeItemData> data;

								switch ( p_level )
								{
								case 0: // Load chains.
								{
									WidgetData index = 0;
									for ( auto & chain : molecule.getChains() )
									{
										data.push_back( TreeItemData {
											chain->getName(), index++, chain->getResidueCount() } );
									}
								}
								break;

								case 1: // Load residues.
								{
									auto * chain = molecule.getChain( p_data );
									assert( chain );
									for ( size_t index = chain->getIndexFirstResidue();
										  index <= chain->getIndexLastResidue();
										  ++index )
									{
										auto * residue = molecule.getResidue( index );
										data.push_back( TreeItemData {
											residue->getName(), index, residue->getAtomCount() } );
									}
								}
								break;

								case 2: // Load atoms.
								{
									auto * residue = molecule.getResidue( p_data );
									assert( residue );
									for ( size_t index = residue->getIndexFirstAtom();
										  index <= residue->getIndexLastAtom();
										  ++index )
									{
										auto * atom = molecule.getAtom( atom_index_t( index ) );
										data.push_back( TreeItemData { atom->getName(), index, 0 } );
									}
								}
								break;

								default: assert( true ); break;
								}

								return data;
							}
						)
					);
				}
			};

			_layout->addWidget( _tree.get() );
		}

		void addTopLevelData( const TreeItemData & p_data, const LoadFunc & p_loadFunc )
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

	  private:
		QPointer<QTreeWidget>				  _tree;
		std::map<QTreeWidgetItem *, LoadFunc> _loadFuncs;

		void _resetTreeItem( QTreeWidgetItem * const p_item )
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
	};

} // namespace VTX::UI::QT::DockWidget

#endif
