#include "ui/qt/widget/scene_tree/item.hpp"

namespace VTX::UI::QT::Widget::SceneTree
{

	Item::Item( QVariantList data, Item * parent ) : m_itemData( std::move( data ) ), m_parentItem( parent )
	{
	}

	void Item::appendChild( std::unique_ptr<Item> && child ) { m_childItems.push_back( std::move( child ) ); }

	Item * Item::child( int row ) { return row >= 0 && row < childCount() ? m_childItems.at( row ).get() : nullptr; }

	int Item::childCount() const { return int( m_childItems.size() ); }

	int Item::row() const
	{
		if ( m_parentItem == nullptr )
			return 0;
		const auto it = std::find_if(
			m_parentItem->m_childItems.cbegin(),
			m_parentItem->m_childItems.cend(),
			[ this ]( const std::unique_ptr<Item> & treeItem ) { return treeItem.get() == this; }
		);

		if ( it != m_parentItem->m_childItems.cend() )
			return std::distance( m_parentItem->m_childItems.cbegin(), it );
		Q_ASSERT( false ); // should not happen
		return -1;
	}

	int Item::columnCount() const { return int( m_itemData.count() ); }

	QVariant Item::data( int column ) const { return m_itemData.value( column ); }

	Item * Item::parentItem() { return m_parentItem; }
} // namespace VTX::UI::QT::Widget::SceneTree
