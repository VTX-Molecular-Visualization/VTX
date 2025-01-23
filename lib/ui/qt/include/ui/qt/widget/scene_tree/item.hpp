#ifndef __VTX_UI_QT_WIDGET_SCENE_TREE_ITEM__
#define __VTX_UI_QT_WIDGET_SCENE_TREE_ITEM__

#include <QVariant>

namespace VTX::UI::QT::Widget::SceneTree
{
	class Item
	{
	  public:
		explicit Item( QVariantList data, Item * parentItem = nullptr );

		void appendChild( std::unique_ptr<Item> && child );

		Item *	 child( int );
		int		 childCount() const;
		int		 columnCount() const;
		QVariant data( int ) const;
		int		 row() const;
		Item *	 parentItem();

	  private:
		std::vector<std::unique_ptr<Item>> m_childItems;
		QVariantList					   m_itemData;
		Item *							   m_parentItem;
	};
} // namespace VTX::UI::QT::Widget::SceneTree

#endif
