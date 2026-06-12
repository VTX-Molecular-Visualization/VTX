#ifndef __VTX_UI_QT_WIDGET_TREE_CAMERA__
#define __VTX_UI_QT_WIDGET_TREE_CAMERA__

#include "ui/qt/widget/tree/base_tree.hpp"
#include <unordered_map>
#include <util/ecs.hpp>

class QTreeWidgetItem;

namespace VTX::App::Events
{
	struct ViewPointAdded;
	struct ViewPointDeleted;
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
		Camera( const Entity p_entity, QWidget * p_parent );
		~Camera() override;

	  private:
		/**
		 * @brief Camera entitie.
		 */
		const Entity _entity;

		/**
		 * @brief Map viewpoint entities to tree items.
		 */
		std::unordered_map<Entity, QTreeWidgetItem *> _entityToItemMap;

		/**
		 * @brief Handle viewpoint addition to update the tree.
		 */
		void _onViewPointAdded( const App::Events::ViewPointAdded & p_event );

		/**
		 * @brief Handle viewpoint deletion to update the tree.
		 */
		void _onViewPointDeleted( const App::Events::ViewPointDeleted & p_event );
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
