#ifndef __VTX_UI_QT_WIDGET_TREE_CAMERA__
#define __VTX_UI_QT_WIDGET_TREE_CAMERA__

#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
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
		Camera( const App::Entity p_entity, QWidget * p_parent ) : BaseTree( p_parent ), _entity( p_entity )
		{
			setExpandsOnDoubleClick( true );

			addTopLevelItem( new QTreeWidgetItem( QStringList() << "Camera" ) );
			topLevelItem( 0 )->setIcon( 0, STYLE().iconFromCodepoint( Style::Icons::CAMERA ) );
		}

	  private:
		const App::Entity _entity;
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
