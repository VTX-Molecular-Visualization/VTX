#include "ui/qt/widget/scene_tree/model.hpp"

namespace VTX::UI::QT::Widget::SceneTree
{

	Model::Model( const QString & data, QObject * parent ) :
		QAbstractItemModel( parent ),
		rootItem( std::make_unique<Item>( QVariantList { tr( "Title" ), tr( "Summary" ) } ) )
	{
		setupModelData( QStringView { data }.split( u'\n' ), rootItem.get() );
	}

	Model::~Model() = default;

	QModelIndex Model::index( int row, int column, const QModelIndex & parent ) const
	{
		if ( !hasIndex( row, column, parent ) )
			return {};

		Item * parentItem = parent.isValid() ? static_cast<Item *>( parent.internalPointer() ) : rootItem.get();

		if ( auto * childItem = parentItem->child( row ) )
			return createIndex( row, column, childItem );
		return {};
	}

	QModelIndex Model::parent( const QModelIndex & index ) const
	{
		if ( !index.isValid() )
			return {};

		auto * childItem  = static_cast<Item *>( index.internalPointer() );
		Item * parentItem = childItem->parentItem();

		return parentItem != rootItem.get() ? createIndex( parentItem->row(), 0, parentItem ) : QModelIndex {};
	}

	int Model::rowCount( const QModelIndex & parent ) const
	{
		if ( parent.column() > 0 )
			return 0;

		const Item * parentItem
			= parent.isValid() ? static_cast<const Item *>( parent.internalPointer() ) : rootItem.get();

		return parentItem->childCount();
	}

	int Model::columnCount( const QModelIndex & parent ) const
	{
		if ( parent.isValid() )
			return static_cast<Item *>( parent.internalPointer() )->columnCount();
		return rootItem->columnCount();
	}

	QVariant Model::data( const QModelIndex & index, int role ) const
	{
		if ( !index.isValid() || role != Qt::DisplayRole )
			return {};

		const auto * item = static_cast<const Item *>( index.internalPointer() );
		return item->data( index.column() );
	}

	Qt::ItemFlags Model::flags( const QModelIndex & index ) const
	{
		return index.isValid() ? QAbstractItemModel::flags( index ) : Qt::ItemFlags( Qt::NoItemFlags );
	}

	QVariant Model::headerData( int section, Qt::Orientation orientation, int role ) const
	{
		return orientation == Qt::Horizontal && role == Qt::DisplayRole ? rootItem->data( section ) : QVariant {};
	}
} // namespace VTX::UI::QT::Widget::SceneTree
