#ifndef __VTX_UI_QT_DOCK_WIDGET_SCENE__
#define __VTX_UI_QT_DOCK_WIDGET_SCENE__

#include "ui/qt/dock_widget/base_dock_widget.hpp"
#include "ui/qt/events.hpp"
#include "ui/qt/widget/tree/camera.hpp"
#include "ui/qt/widget/tree/system.hpp"
#include <QComboBox>
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
		Scene( QWidget * p_parent );

	  private:
		/**
		 * @brief Camera tree.
		 */
		QPointer<Widget::Tree::Camera> _treeCamera;

		/**
		 * @brief Scene tree.
		 */
		std::unordered_map<App::ECS::Entity, QPointer<Widget::Tree::System>> _map;
		QPointer<Widget::Tree::System>										 _tree;
		// TODO: add searchbar with QSortFilterProxyModel.
		// TODO: add option to sync tree expand with selection model.

		void _onSelectionLocked( const Events::SelectionLocked & );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
