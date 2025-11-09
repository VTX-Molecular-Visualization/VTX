#ifndef __VTX_UI_QT_DOCK_WIDGET_SCENE__
#define __VTX_UI_QT_DOCK_WIDGET_SCENE__

#include "ui/qt/dock_widget/base_dock_widget.hpp"
#include "ui/qt/widget/tree.hpp"
#include <QPointer>

namespace VTX::UI::QT::DockWidget
{

	/**
	 * @brief Display a tree widget with loaded systems.
	 * // TODO: Load only minimal data on expand/collapse.
	 */
	class Scene : public BaseDockWidget<Scene>
	{
	  public:
		Scene( QWidget * p_parent ) : BaseDockWidget( "Scene", p_parent )
		{
			_tree = new Widget::Tree( this );
			_tree->setModel( &MODEL() );
			_layout->addWidget( _tree );
		}

		QPointer<Widget::Tree> _tree;
		// TODO: add searchbar with QSortFilterProxyModel.
	};

} // namespace VTX::UI::QT::DockWidget

#endif
