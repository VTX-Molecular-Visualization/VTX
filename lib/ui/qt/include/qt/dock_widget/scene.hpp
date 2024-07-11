#ifndef __VTX_UI_QT_DOCK_WIDGET_SCENE__
#define __VTX_UI_QT_DOCK_WIDGET_SCENE__

#include "qt/base_widget.hpp"
#include <QDockWidget>
#include <QPointer>
#include <QTreeWidget>
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
		{ t.getName() } -> std::same_as<const std::string &>;
		{ t.getPersistentSceneID() } -> std::same_as<int>;
	};

	// Test structs.
	struct TestData
	{
		std::string name		 = "Default molecule";
		int			persistentId = 0;

		const std::string & getName() const { return name; }
		int					getPersistentSceneID() const { return persistentId; }
	};

	struct TestData2
	{
		std::string myName = "Default molecule adapted";
		int			myID   = 0;

		const std::string & getMyName() const { return myName; }
		int					getMyID() const { return myID; }
	};

	// Adapter.
	class BaseAdapterTreeItemData
	{
	  public:
		virtual const std::string & name() const = 0;
		virtual int					id() const	 = 0;
		virtual ~BaseAdapterTreeItemData()		 = default;
	};

	class AdapterSceneItemComponent : public BaseAdapterTreeItemData
	{
	  public:
		AdapterSceneItemComponent( const VTX::App::Component::Scene::SceneItemComponent & p_item ) : _item( p_item ) {}

		const std::string & name() const override { return _item.getName(); }
		int					id() const override { return _item.getPersistentSceneID(); }

	  private:
		const VTX::App::Component::Scene::SceneItemComponent & _item;
	};

	class AdapterTestData2 : public BaseAdapterTreeItemData
	{
	  public:
		AdapterTestData2( const TestData2 & p_item ) : _item( p_item ) {}

		const std::string & name() const override { return _item.getMyName(); }
		int					id() const override { return _item.getMyID(); }

	  private:
		const TestData2 & _item;
	};

	class Scene : public BaseWidget<Scene, QDockWidget>
	{
	  public:
		using WidgetData = size_t;
		struct TreeItemData
		{
			std::string name;
			WidgetData	data;
			size_t		childrenCount;
		};
		using LoadFunc = std::function<std::vector<TreeItemData>( const uint p_level, const WidgetData p_data )>;

		Scene( QWidget * p_parent ) : BaseWidget<Scene, QDockWidget>( "Scene", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

			// Setup tree.
			_tree = new QTreeWidget( this );

			_tree->setColumnCount( 1 );
			_tree->setUniformRowHeights( true );
			_tree->setHeaderHidden( true );
			_tree->setSelectionMode( QAbstractItemView::ExtendedSelection );
			_tree->setRootIsDecorated( true );
			//_tree->setItemsExpandable( true );
			_tree->setAnimated( true );
			_tree->setIndentation( 20 );
			_tree->setSortingEnabled( false );
			_tree->setWordWrap( true );
			_tree->setAlternatingRowColors( true );
			_tree->setAutoScroll( true );
			_tree->setAutoScrollMargin( 16 );
			_tree->setAutoFillBackground( true );
			//_tree->setFrameStyle( QFrame::Panel | QFrame::Sunken );
			_tree->setLineWidth( 1 );
			_tree->setMidLineWidth( 0 );
			_tree->setFocusPolicy( Qt::FocusPolicy::StrongFocus );
			_tree->setTabKeyNavigation( false );
			_tree->setAcceptDrops( false );
			_tree->setDropIndicatorShown( false );
			_tree->setDragEnabled( false );
			_tree->setDragDropMode( QAbstractItemView::DragDropMode::NoDragDrop );
			_tree->setSizeAdjustPolicy( QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents );
			//_tree->setSizePolicy( QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Minimum );
			_tree->setHorizontalScrollBarPolicy( Qt::ScrollBarPolicy::ScrollBarAlwaysOff );
			_tree->setVerticalScrollBarPolicy( Qt::ScrollBarPolicy::ScrollBarAlwaysOff );
			_tree->setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );
			_tree->setExpandsOnDoubleClick( true );
			// TODO: setSelectionModel

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
						child->setText( 0, QString::fromUtf8( d.name + " (%1)" ).arg( d.childrenCount ) );
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

			// Set placeholder data.
			QTreeWidgetItem * item = new QTreeWidgetItem( _tree.get() );
			item->setText( 0, QString::fromUtf8( "Loading..." ) );
			_tree->addTopLevelItem( item );

			setWidget( _tree.get() );
		}

		virtual ~Scene() {}

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
