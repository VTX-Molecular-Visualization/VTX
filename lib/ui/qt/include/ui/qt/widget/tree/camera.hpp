#ifndef __VTX_UI_QT_WIDGET_TREE_CAMERA__
#define __VTX_UI_QT_WIDGET_TREE_CAMERA__

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
		Camera( QWidget * p_parent ) : BaseTree( p_parent )
		{
			setExpandsOnDoubleClick( true );

			addTopLevelItem( new QTreeWidgetItem( QStringList() << "Camera" ) );
			topLevelItem( 0 )->setExpanded( true );

			// Add fake cameras for the example.
			for ( int i = 0; i < 3; i++ )
			{
				QTreeWidgetItem * const cameraItem
					= new QTreeWidgetItem( QStringList() << QString( "Viewpoint %1" ).arg( i + 1 ) );
				topLevelItem( 0 )->addChild( cameraItem );
			}
		}
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
