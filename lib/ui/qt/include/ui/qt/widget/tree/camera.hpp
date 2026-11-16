#ifndef VTX_UI_QT_WIDGET_TREE_CAMERA_HPP
#define VTX_UI_QT_WIDGET_TREE_CAMERA_HPP

#include "ui/qt/widget/tree/base_tree.hpp"
#include <unordered_map>
#include <util/ecs.hpp>
#include <util/event_hub.hpp>

class QTreeWidgetItem;

namespace VTX::App::Events
{
	struct ViewPointRenamed;
} // namespace VTX::App::Events

namespace VTX::UI::QT::Widget::Tree
{
	/**
	 * @brief Camera tree: list of saved viewpoints.
	 */
	class Camera : public Widget::Tree::BaseTree<Camera>
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		Camera( Entity p_entity, QWidget * p_parent );

	  private:
		/**
		 * @brief Map viewpoint entities to tree items.
		 */
		std::unordered_map<Entity, QTreeWidgetItem *> _entityToItemMap;

		/**
		 * @brief Connections to App.
		 */
		VTX::Util::EventHub::ScopedConnection _onConstructConnection;
		VTX::Util::EventHub::ScopedConnection _onDestroyConnection;

		/**
		 * @brief Handle viewpoint addition to update the tree.
		 */
		void _addViewPoint( Registry &, Entity );

		/**
		 * @brief Handle viewpoint deletion to update the tree.
		 */
		void _removeViewPoint( Registry &, Entity );

		/**
		 * @brief Handle viewpoint rename to update the tree.
		 */
		void _onViewPointRenamed( const App::Events::ViewPointRenamed & p_event );

		VTX::Util::EventHub::ScopedConnection _viewPointRenamedConnection;
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
