#ifndef __VTX_UI_QT_WIDGET_TREE_CAMERA__
#define __VTX_UI_QT_WIDGET_TREE_CAMERA__

#include "ui/qt/services.hpp"
#include "ui/qt/style.hpp"
#include "ui/qt/widget/tree/base_tree.hpp"
#include <QTreeWidget>

namespace VTX::UI::QT::Widget::Tree
{
	/**
	 * @brief Camera tree: list of saved viewpoints.
	 */
	class Camera : public Widget::Tree::BaseTree<Camera>
	{
	  public:
		Camera( const App::ECS::Entity p_entity, QWidget * p_parent ) : BaseTree( p_parent ), _entity( p_entity )
		{
			setExpandsOnDoubleClick( true );

			addTopLevelItem( new QTreeWidgetItem( QStringList() << "Camera" ) );
			topLevelItem( 0 )->setIcon( 0, STYLE().iconFromCodepoint( 0xe8fc ) );

			topLevelItem( 0 )->addChild( new QTreeWidgetItem( QStringList() << "Front" ) );
			topLevelItem( 0 )->addChild( new QTreeWidgetItem( QStringList() << "Back" ) );
			topLevelItem( 0 )->addChild( new QTreeWidgetItem( QStringList() << "Left" ) );
			topLevelItem( 0 )->addChild( new QTreeWidgetItem( QStringList() << "Right" ) );
			topLevelItem( 0 )->addChild( new QTreeWidgetItem( QStringList() << "Top" ) );
			topLevelItem( 0 )->addChild( new QTreeWidgetItem( QStringList() << "Bottom" ) );
		}

	  private:
		const App::ECS::Entity _entity;
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
