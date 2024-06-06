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
		// itemExpanded.
		connect(
			this,
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
		connect(
			this,
			&QTreeWidget::itemCollapsed,
			this,
			[ this ]( QTreeWidgetItem * const p_item ) { _resetTreeItem( p_item ); }
		);

		// customContextMenuRequested.
		connect( this, &QTreeWidget::customContextMenuRequested, this, []( const QPoint & p_pos ) {} );
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
