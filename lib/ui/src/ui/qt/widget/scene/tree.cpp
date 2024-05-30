#include "ui/qt/widget/scene/tree.hpp"

namespace VTX::UI::QT::Widget::Scene
{
	Tree::Tree( QWidget * p_parent ) {}

	void Tree::localize() {};

	void Tree::_setupUi( const QString & p_name )
	{
		setColumnCount( 1 );
		setUniformRowHeights( true );
		setHeaderHidden( true );
		setSelectionMode( QAbstractItemView::ExtendedSelection );
		setSelectionBehavior( SelectionBehavior::SelectRows );
		setSizeAdjustPolicy( QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents );
		setSizePolicy( QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Minimum );
		setHorizontalScrollBarPolicy( Qt::ScrollBarPolicy::ScrollBarAlwaysOff );
		setVerticalScrollBarPolicy( Qt::ScrollBarPolicy::ScrollBarAlwaysOff );
		setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );
		setEditTriggers( EditTrigger::SelectedClicked );
		setExpandsOnDoubleClick( true );
		// TODO: setSelectionModel
	}

	void Tree::_setupSlots()
	{
		connect(
			this,
			&QTreeWidget::itemExpanded,
			this,
			[ & ]( QTreeWidgetItem * const p_item )
			{
				// Remove placeholder.
				assert( p_item->childCount() == 1 );
				delete p_item->takeChild( 0 );

				// Add 5 placeholder children.
				for ( int i = 0; i < 5; ++i )
				{
					QTreeWidgetItem * child = new QTreeWidgetItem( p_item );
					_resetTreeItem( child );
					child->setText( 0, QString::fromUtf8( "Item" ) );
				}
			}
		);

		connect(
			this,
			&QTreeWidget::itemCollapsed,
			this,
			[ this ]( QTreeWidgetItem * const p_item ) { _resetTreeItem( p_item ); }
		);

		connect( this, &QTreeWidget::customContextMenuRequested, this, []( const QPoint & p_pos ) {} );
	}

	void Tree::addTopLevelMolecule( const App::Component::Scene::SceneItemComponent & p_item )
	{
		QTreeWidgetItem * item = new QTreeWidgetItem();

		static Qt::ItemFlags flags = Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsUserCheckable;

		item->setFlags( flags );
		item->setData( 0, Qt::UserRole, QVariant::fromValue( p_item.getPersistentSceneID() ) );
		item->setText( 0, QString::fromUtf8( p_item.getName() ) );
		// item->setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( model.getTypeId() ) );
		item->setCheckState( 0, Qt::Checked );

		_resetTreeItem( item );
		addTopLevelItem( item );
	}

	void Tree::_resetTreeItem( QTreeWidgetItem * const p_item )
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

} // namespace VTX::UI::QT::Widget::Scene
